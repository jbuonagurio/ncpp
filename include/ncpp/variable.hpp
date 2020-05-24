// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_VARIABLE_HPP
#define NCPP_VARIABLE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#if defined(NCPP_USE_BOOST) && defined(_MSVC_LANG) && _MSVC_LANG >= 201402L
#pragma warning(push) 
#pragma warning(disable:4996) // disable C++17 depreciation warnings with boost::multi_array
#endif // defined(NCPP_USE_BOOST) && defined(_MSVC_LANG) && _MSVC_LANG >= 201402L

#include <netcdf.h>
#include <netcdf_meta.h>

#include <ncpp/config.hpp>

#include <ncpp/detail/utilities.hpp>
#include <ncpp/functions/variable.hpp>
#include <ncpp/functions/ndarray.hpp>
#include <ncpp/attributes.hpp>
#include <ncpp/dimensions.hpp>
#include <ncpp/selection.hpp>
#include <ncpp/check.hpp>

#include <algorithm>
#include <array>
#include <functional>
#include <iterator>
#include <memory>
#include <numeric>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <valarray>
#include <vector>

#ifdef NCPP_USE_BOOST
// Disable global objects boost::extents and boost::indices
#ifndef BOOST_MULTI_ARRAY_NO_GENERATORS
#define BOOST_MULTI_ARRAY_NO_GENERATORS 1
#endif // !BOOST_MULTI_ARRAY_NO_GENERATORS
#include <boost/multi_array.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#endif // NCPP_USE_BOOST

// TODO:
// - Account for scale_factor, add_offset
// - Use optional for _FillValue, missing_value, valid_min, valid_max, valid_range
// - Chained coordinates lookup for other variables indexed on the coordinate dimension
//   with an instance_dimension attribute

namespace ncpp {

class variables_type;

/// netCDF variable type.
class variable
{
    friend class variables_type;

public:
    variable(int ncid, int varid) :
        dims(ncid, varid), atts(ncid, varid), ncid_(ncid), varid_(varid)
    {
        start_.resize(dims.size(), 0);
        shape_.resize(dims.size(), 0);
        stride_.resize(dims.size(), 1);
        std::transform(dims.begin(), dims.end(), shape_.begin(),
            [](const auto& dim) { return dim.length(); });
    }

    /// Dimensions associated with the variable.
    dimensions_type dims;

    /// Attributes associated with the variable.
    attributes_type atts;

    variable(const variable& rhs) = default;
    variable(variable&& rhs) = default;
    variable& operator=(const variable& rhs) = default;
    variable& operator=(variable&& rhs) = default;

    bool operator<(const variable& rhs) const {
        return (varid_ < rhs.varid_);
    }

    bool operator==(const variable& rhs) const {
        return (ncid_ == rhs.ncid_ && varid_ == rhs.varid_);
    }

    bool operator!=(const variable& rhs) const {
        return !(*this == rhs);
    }

    /// Get the variable name.
    std::string name() const {
        return api::inq_varname(ncid_, varid_);
    }

    /// Get the netCDF ID.
    int ncid() const {
        return ncid_;
    }

    /// Get the variable ID.
    int varid() const {
        return varid_;
    }

    /// Get the netCDF data type ID for the variable.
    int netcdf_type() const {
        return api::inq_vartype(ncid_, varid_);
    }

    /// Get the start indexes of the data array.
    const index_type& start() const {
        return start_;
    }

    /// Get the shape of the data array.
    const index_type& shape() const {
        return shape_;
    }
    
    /// Get the strides of the data array.
    const stride_type& stride() const {
        return stride_;
    }

    /// Get the total number of elements in the data array.
    std::size_t size() const {
        return api::compute_size(shape_);
    }

    template <class T>
    std::optional<T> fill_value() const {
        return api::inq_var_fill(ncid_, varid_);
    }

    /// Returns the variable storage type.
    var_storage_type storage_type() const {
        return api::inq_var_storage(ncid_, varid_).value();
    }

    /// Returns the chunk size for each dimension.
    std::vector<std::size_t> chunk_sizes() const {
        return api::inq_var_chunksizes(ncid_, varid_);
    }

#ifdef NC_HAS_HDF5

    /// Returns the HDF5 filter ID for the variable.
    /// See also: https://portal.hdfgroup.org/display/support/Filters
    unsigned int filter_type() const {
        return api::inq_var_filter_id(ncid_, varid_).value();
    }

    /// Returns the HDF5 filter name for the variable.
    /// See also: https://portal.hdfgroup.org/display/support/Filters
    std::string filter_name() const {
        return api::inq_var_filter_name(ncid_, varid_);
    }

#endif // NC_HAS_HDF5

