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

#include <netcdf.h>

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

class variable
{
private:
    variable(int ncid, int varid) :
        dims(_ncid, _varid), atts(_ncid, _varid), _ncid(ncid), _varid(varid)
    {
        _start.resize(dims.size(), 0);
        _shape.resize(dims.size(), 0);
        std::transform(dims.begin(), dims.end(), _shape.begin(),
            [](const auto& dim) { return dim.length(); });
    }

    int _ncid;
    int _varid;
    std::vector<std::size_t> _start;
    std::vector<std::size_t> _shape;

public:
    /// Dimensions associated with the variable.
    ncpp::dimensions_type dims;

    /// Attributes associated with the variable.
    ncpp::attributes_type atts;

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
        if (dims.size() == 1 && dims.front().name() == this->name())
            return true;
        else
            return false;
    }

    /// Returns true if the variable is contiguous in memory.
    bool is_contiguous() const
    {
        int contiguous;
        ncpp::check(nc_inq_var_chunking(_ncid, _varid, &contiguous, nullptr));
        return (contiguous != 0);
    }
    
    /// Get the start indexes of the data array.
    std::vector<std::size_t> start() const {
        return _start;
    }

    /// Get the shape of the data array.
    std::vector<std::size_t> shape() const {
        return _shape;
    }

    /// Get the total number of elements in the data array.
    std::size_t size() const
    {
        return std::accumulate(_shape.begin(), _shape.end(), 1ull,
            std::multiplies<std::size_t>());
    }
    
    /// Get numeric values as a vector.
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, std::vector<T>>::type values() const
    {
        std::vector<T> result;
        result.resize(this->size());
        ncpp::check(ncpp::detail::get_vara(_ncid, _varid, _start.data(), _shape.data(), result.data()));
        return result;
    }
    
    /// Get string values as a vector.
    template <typename T>
    typename std::enable_if<std::is_same<T, std::string>::value, std::vector<T>>::type values() const
    {
        std::vector<std::string> result;
        const int nct = this->nctype();

        if (nct == NC_CHAR) {
            // For classic strings, the character position is the last dimension.
            std::size_t vlen = std::accumulate(_shape.begin(), std::prev(_shape.end()), 1ull,
                std::multiplies<std::size_t>());
            std::size_t slen = _shape.back();

            // Read the array into a buffer.
            std::string buffer;
            buffer.resize(vlen * slen);
            ncpp::check(nc_get_vara_text(_ncid, _varid, _start.data(), _shape.data(), &buffer[0]));

            // Iterate over the buffer and extract fixed-width strings.
            result.reserve(vlen);
            for (int i = 0; i < vlen; ++i) {
                result.emplace_back(buffer.substr(i * slen, slen));
            }
        }
        else if (nct == NC_STRING) {
            std::size_t n = this->size();
            std::vector<char *> pv(n, nullptr);

            ncpp::check(nc_get_vara_string(_ncid, _varid, _start.data(), _shape.data(), pv.data()));
            
            result.reserve(n);
            for (const auto& p : pv) {
                if (p) values.emplace_back(std::string(p));
            }
            nc_free_string(n, pv.data());
        }
        else {
            ncpp::detail::throw_error(ncpp::error::invalid_conversion);
        }
        
        return result;
    }
    
