// Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
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

#include <ncpp/config.hpp>

#include <ncpp/attributes.hpp>
#include <ncpp/dimensions.hpp>
#include <ncpp/dispatch.hpp>
#include <ncpp/selection.hpp>
#include <ncpp/utilities.hpp>
#include <ncpp/check.hpp>

#ifdef NCPP_USE_BOOST
// Disable global objects boost::extents and boost::indices
#ifndef BOOST_MULTI_ARRAY_NO_GENERATORS
#define BOOST_MULTI_ARRAY_NO_GENERATORS 1
#endif // !BOOST_MULTI_ARRAY_NO_GENERATORS
#include <boost/multi_array.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#endif // NCPP_USE_BOOST

#ifdef NCPP_USE_DATE_H
// Implements C++20 extensions for <chrono>
// https://github.com/HowardHinnant/date
#include <date/date.h>
#endif

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

// TODO:
// - Account for scale_factor, add_offset
// - Use optional for _FillValue, missing_value, valid_min, valid_max, valid_range

namespace ncpp {

class variables_type;

/// netCDF variable type.
class variable
{
private:
    variable(int ncid, int varid) :
        dims(_ncid, _varid), atts(_ncid, _varid), _ncid(ncid), _varid(varid)
    {
        _start.resize(dims.size(), 0);
        _shape.resize(dims.size(), 0);
        _stride.resize(dims.size(), 1);
        std::transform(dims.begin(), dims.end(), _shape.begin(),
            [](const auto& dim) { return dim.length(); });
    }

    int _ncid;
    int _varid;
    std::vector<std::size_t> _start;
    std::vector<std::size_t> _shape;
    std::vector<std::ptrdiff_t> _stride;

public:
    /// Dimensions associated with the variable.
    ncpp::dimensions_type dims;

    /// Attributes associated with the variable.
    ncpp::attributes_type atts;

    variable(const ncpp::variable& rhs) = default;
    variable(ncpp::variable&& rhs) = default;
    variable& operator=(const variable& rhs) = default;
    variable& operator=(variable&& rhs) = default;

    enum class storage_type {
        contiguous = NC_CONTIGUOUS,
        chunked = NC_CHUNKED
    };

    enum class filter_type {
        lzo        = 305,
        bzip2      = 307,
        lzf        = 32000,
        blosc      = 32001,
        mafisc     = 32002,
        snappy     = 32003,
        lz4        = 32004,
        apax       = 32005,
        cbf        = 32006,
        jpeg_xr    = 32007,
        bitshuffle = 32008,
        spdp       = 32009,
        lpc_rice   = 32010,
        ccsds_123  = 32011,
        jpeg_ls    = 32012,
        zfp        = 32013,
        fpzip      = 32014,
        zstandard  = 32015,
        b3d        = 32016,
        sz         = 32017,
        fcidecomp  = 32018,
        jpeg       = 32019,
        vbz        = 32020
    };

    bool operator<(const variable& rhs) const {
        return (_varid < rhs._varid);
    }

    bool operator==(const variable& rhs) const {
        return (_ncid == rhs._ncid && _varid == rhs._varid);
    }

    bool operator!=(const variable& rhs) const {
        return !(*this == rhs);
    }

    /// Get the variable name.
    std::string name() const
    {
        char varname[NC_MAX_NAME + 1];
        ncpp::check(nc_inq_varname(_ncid, _varid, varname));
        return std::string(varname);
    }

    /// Get the netCDF type ID for the variable.
    int nctype() const
    {
        int vartype;
        ncpp::check(nc_inq_vartype(_ncid, _varid, &vartype));
        return vartype;
    }

    /// Returns true if the variable is a coordinate variable.
    bool is_coordinate() const
    {
        if (dims.front().name() == this->name()) {
            if (dims.size() == 1)
                return true;
            else if (this->nctype() == NC_CHAR && dims.size() == 2)
                return true;
        }
        return false;
    }

    /// Returns the variable storage mode.
    storage_type storage_mode() const
    {
        int storage;
        ncpp::check(nc_inq_var_chunking(_ncid, _varid, &storage, nullptr));
        return static_cast<storage_type>(storage);
    }

