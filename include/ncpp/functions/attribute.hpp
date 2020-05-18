// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_FUNCTIONS_ATTRIBUTE_HPP
#define NCPP_FUNCTIONS_ATTRIBUTE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <netcdf.h>

#include <ncpp/config.hpp>
#include <ncpp/check.hpp>

#include <cstddef>
#include <optional>
#include <string>
#include <system_error>
#include <type_traits>
#include <vector>

namespace ncpp {
namespace impl {

// Get the ID of an attribute, or std::nullopt if undefined.
inline std::optional<int> inq_attid(int ncid, int varid, const std::string& attname, std::error_code *ec = nullptr)
{
    int attid;
    check(nc_inq_attid(ncid, varid, attname.c_str(), &attid), ec);
    if (ec && ec->value())
        return {};
    return attid;
}

// Get the name of an attribute, or empty string if undefined.
inline std::string inq_attname(int ncid, int varid, int attnum, std::error_code *ec = nullptr)
{
    char attname[NC_MAX_NAME + 1];
    check(nc_inq_attname(ncid, varid, attnum, attname), ec);
    return (ec && ec->value()) ? std::string() : attname;
}

// Get the type of an attribute, or NC_NAT if undefined.
inline int inq_atttype(int ncid, int varid, const std::string& attname, std::error_code *ec = nullptr)
{
    int atttype;
    check(nc_inq_atttype(ncid, varid, attname.c_str(), &atttype), ec);
    if (ec && ec->value())
        return NC_NAT;
    return atttype;
}

// Get the length of an attribute.
inline std::size_t inq_attlen(int ncid, int varid, const std::string& attname, std::error_code *ec = nullptr)
{
    std::size_t attlen = 0;
    check(nc_inq_attlen(ncid, varid, attname.c_str(), &attlen), ec);
    if (ec && ec->value())
        return 0;
    return attlen;
}


namespace detail {