    /// \group select
    /// Select a subset of the data array by coordinate range for one or more dimensions.
    template <typename... Ts>
    variable select(selection<Ts>&&... selections) const
    {
        variable v(*this);
        
        // Return a copy of the variable with updated start, shape and stride.
        auto tuple = std::make_tuple(std::forward<selection<Ts>>(selections)...);
        detail::apply_index([&] (auto i, auto&& s) {
            v = v.select(s);
        }, tuple);
        
        return v;
    }
    
    /// \group select
    template <class T>
    variable select(selection<T>& s) const
    {
        // Read the coordinate variable.
        std::size_t idx = coordinate_position(s.coordinate);
        int cvarid = dims.at(idx).cvarid_;
        variable cv(ncid_, cvarid);
        auto coords = cv.values<T>();
        
        // Handle decreasing values.
        bool reversed = false;
        if (!std::is_sorted(coords.begin(), coords.end())) {
            std::reverse(coords.begin(), coords.end());
            reversed = true;
        }
        
        if (s.min_value > s.max_value)
            std::swap(s.min_value, s.max_value);
        
        // Set the start and shape indexes.
        auto lower = std::lower_bound(coords.begin(), coords.end(), s.min_value);
        auto upper = std::upper_bound(coords.begin(), coords.end(), s.max_value);
        
        variable v(*this);
        v.start_.at(idx) = reversed ? static_cast<std::size_t>(std::distance(upper, coords.end()))
                                    : static_cast<std::size_t>(std::distance(coords.begin(), lower));
        
        v.shape_.at(idx) = static_cast<std::size_t>(std::distance(lower, upper)) / std::abs(s.stride);
        v.stride_.at(idx) = s.stride;
        return v;
    }

    /// Returns a vector with one variable for each consecutive equal value
    /// range in the coordinate variable. Stride will be reset to 1 in the
    /// coordinate variable dimension.
    template <class T>
    std::vector<std::pair<T, variable>> group_by(const std::string& coordvarname)
    {
        std::vector<std::pair<T, variable>> result;
        std::size_t idx = coordinate_position(coordvarname);
        const auto coords = coordinates<T>(idx);

        // Group all adjacent equal elements.
        for (auto lower = coords.begin(), end = coords.end(); lower != end; /**/) {
            auto upper = std::adjacent_find(lower, end, std::not_equal_to<>{});
            if (upper != end)
                ++upper;

            variable v(*this);
            v.start_.at(idx) = static_cast<std::size_t>(std::distance(coords.begin(), lower));
            v.shape_.at(idx) = static_cast<std::size_t>(std::distance(lower, upper));
            v.stride_.at(idx) = 1;
            result.emplace_back(std::make_pair(*lower, v));

            lower = upper;
        }

        return result;
    }

    /// Change the coordinate variable for a dimension. The new variable must
    /// be one-dimensional (two-dimensional for classic strings) and have the
    /// same dimension. This is also known as an auxiliary coordinate variable
    /// (CF) or non-dimension coordinate variable (xarray).
    void set_coordinate(const std::string& dimname, const std::string& coordvarname)
    {
        // Get the associated dimension.
        auto it = std::find(dims.begin(), dims.end(), dims[dimname]);
        if (it == dims.end())
            detail::throw_error(error::invalid_dimension);

        dimension& dim = *it;

        int cvarid = api::inq_varid(ncid_, coordvarname).value_or(-1);
        if (cvarid == -1)
            detail::throw_error(error::variable_not_found);
        
        int cvartype = api::inq_vartype(ncid_, cvarid);
        auto cvardimids = api::inq_vardimid(ncid_, cvarid);

        // Ensure the variable is one-dimensional; allow two dimensions for classic strings.
        if ((cvartype != NC_CHAR && cvardimids.size() != 1) ||
            (cvartype == NC_CHAR && cvardimids.size() > 2))
            detail::throw_error(error::invalid_dimension_size);

        // Ensure the variable is indexed by this dimension.
        if (cvardimids.at(0) != dim.dimid())
            detail::throw_error(error::invalid_dimension);
        
        // Update the coordinate variable ID for the dimension.
        dim.cvarid_ = cvarid;
    }

