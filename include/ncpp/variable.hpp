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
#include <ncpp/check.hpp>
#include <ncpp/dimensions.hpp>
#include <ncpp/dispatch.hpp>

#ifdef NCPP_USE_BOOST
// Disable global objects boost::extents and boost::indices.
#ifndef BOOST_MULTI_ARRAY_NO_GENERATORS
#define BOOST_MULTI_ARRAY_NO_GENERATORS 1
#endif // !BOOST_MULTI_ARRAY_NO_GENERATORS
#include <boost/multi_array.hpp>
#endif // NCPP_USE_BOOST

#include <algorithm>
#include <functional>
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

class variable {
private:
    variable(int ncid, int varid) :
        dims(_ncid, _varid), atts(_ncid, _varid), _ncid(ncid), _varid(varid) {}

    int _ncid;
    int _varid;

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

    /// Returns true if the variable is contiguous.
    bool is_contiguous() const
    {
        int contiguous;
        ncpp::check(nc_inq_var_chunking(_ncid, _varid, &contiguous, nullptr));
        return (contiguous != 0);
    }

    /// Get the shape of the data array.
    std::vector<std::size_t> shape() const
    {
        std::vector<std::size_t> shape(dims.size());
        std::transform(dims.begin(), dims.end(), shape.begin(),
            [](const auto& dim) { return dim.length(); });
        
        return shape;
    }

    /// Get the total number of elements in the data array.
    std::size_t size() const
    {
        const auto& shape = this->shape();
        return std::accumulate(shape.begin(), shape.end(), 1ull,
            std::multiplies<std::size_t>());
    }

    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, std::vector<T>>::type value() const
    {
        std::vector<T> result;
        result.resize(this->size());
        ncpp::check(ncpp::detail::get_var(_ncid, _varid, result.data()));
        return result;
    }

    /// Read numeric values into std::vector.
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, void>::type read(std::vector<T>& values) const
    {
        values.resize(this->size());
        ncpp::check(ncpp::detail::get_var(_ncid, _varid, values.data()));
    }

    /// Read numeric values into std::valarray.
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, void>::type read(std::valarray<T>& values) const
    {
        values.resize(this->size());
        ncpp::check(ncpp::detail::get_var(_ncid, _varid, &values[0]));
    }

    /// Read numeric values into std::array.
    template <typename T, std::size_t N>
    void read(std::array<T, N>& values) const
    {
        static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
        static_assert(N > 0, "N must be non-zero");

        if (N != this->size())
            ncpp::detail::throw_error(ncpp::error::invalid_coordinates);

        ncpp::check(ncpp::detail::get_var(_ncid, _varid, values.data()));
    }

#ifdef NCPP_USE_BOOST
    /// Read numeric values into boost::multi_array.
    template <typename T, std::size_t N>
    void read(boost::multi_array<T, N>& values) const
    {
        static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
        static_assert(N > 0, "N must be non-zero");

        const auto& shape = this->shape();
        if (N != shape.size())
            ncpp::detail::throw_error(ncpp::error::invalid_coordinates);

        // Set the extents.
        boost::array<typename boost::multi_array<T, N>::size_type, N> ext;
        std::copy_n(shape.begin(), N, ext.begin());
        values.resize(ext);

        ncpp::check(ncpp::detail::get_var(_ncid, _varid, values.data()));
    }
#endif NCPP_USE_BOOST

    /// Read string values into std::vector.
    void read(std::vector<std::string>& values) const
    {
        const int nct = this->nctype();

        if (nct == NC_CHAR) {
            // For classic strings, the character position is the last dimension.
            const auto& shape = this->shape();
            std::size_t vlen = std::accumulate(shape.begin(), std::prev(shape.end()), 1ull,
                std::multiplies<std::size_t>());
            std::size_t slen = shape.back();

            // Read the array into a buffer.
            std::string buffer;
            buffer.resize(vlen * slen);
            ncpp::check(nc_get_var_text(_ncid, _varid, &buffer[0]));

            // Iterate over the buffer and extract fixed-width strings.
            values.clear();
            values.reserve(vlen);
            for (int i = 0; i < vlen; ++i) {
                values.emplace_back(buffer.substr(i * slen, slen));
            }
        }
        else if (nct == NC_STRING) {
            std::size_t n = this->size();
            std::vector<char *> pv(n, nullptr);

            ncpp::check(nc_get_var_string(_ncid, _varid, pv.data()));
            
            values.clear();
            values.reserve(n);
            for (const auto& p : pv) {
                if (p) values.emplace_back(std::string(p));
            }
            nc_free_string(n, pv.data());
        }
        else {
            ncpp::detail::throw_error(ncpp::error::invalid_conversion);
        }
    }

    friend class variables_type;
};

} // namespace ncpp

#if defined(NCPP_USE_BOOST) && defined(_MSVC_LANG) && _MSVC_LANG >= 201402L
#pragma warning(pop)
#endif // defined(NCPP_USE_BOOST) && defined(_MSVC_LANG) && _MSVC_LANG >= 201402L

#endif // NCPP_VARIABLE_HPP