    /// Returns the chunk size for each dimension.
    std::vector<std::size_t> chunk_sizes() const
    {
        int storage;
        std::vector<std::size_t> sizes(dims.size(), 0);
        ncpp::check(nc_inq_var_chunking(_ncid, _varid, &storage, sizes.data()));
        return sizes;
    }

    /// Returns the HDF5 filter type for the variable.
    /// See also: https://portal.hdfgroup.org/display/support/Filters
    filter_type filter() const
    {
        unsigned int filterid;
        ncpp::check(nc_inq_var_filter(_ncid, _varid, &filterid, nullptr, nullptr));
        return static_cast<filter_type>(filterid);
    }

    /// Get the start indexes of the data array.
    std::vector<std::size_t> start() const {
        return _start;
    }

    /// Get the shape of the data array.
    std::vector<std::size_t> shape() const {
        return _shape;
    }
    
    /// Get the strides of the data array.
    std::vector<std::ptrdiff_t> stride() const {
        return _stride;
    }

    /// Get the total number of elements in the data array.
    std::size_t size() const
    {
        return std::accumulate(_shape.begin(), _shape.end(), 1ull,
            std::multiplies<std::size_t>());
    }
    
    /// Get numeric values as a vector.
    template <typename T, typename A = std::allocator<T>>
    typename std::enable_if<std::is_arithmetic<T>::value, std::vector<T, A>>::type values() const
    {
        std::vector<T, A> result;
        result.resize(this->size());
        ncpp::check(ncpp::detail::get_vars(_ncid, _varid, _start.data(), _shape.data(), _stride.data(), result.data()));
        return result;
    }
    
    /// Get string values as a vector.
    template <typename T, typename A = std::allocator<T>>
    typename std::enable_if<std::is_same<T, std::string>::value, std::vector<T, A>>::type values() const
    {
        std::vector<std::string, A> result;
        const int nct = this->nctype();

        if (nct == NC_CHAR) {
            // For classic strings, the character position is the last dimension.
            std::size_t vlen = std::accumulate(_shape.begin(), std::prev(_shape.end()), 1ull,
                std::multiplies<std::size_t>());
            std::size_t slen = _shape.back();

            // Read the array into a buffer.
            std::string buffer;
            buffer.resize(vlen * slen);
            ncpp::check(nc_get_vars_text(_ncid, _varid, _start.data(), _shape.data(), _stride.data(), &buffer[0]));

            // Iterate over the buffer and extract fixed-width strings.
            result.reserve(vlen);
            for (int i = 0; i < vlen; ++i) {
                std::string s = buffer.substr(i * slen, slen);
                s.erase(s.find_last_not_of(' ') + 1);
                result.push_back(s);
            }
        }
        else if (nct == NC_STRING) {
            std::size_t n = this->size();
            std::vector<char *> pv(n, nullptr);

            ncpp::check(nc_get_vars_string(_ncid, _varid, _start.data(), _shape.data(), _stride.data(), pv.data()));
            
            result.reserve(n);
            for (const auto& p : pv) {
                if (p) result.emplace_back(std::string(p));
            }
            nc_free_string(n, pv.data());
        }
        else {
            ncpp::detail::throw_error(ncpp::error::invalid_conversion);
        }
        
        return result;
    }
    
#ifdef NCPP_USE_DATE_H
    /// Get time values as a vector of std::chrono::time_point.
    /// Parser assumes a standard Gregorian calendar and CF Conventions for time units attribute.
    template <typename T, typename A = std::allocator<T>>
    typename std::enable_if<detail::is_chrono_time_point<T>::value, std::vector<T, A>>::type values() const
    {
        // Read and validate the calendar attribute, if present.
        // We currently assume a proleptic Gregorian calendar.
        if (atts.contains("calendar")) {
            std::string calendar;
            atts["calendar"].read(calendar);
            if (calendar != "gregorian" &&
                calendar != "standard" &&
                calendar != "proleptic_gregorian")
                throw std::runtime_error("Calendar type not implemented");
        }
        
        // Read the units attribute.
        std::string units;
        atts["units"].read(units);
        std::stringstream ss(units);
        std::string token;
        
        // Determine the duration scale factor.
        // Supported units: week, day (d), hour (hr, h), minute (min), second (sec, s)
        std::chrono::seconds scale(1);
        ss >> token;
        if (token == "weeks" || token == "week") {
            scale = std::chrono::seconds(604800);
        }
        else if (token == "days" || token == "day" || token == "d") {
            scale = std::chrono::seconds(86400);
        }
        else if (token == "hours" || token == "hour" || token == "h") {
            scale = std::chrono::seconds(3600);
        }
        else if (token == "minutes" || token == "minute" || token == "m") {
            scale = std::chrono::seconds(60);
        }
        else if (token != "seconds" && token != "second" && token != "s") {
            throw std::runtime_error("Invalid time units attribute");
        }
        
        // Check for "since" delimiter.
        ss >> token;
        if (token != "since") {
            throw std::runtime_error("Invalid time units attribute");
        }
        
        // Reset the buffer and parse the date-time string, checking several possible formats.
        // Assumes CF Convention (ex. "1992-10-8 15:15:42.5 -6:00")
        ss >> std::ws;
        std::getline(ss, token);
        T start;
        const std::array<std::string, 4> formats = { "%F %T %Ez", "%F %T", "%F %R", "%F" };
        for (const auto& format : formats) {
            ss.clear();
            ss.str(token);
            ss >> date::parse(format, start);
            if (!ss.fail())
                break;
        }
        
        if (ss.fail()) {
            throw std::runtime_error("Failed to parse time units attribute");
        }
        
        // Create the result vector.
        std::vector<double> offsets = values<double>();
        std::vector<T, A> result;
        result.reserve(offsets.size());
        for (const auto& offset : offsets) {
            std::chrono::duration<double> sec(offset * scale);
            T tp = start + std::chrono::duration_cast<T::duration>(sec);
            result.push_back(tp);
        }
        
        return result;
    }
#endif // NCPP_USE_DATE_H

