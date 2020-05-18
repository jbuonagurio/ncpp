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
private:
    variable(int ncid, int varid) :
        dims(ncid_, varid_), atts(ncid_, varid_), ncid_(ncid), varid_(varid)
    {
        start_.resize(dims.size(), 0);
        shape_.resize(dims.size(), 0);
        stride_.resize(dims.size(), 1);
        std::transform(dims.begin(), dims.end(), shape_.begin(),
            [](const auto& dim) { return dim.length(); });
    }

    int ncid_;
    int varid_;
    std::vector<std::size_t> start_;
    std::vector<std::size_t> shape_;
    std::vector<std::ptrdiff_t> stride_;

public:
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
    std::string name() const
    {
        return inq_varname(ncid_, varid_);
    }

    // Get the variable ID.
    int varid() const
    {
        return varid_;
    }

    /// Get the netCDF data type ID for the variable.
    int netcdf_type() const
    {
        return inq_vartype(ncid_, varid_);
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
    std::size_t size() const
    {
        return std::accumulate(shape_.begin(), shape_.end(), 1ull,
            std::multiplies<std::size_t>());
    }

    /// Returns true if the variable is a coordinate variable.
    bool is_coordinate() const
    {
        if (dims.empty())
            return false;
        
        if (dims.front().name() == this->name()) {
            if (dims.size() == 1)
                return true;
            else if (this->netcdf_type() == NC_CHAR && dims.size() == 2)
                return true;
        }
        return false;
    }

    template <class T>
    std::optional<T> fill_value() const
    {
        return inq_var_fill(ncid_, varid_);
    }

    /// Returns the variable storage type.
    var_storage_type storage_type() const
    {
        return inq_var_storage(ncid_, varid_).value();
    }

    /// Returns the chunk size for each dimension.
    std::vector<std::size_t> chunk_sizes() const
    {
        return inq_var_chunksizes(ncid_, varid_);
    }

#ifdef NC_HAS_HDF5

    /// Returns the HDF5 filter ID for the variable.
    /// See also: https://portal.hdfgroup.org/display/support/Filters
    unsigned int filter_type() const
    {
        return inq_var_filter_id(ncid_, varid_).value();
    }

    /// Returns the HDF5 filter name for the variable.
    /// See also: https://portal.hdfgroup.org/display/support/Filters
    std::string filter_name() const
    {
        return inq_var_filter_name(ncid_, varid_);
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
        variable v(*this);
        
        // Get the associated dimension and index.
        const auto it = std::find(dims.begin(), dims.end(), dims[s.dimension_name]);
        if (it == dims.end())
            detail::throw_error(error::invalid_dimension);
        
        const auto idx = std::distance(dims.begin(), it);

        // Find indexes from dimension coordinates.
        int cvarid = it->cvarid_;
        if (cvarid == -1)
            detail::throw_error(error::variable_not_found);
        
        variable cv(ncid_, cvarid);
        auto coords = cv.values<T>();
        
        // Handle decreasing coordinates.
        bool reversed = false;
        if (!std::is_sorted(coords.begin(), coords.end())) {
            std::reverse(coords.begin(), coords.end());
            reversed = true;
        }
        
        if (s.min_coordinate > s.max_coordinate) {
            std::swap(s.min_coordinate, s.max_coordinate);
        }
        
        // Set the start and shape indexes.
        auto lower = std::lower_bound(coords.begin(), coords.end(), s.min_coordinate);
        auto upper = std::upper_bound(coords.begin(), coords.end(), s.max_coordinate);
        
        if (reversed)
            v.start_.at(idx) = static_cast<std::size_t>(std::distance(upper, coords.end()));
        else
            v.start_.at(idx) = static_cast<std::size_t>(std::distance(coords.begin(), lower));
        
        v.stride_.at(idx) = s.stride;
        v.shape_.at(idx) = static_cast<std::size_t>(std::distance(lower, upper)) / std::abs(s.stride);
        
        return v;
    }
    
    /// Get the coordinates for all dimensions as a vector of tuples.
    template <typename... Ts>
    auto coordinates() const
    {
        // Make sure we have the correct number of columns.
        if (sizeof...(Ts) != dims.size())
            detail::throw_error(error::invalid_coordinates);
        
        std::tuple<std::vector<Ts>...> columns;
        
        // Iterate over the tuple and read the dimension coordinates into each vector.
        detail::apply_index([&] (auto i, auto&& column) {
            using value_type = typename std::decay_t<decltype(column)>::value_type;
            column = coordinates<value_type>(i);
        }, columns);
        
        // Create the cartesian product for the hyperslab.
        std::vector<std::tuple<Ts...>> result;
        result = detail::tuple_cartesian_product(columns);
        
        return result;
    }
    
    /// Get the coordinates for one dimension by position as a vector.
    template <class T>
    std::vector<T> coordinates(std::size_t dimension_pos) const
    {   
        // Get the coordinate values.
        int cvarid = dims.at(dimension_pos).cvarid_;
        variable cv(ncid_, cvarid);
        cv.start_.at(0) = start_.at(dimension_pos);
        cv.shape_.at(0) = shape_.at(dimension_pos);
        cv.stride_.at(0) = stride_.at(dimension_pos);
        auto coords = cv.values<T>();
        return coords;
    }
    
    /// Get the coordinates for one dimension by name as a vector.
    template <class T>
    std::vector<T> coordinates(const std::string& dimension_name) const
    {   
        // Get the associated dimension and index.
        const auto it = std::find(dims.begin(), dims.end(), dims[dimension_name]);
        if (it == dims.end())
            detail::throw_error(error::invalid_dimension);
        
        const auto pos = std::distance(dims.begin(), it);
        return coordinates<T>(pos);
    }
    
    /// Copy values to allocated memory.
    template <class T>
    void read(T *out) const
    {
        check(impl::detail::get_vars(ncid_, varid_, start_.data(), shape_.data(), stride_.data(), out));
    }

    /// Get numeric values as std::vector.
    template <class T, class A = std::allocator<T>>
    typename std::enable_if_t<std::is_arithmetic_v<T>, std::vector<T, A>>
    values() const
    {
        return get_vars<std::vector<T, A>>(ncid_, varid_, start_, shape_, stride_);
    }
    
    /// Get string values as std::vector.
    template <class T, class A = std::allocator<T>>
    typename std::enable_if_t<std::is_same_v<T, std::string>, std::vector<T, A>>
    values() const
    {
        return get_vars<std::vector<T, A>>(ncid_, varid_, start_, shape_, stride_);
    }
    
#ifdef NCPP_USE_DATE_H

    /// Get time values as a vector of std::chrono::time_point, typically
    /// date::sys_days or date::sys_seconds. Assumes Gregorian calendar
    /// and CF Conventions for time units. Throws attribute_not_found
    /// error if the calendar or units attribute cannot be parsed.
    template <class T, class A = std::allocator<T>>
    typename std::enable_if_t<detail::is_chrono_time_point<T>::value, std::vector<T, A>>
    values() const
    {
        return get_vars<std::vector<T, A>>(ncid_, varid_, start_, shape_, stride_);
    }

#endif // NCPP_USE_DATE_H

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

        check(iimpl::detail::get_vars(ncid_, varid_, start_.data(), shape_.data(), stride_.data(), result.data()));
        return result;
    }

    /// Get numeric values as a uBLAS matrix.
    template <class T, class A = std::allocator<T>>
    typename std::enable_if<std::is_arithmetic<T>::value, matrix_type<T, A>>::type matrix() const
    {
        auto rank = std::count_if(shape_.begin(), shape_.end(), [](auto x) { return x > 1; });
        if (rank > 2)
            detail::throw_error(error::invalid_coordinates);
        
        std::array<std::size_t, 2> extents;
        std::copy_if(shape_.begin(), shape_.end(), extents.begin(), [](auto x) { return x > 1; });
        matrix_type<T, A> result(extents[0], extents[1]);
        
        check(iimpl::detail::get_vars(ncid_, varid_, start_.data(), shape_.data(), stride_.data(), result.data().begin()));
        return result;
    }