    /// Get the coordinates for all dimensions as a vector of tuples.
    template <class... Ts>
    std::vector<std::tuple<Ts...>> coordinates() const
    {
        // Make sure we have the correct number of columns.
        if (sizeof...(Ts) != dims.size())
            detail::throw_error(error::invalid_dimension_size);
        
        std::tuple<std::vector<Ts>...> columns;
        
        // Iterate over the tuple and read the dimension coordinates into each vector.
        detail::apply_index([&] (auto i, auto&& column) {
            using value_type = typename std::decay_t<decltype(column)>::value_type;
            column = coordinates<value_type>(i);
        }, columns);
        
        // Create the cartesian product for the hyperslab.
        return detail::tuple_cartesian_product(columns);
    }
    
    /// Get the coordinates for one dimension by position as a vector.
    template <class T>
    std::vector<T> coordinates(std::size_t pos) const
    {   
        if (pos >= dims.size())
            detail::throw_error(error::invalid_dimension);
        
        // Get the coordinate values.
        int cvarid = dims.at(pos).cvarid_;
        variable cv(ncid_, cvarid);
        cv.start_.at(0) = start_.at(pos);
        cv.shape_.at(0) = shape_.at(pos);
        cv.stride_.at(0) = stride_.at(pos);
        auto coords = cv.values<T>();
        return coords;
    }
    
    /// Get the coordinates for one dimension by name as a vector.
    template <class T>
    std::vector<T> coordinates(const std::string& coordvarname) const
    {
        std::size_t idx = coordinate_position(coordvarname);
        return coordinates<T>(idx);
    }

    // Get the dimension position for a coordinate variable.
    std::size_t coordinate_position(const std::string& coordvarname) const
    {
        int cvarid = api::inq_varid(ncid_, coordvarname).value_or(-1);
        if (cvarid == -1)
            detail::throw_error(error::variable_not_found);

        // Find the dimension associated with this coordinate variable.
        const auto it = std::find_if(dims.begin(), dims.end(), [&](const auto& dim)
            { return dim.cvarid_ == cvarid; });

        if (it == dims.end())
            detail::throw_error(error::variable_not_found);

        return static_cast<std::size_t>(std::distance(dims.begin(), it));
    }

    /// Copy values to allocated memory.
    template <class T>
    void read(T *out) const
    {
        check(api::impl::detail::get_vars(ncid_, varid_, start_.data(), shape_.data(), stride_.data(), out));
    }

    /// Get values as std::vector.
    template <class T, class A = std::allocator<T>>
    std::vector<T, A> values() const
    {
        return api::get_vars<std::vector<T, A>>(ncid_, varid_, start_, shape_, stride_);
    }
    
#ifdef NCPP_USE_BOOST

    template <class T, std::size_t N, class A = std::allocator<T>>
    using multi_array_type = boost::multi_array<T, N, A>;

    template <class T, class A = std::allocator<T>>
    using matrix_type = boost::numeric::ublas::matrix<T,
        boost::numeric::ublas::column_major,
        boost::numeric::ublas::unbounded_array<T, A>>;

    /// Get numeric values as a boost::multi_array.
    template <class T, std::size_t N, class A = std::allocator<T>>
    typename std::enable_if<std::is_arithmetic<T>::value, multi_array_type<T, N, A>>::type multi_array() const
    {
        if (N != shape_.size())
            detail::throw_error(error::invalid_coordinates);

        boost::array<typename boost::multi_array<T, N, A>::size_type, N> extents;
        std::copy_n(shape_.begin(), N, extents.begin());
        boost::multi_array<T, N, A> result(extents, boost::fortran_storage_order{});

        check(api::impl::detail::get_vars(ncid_, varid_, start_.data(), shape_.data(), stride_.data(), result.data()));
        return result;
    }

    /// Get numeric values as a uBLAS matrix.
    template <class T, class A = std::allocator<T>>
    typename std::enable_if<std::is_arithmetic<T>::value, matrix_type<T, A>>::type matrix() const
    {
        auto extents = detail::squeeze(shape_);
        if (extents.size() != 2)
            detail::throw_error(error::invalid_coordinates); // NC_EINVALCOORDS
        
        matrix_type<T, A> result(extents[0], extents[1]);
        check(api::impl::detail::get_vars(ncid_, varid_, start_.data(), shape_.data(), stride_.data(), result.data().begin()));
        return result;
    }

#endif // NCPP_USE_BOOST

private:
    int ncid_;
    int varid_;
    std::vector<std::size_t> start_;
    std::vector<std::size_t> shape_;
    std::vector<std::ptrdiff_t> stride_;
};

} // namespace ncpp

#if defined(NCPP_USE_BOOST) && defined(_MSVC_LANG) && _MSVC_LANG >= 201402L
#pragma warning(pop)
#endif // defined(NCPP_USE_BOOST) && defined(_MSVC_LANG) && _MSVC_LANG >= 201402L

#endif // NCPP_VARIABLE_HPP