#ifdef NCPP_USE_DATE_H
    /// Get time values as a vector of std::chrono::time_point<std::chrono::system_clock, T>.
    /// Assumes a standard Gregorian calendar and CF Conventions for time units attribute.
    template <typename T>
    typename std::enable_if<detail::is_chrono_duration<T>::value,
        std::vector<std::chrono::time_point<std::chrono::system_clock, T>>>::type values() const
    {
        // Read and validate the calendar attribute, if present.
        // We currently assume a proleptic Gregorian calendar.
        if (atts.contains("calendar")) {
            std::string calendar;
            atts["calendar"].read(calendar);
            if (calendar != "gregorian" &&
                calendar != "standard" &&
                calendar != "proleptic_gregorian")
                throw std::invalid_argument("Calendar type not implemented");
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
            throw std::invalid_argument("Invalid time units attribute");
        }
        
        // Check for "since" delimiter.
        ss >> token;
        if (token != "since") {
            throw std::invalid_argument("Invalid time units attribute");
        }
        
        // Reset the buffer and parse the date-time string, checking several possible formats.
        // Assumes CF Convention (ex. "1992-10-8 15:15:42.5 -6:00")
        ss >> std::ws;
        std::getline(ss, token);
        std::chrono::time_point<std::chrono::system_clock, T> start;
        const std::array<std::string, 4> formats = { "%F %T %Ez", "%F %T", "%F %R", "%F" };
        for (const auto& format : formats) {
            ss.clear();
            ss.str(token);
            ss >> date::parse(format, start);
            if (!ss.fail())
                break;
        }
        
        if (ss.fail()) {
            throw std::invalid_argument("Failed to parse time units attribute");
        }
        
        // Create the result vector.
        std::vector<double> offsets = values<double>();
        std::vector<std::chrono::time_point<std::chrono::system_clock, T>> result;
        result.reserve(offsets.size());
        for (const auto& offset : offsets) {
            std::chrono::duration<double> sec(offset * scale);
            auto tp = start + std::chrono::duration_cast<T>(sec);
            result.push_back(tp);
        }
        
        return result;
    }
#endif NCPP_USE_DATE_H

    /// Read numeric values into std::valarray.
    template <typename T>
    void read(std::valarray<T>& values) const
    {
        static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
        
        values.resize(this->size());
        ncpp::check(ncpp::detail::get_vara(_ncid, _varid, _start.data(), _shape.data(), &values[0]));
    }

    /// Read numeric values into std::array.
    template <typename T, std::size_t N>
    void read(std::array<T, N>& values) const
    {
        static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
        static_assert(N > 0, "N must be non-zero");

        if (N != this->size())
            ncpp::detail::throw_error(ncpp::error::invalid_coordinates);

        ncpp::check(ncpp::detail::get_vara(_ncid, _varid, _start.data(), _shape.data(), values.data()));
    }

#ifdef NCPP_USE_BOOST
    /// Read numeric values into boost::multi_array.
    template <typename T, std::size_t N>
    void read(boost::multi_array<T, N>& values) const
    {
        static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
        static_assert(N > 0, "N must be non-zero");

        if (N != _shape.size())
            ncpp::detail::throw_error(ncpp::error::invalid_coordinates);

        // Set the extents.
        boost::array<typename boost::multi_array<T, N>::size_type, N> ext;
        std::copy_n(_shape.begin(), N, ext.begin());
        values.resize(ext);

        ncpp::check(ncpp::detail::get_vara(_ncid, _varid, _start.data(), _shape.data(), values.data()));
    }
#endif NCPP_USE_BOOST
    
    /// Select a subset of the data array by coordinate range.
    template <typename T>
    variable select(std::vector<selection<T>> selections) const
    {
        variable v(*this);
        
        for (auto& s : selections)
        {
            // Get the associated dimension and index.
            const auto it = std::find(dims.begin(), dims.end(), dims[s.dimension_name]);
            if (it == dims.end())
                continue;
            
            const auto index = std::distance(dims.begin(), it);

            // Find indexes from dimension coordinates.
            int cvarid = it->coordvarid();
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
            
            v._shape.at(index) = static_cast<std::size_t>(std::distance(lower, upper));
        }
        
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
        int cvarid = dims.at(index).coordvarid();
        variable cv(_ncid, cvarid);
        auto coords = cv.values<T>();
        
        auto it0 = coords.begin() + _start[index];
        auto it1 = coords.begin() + _start[index] + _shape[index];
        
        return std::vector<T>(it0, it1);
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

    friend class variables_type;
};

} // namespace ncpp

#if defined(NCPP_USE_BOOST) && defined(_MSVC_LANG) && _MSVC_LANG >= 201402L
#pragma warning(pop)
#endif // defined(NCPP_USE_BOOST) && defined(_MSVC_LANG) && _MSVC_LANG >= 201402L

#endif // NCPP_VARIABLE_HPP