#endif // NCPP_USE_BOOST

    /// Bidirectional iterator for individual values.
    template <class T>
    class value_iterator
    {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        
        value_iterator(variable& v, std::ptrdiff_t position = 0)
            : var_(v), position_(position) {}

        value_iterator(const value_iterator& rhs) = default;
        value_iterator(value_iterator&& rhs) = default;
        value_iterator& operator=(const value_iterator& rhs) = default;
        value_iterator& operator=(value_iterator&& rhs) = default;

        bool operator==(const value_iterator& rhs) const {
            return (var_ == rhs.var_ && position_ == rhs.position_);
        }

        bool operator!=(const value_iterator& rhs) const { 
            return !(*this == rhs);
        }

        /// Calculate the dimension indices from the current iterator position.
        index_type index() const {
            index_type idx = var_.start_;
            std::lldiv_t q { position_ , 0LL };
            for (std::ptrdiff_t i = idx.size() - 1;  i >= 0; --i) {
                q = std::div(q.quot, static_cast<std::ptrdiff_t>(var_.shape_[i]));
                idx[i] = var_.start_[i] + q.rem * var_.stride_[i];
            }
            return idx;
        }
        
        /// Prefix increment operator.
        value_iterator operator++() {
            ++position_;
            return *this;
        }

        /// Postfix increment operator.
        value_iterator operator++(int) {
            iterator it = *this;
            ++position_;
            return it;
        }

        /// Prefix decrement operator.
        value_iterator operator--() {
            --position_;
            return *this;
        }

        /// Postfix decrement operator.
        value_iterator operator--(int) {
            iterator it = *this;
            ++position_;
            return it;
        }

        T operator*() const {
            auto start = index();
            return get_var1<T>(var_.ncid_, var_.varid_, start);
        }

    private:
        variable& var_;
        std::ptrdiff_t position_;
    };

    template <class T>
    using iterator = value_iterator<T>;

    template <class T>
    using reverse_iterator = std::reverse_iterator<iterator<T>>;

    template <class T>
    iterator<T> begin() {
        return iterator<T>(*this);
    }

    template <class T>
    iterator<T> end() {
        return iterator<T>(*this, this->size());
    }
    
    template <class T>
    reverse_iterator<T> rbegin() {
        return reverse_iterator<T>(end());
    }
    
    template <class T>
    reverse_iterator<T> rend() {
        return reverse_iterator<T>(begin());
    }

    friend class variables_type;
};

} // namespace ncpp

#if defined(NCPP_USE_BOOST) && defined(_MSVC_LANG) && _MSVC_LANG >= 201402L
#pragma warning(pop)
#endif // defined(NCPP_USE_BOOST) && defined(_MSVC_LANG) && _MSVC_LANG >= 201402L

#endif // NCPP_VARIABLE_HPP