    /// Read numeric values into std::valarray.
    template <typename T>
    void read(std::valarray<T>& values) const
    {
        static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
        
        values.resize(this->size());
        ncpp::check(ncpp::detail::get_vars(_ncid, _varid, _start.data(), _shape.data(), _stride.data(), &values[0]));
    }

    /// Read numeric values into std::array.
    template <typename T, std::size_t N>
    void read(std::array<T, N>& values) const
    {
        static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
        static_assert(N > 0, "N must be non-zero");

        if (N != this->size())
            ncpp::detail::throw_error(ncpp::error::invalid_coordinates);

        ncpp::check(ncpp::detail::get_vars(_ncid, _varid, _start.data(), _shape.data(), _stride.data(), values.data()));
    }

#ifdef NCPP_USE_BOOST
    /// Read numeric values into boost::multi_array.
    template <typename T, std::size_t N, typename A = std::allocator<T>>
    void read(boost::multi_array<T, N, A>& values) const
    {
        static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
        static_assert(N > 0, "N must be non-zero");

        if (N != _shape.size())
            ncpp::detail::throw_error(ncpp::error::invalid_coordinates);

        // Set the extents.
        boost::array<typename boost::multi_array<T, N, A>::size_type, N> ext;
        std::copy_n(_shape.begin(), N, ext.begin());
        values.resize(ext);

        ncpp::check(ncpp::detail::get_vars(_ncid, _varid, _start.data(), _shape.data(), _stride.data(), values.data()));
    }

	/// Get numeric values as a uBLAS matrix.
    template <typename T, typename A = std::allocator<T>>
    using matrix_type = boost::numeric::ublas::matrix<T,
        boost::numeric::ublas::column_major,
        boost::numeric::ublas::unbounded_array<T, A>>;
    
    template <typename T, typename A = std::allocator<T>>
    typename std::enable_if<std::is_arithmetic<T>::value, matrix_type<T, A>>::type matrix() const
    {
        if (2 != _shape.size())
            ncpp::detail::throw_error(ncpp::error::invalid_coordinates);
        
        matrix_type<T, A> result(_shape[0], _shape[1]);
        ncpp::check(ncpp::detail::get_vars(_ncid, _varid, _start.data(), _shape.data(), _stride.data(), result.data().begin()));
        return result;
    }
#endif // NCPP_USE_BOOST
    
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
    template <typename T>
    variable select(selection<T>& s) const
    {
        variable v(*this);
        
        // Get the associated dimension and index.
        const auto it = std::find(dims.begin(), dims.end(), dims[s.dimension_name]);
        if (it == dims.end())
            ncpp::detail::throw_error(ncpp::error::invalid_dimension);
        
        const auto index = std::distance(dims.begin(), it);

        // Find indexes from dimension coordinates.
        int cvarid = it->_cvarid;
        if (cvarid == -1)
            ncpp::detail::throw_error(ncpp::error::variable_not_found);
        
        variable cv(_ncid, cvarid);
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
            v._start.at(index) = static_cast<std::size_t>(std::distance(upper, coords.end()));
        else
            v._start.at(index) = static_cast<std::size_t>(std::distance(coords.begin(), lower));
        
        v._stride.at(index) = s.stride;
        v._shape.at(index) = static_cast<std::size_t>(std::distance(lower, upper)) / std::abs(s.stride);
        
        return v;
    }
    