    inline int put_att(int ncid, int varid, const char *name, std::size_t len, const char *op)
        { return nc_put_att_text(ncid, varid, name, len, op); }
    inline int get_att(int ncid, int varid, const char *name, char *ip)
        { return nc_get_att_text(ncid, varid, name, ip); }
    inline int put_att(int ncid, int varid, const char *name, std::size_t len, const char **op)
        { return nc_put_att_string(ncid, varid, name, len, op); }
    inline int get_att(int ncid, int varid, const char *name, char **ip)
        { return nc_get_att_string(ncid, varid, name, ip); }
    inline int put_att(int ncid, int varid, const char *name, std::size_t len, const unsigned char *op)
        { return nc_put_att_uchar(ncid, varid, name, NC_UBYTE, len, op); }
    inline int get_att(int ncid, int varid, const char *name, unsigned char *ip)
        { return nc_get_att_uchar(ncid, varid, name, ip); }
    inline int put_att(int ncid, int varid, const char *name, std::size_t len, const signed char *op)
        { return nc_put_att_schar(ncid, varid, name, NC_BYTE, len, op); }
    inline int get_att(int ncid, int varid, const char *name, signed char *ip)
        { return nc_get_att_schar(ncid, varid, name, ip); }
    inline int put_att(int ncid, int varid, const char *name, std::size_t len, const short *op)
        { return nc_put_att_short(ncid, varid, name, NC_SHORT, len, op); }
    inline int get_att(int ncid, int varid, const char *name, short *ip)
        { return nc_get_att_short(ncid, varid, name, ip); }
    inline int put_att(int ncid, int varid, const char *name, std::size_t len, const int *op)
        { return nc_put_att_int(ncid, varid, name, NC_INT, len, op); }
    inline int get_att(int ncid, int varid, const char *name, int *ip)
        { return nc_get_att_int(ncid, varid, name, ip); }
    inline int put_att(int ncid, int varid, const char *name, std::size_t len, const long *op)
        { return nc_put_att_long(ncid, varid, name, NC_LONG, len, op); }
    inline int get_att(int ncid, int varid, const char *name, long *ip)
        { return nc_get_att_long(ncid, varid, name, ip); }
    inline int put_att(int ncid, int varid, const char *name, std::size_t len, const float *op)
        { return nc_put_att_float(ncid, varid, name, NC_FLOAT, len, op); }
    inline int get_att(int ncid, int varid, const char *name, float *ip)
        { return nc_get_att_float(ncid, varid, name, ip); }
    inline int put_att(int ncid, int varid, const char *name, std::size_t len, const double *op)
        { return nc_put_att_double(ncid, varid, name, NC_DOUBLE, len, op); }
    inline int get_att(int ncid, int varid, const char *name, double *ip)
        { return nc_get_att_double(ncid, varid, name, ip); }
    inline int put_att(int ncid, int varid, const char *name, std::size_t len, const unsigned short *op)
        { return nc_put_att_ushort(ncid, varid, name, NC_USHORT, len, op); }
    inline int get_att(int ncid, int varid, const char *name, unsigned short *ip)
        { return nc_get_att_ushort(ncid, varid, name, ip); }
    inline int put_att(int ncid, int varid, const char *name, std::size_t len, const unsigned int *op)
        { return nc_put_att_uint(ncid, varid, name, NC_UINT, len, op); }
    inline int get_att(int ncid, int varid, const char *name, unsigned int *ip)
        { return nc_get_att_uint(ncid, varid, name, ip); }
    inline int put_att(int ncid, int varid, const char *name, std::size_t len, const long long *op)
        { return nc_put_att_longlong(ncid, varid, name, NC_INT64, len, op); }
    inline int get_att(int ncid, int varid, const char *name, long long *ip)
        { return nc_get_att_longlong(ncid, varid, name, ip); }
    inline int put_att(int ncid, int varid, const char *name, std::size_t len, const unsigned long long *op)
        { return nc_put_att_ulonglong(ncid, varid, name, NC_UINT64, len, op); }
    inline int get_att(int ncid, int varid, const char *name, unsigned long long *ip)
        { return nc_get_att_ulonglong(ncid, varid, name, ip); }

} // namespace detail


// Get a scalar attribute with arithmetic type.
template <class T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
get_att(int ncid, int varid, const std::string& attname, std::error_code *ec = nullptr)
{
    T result = {};
    std::size_t attlen = inq_attlen(ncid, varid, attname, ec);
    if (attlen != 1) {
        check(NC_ERANGE, ec); // Numeric conversion not representable
        return result;
    }

    check(detail::get_att(ncid, varid, attname.c_str(), &result), ec);
    return result;
}

// Get a scalar attribute with fixed-length string type (`NC_CHAR`).
template <class T>
typename std::enable_if_t<std::is_same_v<T, std::string>, std::string>
get_att(int ncid, int varid, const std::string& attname, std::error_code *ec = nullptr)
{
    std::string result;
    std::size_t attlen = inq_attlen(ncid, varid, attname, ec);
    if (attlen == 0)
        return result;
    
    result.resize(attlen);
    check(nc_get_att_text(ncid, varid, attname.c_str(), &result[0]), ec);
    return result;
}

// Get an attribute array with arithmetic type.
template <class Container>
typename std::enable_if_t<std::is_arithmetic_v<typename Container::value_type>, Container>
get_att_array(int ncid, int varid, const std::string& attname, std::error_code *ec = nullptr)
{
    Container result;
    std::size_t attlen = inq_attlen(ncid, varid, attname, ec);
    if (attlen == 0)
        return result;
    
    result.resize(attlen);
    check(detail::get_att(ncid, varid, attname.c_str(), result.data()), ec);
    return result;
}

// Get an attribute array with variable-length string type (`NC_STRING`).
template <class Vector>
typename std::enable_if_t<std::is_same_v<typename Vector::value_type, std::string>, Vector>
get_att_array(int ncid, int varid, const std::string& attname, std::error_code *ec = nullptr)
{
    Vector result;
    std::size_t attlen = inq_attlen(ncid, varid, attname, ec);
    if (attlen == 0)
        return result;
    
    std::vector<char *> pv(attlen, nullptr);
    check(nc_get_att_string(ncid, varid, attname.c_str(), pv.data()), ec);
    if (ec && ec->value())
        return result;

    result.reserve(attlen);
    for (const auto p : pv) {
        if (p) result.emplace_back(std::string(p));
    }
    
    nc_free_string(attlen, pv.data());
    return result;
}


} // namespace impl


inline std::optional<int> inq_attid(int ncid, int varid, const std::string& attname, std::error_code& ec) noexcept
    { return impl::inq_attid(ncid, varid, attname, &ec); }
inline std::optional<int> inq_attid(int ncid, int varid, const std::string& attname)
    { return impl::inq_attid(ncid, varid, attname); }


inline std::string inq_attname(int ncid, int varid, int attnum, std::error_code& ec)
    { return impl::inq_attname(ncid, varid, attnum, &ec); }
inline std::string inq_attname(int ncid, int varid, int attnum)
    { return impl::inq_attname(ncid, varid, attnum); }


inline int inq_atttype(int ncid, int varid, const std::string& attname, std::error_code& ec) noexcept
    { return impl::inq_atttype(ncid, varid, attname, &ec); }
inline int inq_atttype(int ncid, int varid, const std::string& attname)
    { return impl::inq_atttype(ncid, varid, attname); }


inline std::size_t inq_attlen(int ncid, int varid, const std::string& attname, std::error_code& ec) noexcept
    { return impl::inq_attlen(ncid, varid, attname, &ec); }
inline std::size_t inq_attlen(int ncid, int varid, const std::string& attname)
    { return impl::inq_attlen(ncid, varid, attname); }


template <class T>
T get_att(int ncid, int varid, const std::string& attname, std::error_code &ec) noexcept
    { return impl::get_att<T>(ncid, varid, attname, &ec); }
template <class T>
T get_att(int ncid, int varid, const std::string& attname)
    { return impl::get_att<T>(ncid, varid, attname); }


template <class Container>
Container get_att_array(int ncid, int varid, const std::string& attname, std::error_code &ec)
    { return impl::get_att_array<Container>(ncid, varid, attname, &ec); }
template <class Container>
Container get_att_array(int ncid, int varid, const std::string& attname)
    { return impl::get_att_array<Container>(ncid, varid, attname); }


} // namespace ncpp

#endif // NCPP_FUNCTIONS_ATTRIBUTE_HPP


