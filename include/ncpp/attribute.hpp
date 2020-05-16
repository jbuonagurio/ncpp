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

#include <ncpp/config.hpp>

#include <ncpp/check.hpp>
#include <ncpp/dispatch.hpp>
#include <ncpp/types.hpp>
#include <ncpp/variant.hpp>

#include <netcdf.h>
#include <array>
#include <memory>
#include <ostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace ncpp {

class attributes_type;

/// netCDF attribute type.
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
    int netcdf_type() const
    {
        int atttype;
        ncpp::check(nc_inq_atttype(_ncid, _varid, _attname.data(), &atttype));
        return atttype;
    }
    
    /// Get scalar attribute with numeric type.
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, T>::type value() const
    {
        if (this->length() != 1)
            ncpp::detail::throw_error(ncpp::error::result_out_of_range);
        
        T result;
        ncpp::check(ncpp::detail::get_att(_ncid, _varid, _attname.c_str(), &result));
        return result;
    }

    /// Get attribute array with numeric type.
    template <typename T, typename A = std::allocator<T>>
    typename std::enable_if<std::is_arithmetic<T>::value, std::vector<T, A>>::type values() const
    {
        std::vector<T, A> result;
        result.resize(this->length());
        ncpp::check(ncpp::detail::get_att(_ncid, _varid, _attname.c_str(), result.data()));
        return result;
    }

    /// Get fixed-length string attribute (`NC_CHAR`).
    template <typename T>
    typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type value() const
    {
        std::string result;
        result.resize(this->length());
        ncpp::check(ncpp::detail::get_att(_ncid, _varid, _attname.c_str(), &result[0]));
        return result;
    }

    /// Get variable-length string attribute (`NC_STRING`).
    template <typename T, typename A = std::allocator<T>>
    typename std::enable_if<std::is_same<T, std::string>::value, std::vector<std::string, A>>::type values() const
    {
        std::vector<std::string> result;
        std::size_t n = this->length();
        std::vector<char *> pv(n, nullptr);
        ncpp::check(nc_get_att_string(_ncid, _varid, _attname.c_str(), pv.data()));
        result.reserve(n);
        for (const auto& p : pv) {
            if (p) result.emplace_back(std::string(p));
        }
        nc_free_string(n, pv.data());
        return result;
    }

    /// Get attribute array with variant type.
    ncpp::variant value() const
    {
        switch (this->netcdf_type()) {
        case NC_BYTE:   return this->values<signed char>();
        case NC_CHAR:   return this->value<std::string>();
        case NC_SHORT:  return this->values<short>();
        case NC_INT:    return this->values<int>();
        case NC_FLOAT:  return this->values<float>();
        case NC_DOUBLE: return this->values<double>();
        case NC_UBYTE:  return this->values<unsigned char>();
        case NC_USHORT: return this->values<unsigned short>();
        case NC_UINT:   return this->values<unsigned int>();
        case NC_INT64:  return this->values<long long>();
        case NC_UINT64: return this->values<unsigned long long>();
        case NC_STRING: return this->values<std::string>();
        default:
            ncpp::detail::throw_error(ncpp::error::invalid_data_type);
            return {};
        }
    }

    friend class attributes_type;
};

} // namespace ncpp

#endif // NCPP_ATTRIBUTE_HPP