    /// Get the coordinates for all dimensions as a vector of tuples.
    template <typename... Ts>
    auto coordinates() const
    {
        // Make sure we have the correct number of columns.
        if (sizeof...(Ts) != dims.size())
            ncpp::detail::throw_error(ncpp::error::invalid_coordinates);
        
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
    
    /// Get the coordinates for one dimension by index as a vector.
    template <typename T>
    std::vector<T> coordinates(int index) const
    {   
        // Get the coordinate values.
        int cvarid = dims.at(index)._cvarid;
        variable cv(_ncid, cvarid);
        cv._start.at(0) = _start.at(index);
        cv._shape.at(0) = _shape.at(index);
        cv._stride.at(0) = _stride.at(index);
        auto coords = cv.values<T>();
        return coords;
    }
    
    /// Get the coordinates for one dimension by name as a vector.
    template <typename T>
    std::vector<T> coordinates(const std::string& dimension_name) const
    {   
        // Get the associated dimension and index.
        const auto it = std::find(dims.begin(), dims.end(), dims[dimension_name]);
        if (it == dims.end())
            return std::vector<T>();
        
        const auto index = std::distance(dims.begin(), it);
        return coordinates<T>(index);
    }

    /// Forward iterator for individual values.
    template <typename T>
    class iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        
        iterator(ncpp::variable& v, std::ptrdiff_t position = 0)
            : _v(v), _position(position), _index(v._start)
        {
            update_cache();
        }

        iterator(const iterator& rhs) = default;
        iterator(iterator&& rhs) = default;
        iterator& operator=(const iterator& rhs) = default;
        iterator& operator=(iterator&& rhs) = default;

        bool operator==(const iterator& rhs) const {
            return (_v == rhs._v && _position == rhs._position);
        }

        bool operator!=(const iterator& rhs) const { 
            return !(*this == rhs);
        }

        /// Returns the dimension indices at the current iterator position.
        std::vector<std::size_t> index() const {
            return _index;
        }
        
        /// Prefix increment operator.
        iterator operator++() {
            ++_position;
            update_cache();
            return *this;
        }

        /// Postfix increment operator.
        iterator operator++(int) {
            iterator it = *this;
            ++_position;
            update_cache();
            return it;
        }

        const T& operator*() const {
            return _value;
        }

        T* operator->() const {
            return &_value;
        }

    private:
        void update_cache()
        {
            // Calculate dimension indices from position.
            std::lldiv_t q { _position , 0LL };
            for (std::ptrdiff_t i = _index.size() - 1;  i >= 0; --i) {
                q = std::div(q.quot, static_cast<std::ptrdiff_t>(_v._shape[i]));
                _index[i] = _v._start[i] + q.rem * _v._stride[i];
            }

            ncpp::check(ncpp::detail::get_var1(_v._ncid, _v._varid, _index.data(), &_value));
        }

        ncpp::variable& _v;
        std::ptrdiff_t _position;
        std::vector<std::size_t> _index;
        T _value;
    };

    template <typename T>
    iterator<T> begin() {
        return iterator<T>(*this);
    }

    template <typename T>
    iterator<T> end() {
        return iterator<T>(*this, this->size());
    }

    friend class variables_type;
};

} // namespace ncpp

#if defined(NCPP_USE_BOOST) && defined(_MSVC_LANG) && _MSVC_LANG >= 201402L
#pragma warning(pop)
#endif // defined(NCPP_USE_BOOST) && defined(_MSVC_LANG) && _MSVC_LANG >= 201402L

#endif // NCPP_VARIABLE_HPP
