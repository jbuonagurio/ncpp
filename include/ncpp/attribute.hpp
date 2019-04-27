// Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_ATTRIBUTE_HPP
#define NCPP_ATTRIBUTE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <netcdf.h>

#include <ncpp/check.hpp>
#include <ncpp/dispatch.hpp>
#include <ncpp/types.hpp>
#ifdef NCPP_USE_VARIANT
#include <ncpp/variant.hpp>
#endif // NCPP_USE_VARIANT

#include <array>
#include <ostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace ncpp {

class attributes_type;

class attribute
{
private:
    attribute(int ncid, int varid, const std::string& attname)
        : _ncid(ncid), _varid(varid), _attname(attname) {}
    
    int _ncid;
    int _varid;
    std::string _attname;

public:
    bool operator<(const attribute& rhs) const {
        return std::tie(_varid, _attname) <
               std::tie(rhs._varid, rhs._attname);
    }
    
    bool operator==(const attribute& rhs) const {
        return std::tie(_ncid, _varid, _attname) ==
               std::tie(rhs._ncid, rhs._varid, rhs._attname);
    }

    bool operator!=(const attribute& rhs) const {
        return !(*this == rhs);
    }

    /// Get the attribute name.
    std::string name() const
    {
        return _attname;
    }

    /// Get the attribute length.
    std::size_t length() const
    {
        std::size_t attlen;
        ncpp::check(nc_inq_attlen(_ncid, _varid, _attname.data(), &attlen));
        return attlen;
    }

    /// Get the netCDF type ID for the attribute.
    int nctype() const
    {
        int atttype;
        ncpp::check(nc_inq_atttype(_ncid, _varid, _attname.data(), &atttype));
        return atttype;
    }



    /// Read fixed-length string attribute (NC_CHAR).
    std::string value() const
    {
        std::string result;
        std::size_t n = this->length();
        result.resize(n);
        ncpp::check(ncpp::detail::get_att(_ncid, _varid, _attname.c_str(), &result[0]));
        return result;
    }









    /// Read scalar attribute with numeric type.
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, void>::type read(T& value) const
    {
        if (this->length() == 1)
            ncpp::check(ncpp::detail::get_att(_ncid, _varid, _attname.c_str(), value));
        else
            ncpp::detail::throw_error(ncpp::error::result_out_of_range);
    }

    /// Read fixed-length attribute array with numeric type.
    template <typename T, std::size_t N>
    void read(std::array<T, N>& values) const
    {
        static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
        static_assert(N > 0, "N must be non-zero");

        if (N != this->length())
            ncpp::detail::throw_error(ncpp::error::invalid_coordinates);

        ncpp::check(ncpp::detail::get_att(_ncid, _varid, _attname.c_str(), values.data()));
    }

    /// Read variable-length attribute array with numeric type.
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, void>::type read(std::vector<T>& values) const
    {
        std::size_t n = this->length();
        values.resize(n);
        ncpp::check(ncpp::detail::get_att(_ncid, _varid, _attname.c_str(), values.data()));
    }

    /// Read fixed-length string attribute (NC_CHAR).
    void read(std::string& value) const
    {
        std::size_t n = this->length();
        value.resize(n);
        ncpp::check(ncpp::detail::get_att(_ncid, _varid, _attname.c_str(), &value[0]));
    }

    /// Read variable-length string attribute (NC_STRING).
    void read(std::vector<std::string>& values) const
    {
        std::size_t n = this->length();
        std::vector<char *> pv(n, nullptr);
        ncpp::check(nc_get_att_string(_ncid, _varid, _attname.c_str(), pv.data()));
        values.clear();
        values.reserve(n);
        for (const auto& p : pv) {
            if (p) values.emplace_back(std::string(p));
        }
        nc_free_string(n, pv.data());
    }

#ifdef NCPP_USE_VARIANT
    /// Read attribute array with variant type.
    void read(ncpp::variant& values) const
    {
        int i = this->nctype();
        values = ncpp::nctype_to_variant.at(i);
        std::visit([&](auto&& arg) {
            this->read(arg);
        }, values);
    }
#endif // NCPP_USE_VARIANT

    friend class attributes_type;
};

} // namespace ncpp

#endif // NCPP_ATTRIBUTE_HPP