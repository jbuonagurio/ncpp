// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
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

#include <ncpp/config.hpp>

#include <ncpp/functions/variable.hpp>
#include <ncpp/functions/attribute.hpp>
#include <ncpp/check.hpp>
#include <ncpp/types.hpp>
#include <ncpp/variant.hpp>

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
    friend class attributes_type;

public:
    attribute(int ncid, int varid, const std::string& attname)
        : ncid_(ncid), varid_(varid), attname_(attname) {}

    bool operator<(const attribute& rhs) const {
        return std::tie(varid_, attname_) <
               std::tie(rhs.varid_, rhs.attname_);
    }
    
    bool operator==(const attribute& rhs) const {
        return std::tie(ncid_, varid_, attname_) ==
               std::tie(rhs.ncid_, rhs.varid_, rhs.attname_);
    }

    bool operator!=(const attribute& rhs) const {
        return !(*this == rhs);
    }

    /// Get the attribute name.
    std::string name() const
    {
        return attname_;
    }

    /// Get the attribute length.
    std::size_t length() const
    {
        return inq_attlen(ncid_, varid_, attname_);
    }

    /// Get the netCDF type ID for the attribute.
    int netcdf_type() const
    {
        return inq_atttype(ncid_, varid_, attname_);
    }
    
    /// Get scalar attribute with arithmetic type.
    template <class T>
    typename std::enable_if<std::is_arithmetic<T>::value, T>::type value() const
    {
        return get_att<T>(ncid_, varid_, attname_);
    }

    /// Get scalar attribute with fixed-length string type (`NC_CHAR`).
    template <class T>
    typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type value() const
    {
        return get_att<T>(ncid_, varid_, attname_);
    }

    /// Get attribute array with arithmetic type.
    template <class T, class A = std::allocator<T>>
    typename std::enable_if<std::is_arithmetic<T>::value, std::vector<T, A>>::type values() const
    {
        return get_att_array<std::vector<T, A>>(ncid_, varid_, attname_);
    }

    /// Get attribute array with variable-length string type (`NC_STRING`).
    template <class T, class A = std::allocator<T>>
    typename std::enable_if<std::is_same<T, std::string>::value, std::vector<std::string, A>>::type values() const
    {
        return get_att_array<std::vector<T, A>>(ncid_, varid_, attname_);
    }

    /// Get attribute array with variant type.
    variant value() const
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
            detail::throw_error(error::invalid_data_type);
            return {};
        }
    }

private:    
    int ncid_;
    int varid_;
    std::string attname_;
};

} // namespace ncpp

#endif // NCPP_ATTRIBUTE_HPP