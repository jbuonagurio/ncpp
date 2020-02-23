// Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_DISPATCH_HPP
#define NCPP_DISPATCH_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ncpp/config.hpp>

#include <netcdf.h>

#include <cstddef>

// This file provides overloads for typed netCDF-C libdispatch functions.

namespace ncpp {
namespace detail {

#pragma region {put,get}_var1

inline int put_var1(int ncid, int varid, const std::size_t *indexp, const char *op) {
    return nc_put_var1_text(ncid, varid, indexp, op);
}
inline int get_var1(int ncid, int varid, const std::size_t *indexp, char *ip) {
    return nc_get_var1_text(ncid, varid, indexp, ip);
}
inline int put_var1(int ncid, int varid, const std::size_t *indexp, const unsigned char *op) {
    return nc_put_var1_uchar(ncid, varid, indexp, op);
}
inline int get_var1(int ncid, int varid, const std::size_t *indexp, unsigned char *ip) {
    return nc_get_var1_uchar(ncid, varid, indexp, ip);
}
inline int put_var1(int ncid, int varid, const std::size_t *indexp, const signed char *op) {
    return nc_put_var1_schar(ncid, varid, indexp, op);
}
inline int get_var1(int ncid, int varid, const std::size_t *indexp, signed char *ip) {
    return nc_get_var1_schar(ncid, varid, indexp, ip);
}
inline int put_var1(int ncid, int varid, const std::size_t *indexp, const short *op) {
    return nc_put_var1_short(ncid, varid, indexp, op);
}
inline int get_var1(int ncid, int varid, const std::size_t *indexp, short *ip) {
    return nc_get_var1_short(ncid, varid, indexp, ip);
}
inline int put_var1(int ncid, int varid, const std::size_t *indexp, const int *op) {
    return nc_put_var1_int(ncid, varid, indexp, op);
}
inline int get_var1(int ncid, int varid, const std::size_t *indexp, int *ip) {
    return nc_get_var1_int(ncid, varid, indexp, ip);
}
inline int put_var1(int ncid, int varid, const std::size_t *indexp, const long *op) {
    return nc_put_var1_long(ncid, varid, indexp, op);
}
inline int get_var1(int ncid, int varid, const std::size_t *indexp, long *ip) {
    return nc_get_var1_long(ncid, varid, indexp, ip);
}
inline int put_var1(int ncid, int varid, const std::size_t *indexp, const float *op) {
    return nc_put_var1_float(ncid, varid, indexp, op);
}
inline int get_var1(int ncid, int varid, const std::size_t *indexp, float *ip) {
    return nc_get_var1_float(ncid, varid, indexp, ip);
}
inline int put_var1(int ncid, int varid, const std::size_t *indexp, const double *op) {
    return nc_put_var1_double(ncid, varid, indexp, op);
}
inline int get_var1(int ncid, int varid, const std::size_t *indexp, double *ip) {
    return nc_get_var1_double(ncid, varid, indexp, ip);
}
inline int put_var1(int ncid, int varid, const std::size_t *indexp, const unsigned short *op) {
    return nc_put_var1_ushort(ncid, varid, indexp, op);
}
inline int get_var1(int ncid, int varid, const std::size_t *indexp, unsigned short *ip) {
    return nc_get_var1_ushort(ncid, varid, indexp, ip);
}
inline int put_var1(int ncid, int varid, const std::size_t *indexp, const unsigned int *op) {
    return nc_put_var1_uint(ncid, varid, indexp, op);
}
inline int get_var1(int ncid, int varid, const std::size_t *indexp, unsigned int *ip) {
    return nc_get_var1_uint(ncid, varid, indexp, ip);
}
inline int put_var1(int ncid, int varid, const std::size_t *indexp, const long long *op) {
    return nc_put_var1_longlong(ncid, varid, indexp, op);
}
inline int get_var1(int ncid, int varid, const std::size_t *indexp, long long *ip) {
    return nc_get_var1_longlong(ncid, varid, indexp, ip);
}
inline int put_var1(int ncid, int varid, const std::size_t *indexp, const unsigned long long *op) {
    return nc_put_var1_ulonglong(ncid, varid, indexp, op);
}
inline int get_var1(int ncid, int varid, const std::size_t *indexp, unsigned long long *ip) {
    return nc_get_var1_ulonglong(ncid, varid, indexp, ip);
}
inline int put_var1(int ncid, int varid, const std::size_t *indexp, const char **op) {
    return nc_put_var1_string(ncid, varid, indexp, op);
}
inline int get_var1(int ncid, int varid, const std::size_t *indexp, char **ip) {
    return nc_get_var1_string(ncid, varid, indexp, ip);
}

#pragma endregion

#pragma region {put,get}_vara

inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const char *op) {
    return nc_put_vara_text(ncid, varid, startp, countp, op);
}
inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, char *ip) {
    return nc_get_vara_text(ncid, varid, startp, countp, ip);
}
inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const unsigned char *op) {
    return nc_put_vara_uchar(ncid, varid, startp, countp, op);
}
inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, unsigned char *ip) {
    return nc_get_vara_uchar(ncid, varid, startp, countp, ip);
}
inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const signed char *op) {
    return nc_put_vara_schar(ncid, varid, startp, countp, op);
}
inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, signed char *ip) {
    return nc_get_vara_schar(ncid, varid, startp, countp, ip);
}
inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const short *op) {
    return nc_put_vara_short(ncid, varid, startp, countp, op);
}
inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, short *ip) {
    return nc_get_vara_short(ncid, varid, startp, countp, ip);
}
inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const int *op) {
    return nc_put_vara_int(ncid, varid, startp, countp, op);
}
inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, int *ip) {
    return nc_get_vara_int(ncid, varid, startp, countp, ip);
}
inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const long *op) {
    return nc_put_vara_long(ncid, varid, startp, countp, op);
}
inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, long *ip) {
    return nc_get_vara_long(ncid, varid, startp, countp, ip);
}
inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const float *op) {
    return nc_put_vara_float(ncid, varid, startp, countp, op);
}
inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, float *ip) {
    return nc_get_vara_float(ncid, varid, startp, countp, ip);
}
inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const double *op) {
    return nc_put_vara_double(ncid, varid, startp, countp, op);
}
inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, double *ip) {
    return nc_get_vara_double(ncid, varid, startp, countp, ip);
}
inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const unsigned short *op) {
    return nc_put_vara_ushort(ncid, varid, startp, countp, op);
}
inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, unsigned short *ip) {
    return nc_get_vara_ushort(ncid, varid, startp, countp, ip);
}
inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const unsigned int *op) {
    return nc_put_vara_uint(ncid, varid, startp, countp, op);
}
inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, unsigned int *ip) {
    return nc_get_vara_uint(ncid, varid, startp, countp, ip);
}
inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const long long *op) {
    return nc_put_vara_longlong(ncid, varid, startp, countp, op);
}
inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, long long *ip) {
    return nc_get_vara_longlong(ncid, varid, startp, countp, ip);
}
inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const unsigned long long *op) {
    return nc_put_vara_ulonglong(ncid, varid, startp, countp, op);
}
inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, unsigned long long *ip) {
    return nc_get_vara_ulonglong(ncid, varid, startp, countp, ip);
}
inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const char **op) {
    return nc_put_vara_string(ncid, varid, startp, countp, op);
}
inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, char **ip) {
    return nc_get_vara_string(ncid, varid, startp, countp, ip);
}

#pragma endregion

#pragma region {put,get}_vars

inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const char *op) {
    return nc_put_vars_text(ncid, varid, startp, countp, stridep, op);
}
inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, char *ip) {
    return nc_get_vars_text(ncid, varid, startp, countp, stridep, ip);
}
inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const unsigned char *op) {
    return nc_put_vars_uchar(ncid, varid, startp, countp, stridep, op);
}
inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, unsigned char *ip) {
    return nc_get_vars_uchar(ncid, varid, startp, countp, stridep, ip);
}
inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const signed char *op) {
    return nc_put_vars_schar(ncid, varid, startp, countp, stridep, op);
}
inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, signed char *ip) {
    return nc_get_vars_schar(ncid, varid, startp, countp, stridep, ip);
}
inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const short *op) {
    return nc_put_vars_short(ncid, varid, startp, countp, stridep, op);
}
inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, short *ip) {
    return nc_get_vars_short(ncid, varid, startp, countp, stridep, ip);
}
inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const int *op) {
    return nc_put_vars_int(ncid, varid, startp, countp, stridep, op);
}
inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, int *ip) {
    return nc_get_vars_int(ncid, varid, startp, countp, stridep, ip);
}
inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const long *op) {
    return nc_put_vars_long(ncid, varid, startp, countp, stridep, op);
}
inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, long *ip) {
    return nc_get_vars_long(ncid, varid, startp, countp, stridep, ip);
}
inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const float *op) {
    return nc_put_vars_float(ncid, varid, startp, countp, stridep, op);
}
inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, float *ip) {
    return nc_get_vars_float(ncid, varid, startp, countp, stridep, ip);
}
inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const double *op) {
    return nc_put_vars_double(ncid, varid, startp, countp, stridep, op);
}
inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, double *ip) {
    return nc_get_vars_double(ncid, varid, startp, countp, stridep, ip);
}
inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const unsigned short *op) {
    return nc_put_vars_ushort(ncid, varid, startp, countp, stridep, op);
}
inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, unsigned short *ip) {
    return nc_get_vars_ushort(ncid, varid, startp, countp, stridep, ip);
}
inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const unsigned int *op) {
    return nc_put_vars_uint(ncid, varid, startp, countp, stridep, op);
}
inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, unsigned int *ip) {
    return nc_get_vars_uint(ncid, varid, startp, countp, stridep, ip);
}
inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const long long *op) {
    return nc_put_vars_longlong(ncid, varid, startp, countp, stridep, op);
}
inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, long long *ip) {
    return nc_get_vars_longlong(ncid, varid, startp, countp, stridep, ip);
}
inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const unsigned long long *op) {
    return nc_put_vars_ulonglong(ncid, varid, startp, countp, stridep, op);
}
inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, unsigned long long *ip) {
    return nc_get_vars_ulonglong(ncid, varid, startp, countp, stridep, ip);
}
inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const char **op) {
    return nc_put_vars_string(ncid, varid, startp, countp, stridep, op);
}
inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, char **ip) {
    return nc_get_vars_string(ncid, varid, startp, countp, stridep, ip);
}

#pragma endregion

#pragma region {put,get}_vars

inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const char *op) {
    return nc_put_varm_text(ncid, varid, startp, countp, stridep, imapp, op);
}
inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, char *ip) {
    return nc_get_varm_text(ncid, varid, startp, countp, stridep, imapp, ip);
}
inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const unsigned char *op) {
    return nc_put_varm_uchar(ncid, varid, startp, countp, stridep, imapp, op);
}
inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, unsigned char *ip) {
    return nc_get_varm_uchar(ncid, varid, startp, countp, stridep, imapp, ip);
}
inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const signed char *op) {
    return nc_put_varm_schar(ncid, varid, startp, countp, stridep, imapp, op);
}
inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, signed char *ip) {
    return nc_get_varm_schar(ncid, varid, startp, countp, stridep, imapp, ip);
}
inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const short *op) {
    return nc_put_varm_short(ncid, varid, startp, countp, stridep, imapp, op);
}
inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, short *ip) {
    return nc_get_varm_short(ncid, varid, startp, countp, stridep, imapp, ip);
}
inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const int *op) {
    return nc_put_varm_int(ncid, varid, startp, countp, stridep, imapp, op);
}
inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, int *ip) {
    return nc_get_varm_int(ncid, varid, startp, countp, stridep, imapp, ip);
}
inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const long *op) {
    return nc_put_varm_long(ncid, varid, startp, countp, stridep, imapp, op);
}
inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, long *ip) {
    return nc_get_varm_long(ncid, varid, startp, countp, stridep, imapp, ip);
}
inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const float *op) {
    return nc_put_varm_float(ncid, varid, startp, countp, stridep, imapp, op);
}
inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, float *ip) {
    return nc_get_varm_float(ncid, varid, startp, countp, stridep, imapp, ip);
}
inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const double *op) {
    return nc_put_varm_double(ncid, varid, startp, countp, stridep, imapp, op);
}
inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, double *ip) {
    return nc_get_varm_double(ncid, varid, startp, countp, stridep, imapp, ip);
}
inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const unsigned short *op) {
    return nc_put_varm_ushort(ncid, varid, startp, countp, stridep, imapp, op);
}
inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, unsigned short *ip) {
    return nc_get_varm_ushort(ncid, varid, startp, countp, stridep, imapp, ip);
}
inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const unsigned int *op) {
    return nc_put_varm_uint(ncid, varid, startp, countp, stridep, imapp, op);
}
inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, unsigned int *ip) {
    return nc_get_varm_uint(ncid, varid, startp, countp, stridep, imapp, ip);
}
inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const long long *op) {
    return nc_put_varm_longlong(ncid, varid, startp, countp, stridep, imapp, op);
}
inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, long long *ip) {
    return nc_get_varm_longlong(ncid, varid, startp, countp, stridep, imapp, ip);
}
inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const unsigned long long *op) {
    return nc_put_varm_ulonglong(ncid, varid, startp, countp, stridep, imapp, op);
}
inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, unsigned long long *ip) {
    return nc_get_varm_ulonglong(ncid, varid, startp, countp, stridep, imapp, ip);
}
inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const char **op) {
    return nc_put_varm_string(ncid, varid, startp, countp, stridep, imapp, op);
}
inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, char **ip) {
    return nc_get_varm_string(ncid, varid, startp, countp, stridep, imapp, ip);
}

#pragma endregion

#pragma region {put,get}_var

inline int put_var(int ncid, int varid, const char *op) {
    return nc_put_var_text(ncid, varid, op);
}
inline int get_var(int ncid, int varid, char *ip) {
    return nc_get_var_text(ncid, varid, ip);
}
inline int put_var(int ncid, int varid, const unsigned char *op) {
    return nc_put_var_uchar(ncid, varid, op);
}
inline int get_var(int ncid, int varid, unsigned char *ip) {
    return nc_get_var_uchar(ncid, varid, ip);
}
inline int put_var(int ncid, int varid, const signed char *op) {
    return nc_put_var_schar(ncid, varid, op);
}
inline int get_var(int ncid, int varid, signed char *ip) {
    return nc_get_var_schar(ncid, varid, ip);
}
inline int put_var(int ncid, int varid, const short *op) {
    return nc_put_var_short(ncid, varid, op);
}
inline int get_var(int ncid, int varid, short *ip) {
    return nc_get_var_short(ncid, varid, ip);
}
inline int put_var(int ncid, int varid, const int *op) {
    return nc_put_var_int(ncid, varid, op);
}
inline int get_var(int ncid, int varid, int *ip) {
    return nc_get_var_int(ncid, varid, ip);
}
inline int put_var(int ncid, int varid, const long *op) {
    return nc_put_var_long(ncid, varid, op);
}
inline int get_var(int ncid, int varid, long *ip) {
    return nc_get_var_long(ncid, varid, ip);
}
inline int put_var(int ncid, int varid, const float *op) {
    return nc_put_var_float(ncid, varid, op);
}
inline int get_var(int ncid, int varid, float *ip) {
    return nc_get_var_float(ncid, varid, ip);
}
inline int put_var(int ncid, int varid, const double *op) {
    return nc_put_var_double(ncid, varid, op);
}
inline int get_var(int ncid, int varid, double *ip) {
    return nc_get_var_double(ncid, varid, ip);
}
inline int put_var(int ncid, int varid, const unsigned short *op) {
    return nc_put_var_ushort(ncid, varid, op);
}
inline int get_var(int ncid, int varid, unsigned short *ip) {
    return nc_get_var_ushort(ncid, varid, ip);
}
inline int put_var(int ncid, int varid, const unsigned int *op) {
    return nc_put_var_uint(ncid, varid, op);
}
inline int get_var(int ncid, int varid, unsigned int *ip) {
    return nc_get_var_uint(ncid, varid, ip);
}
inline int put_var(int ncid, int varid, const long long *op) {
    return nc_put_var_longlong(ncid, varid, op);
}
inline int get_var(int ncid, int varid, long long *ip) {
    return nc_get_var_longlong(ncid, varid, ip);
}
inline int put_var(int ncid, int varid, const unsigned long long *op) {
    return nc_put_var_ulonglong(ncid, varid, op);
}
inline int get_var(int ncid, int varid, unsigned long long *ip) {
    return nc_get_var_ulonglong(ncid, varid, ip);
}
inline int put_var(int ncid, int varid, const char **op) {
    return nc_put_var_string(ncid, varid, op);
}
inline int get_var(int ncid, int varid, char **ip) {
    return nc_get_var_string(ncid, varid, ip);
}

#pragma endregion

#pragma region {put,get}_att

inline int put_att(int ncid, int varid, const char *name, std::size_t len, const char *op) {
    return nc_put_att_text(ncid, varid, name, len, op);
}
inline int get_att(int ncid, int varid, const char *name, char *ip) {
    return nc_get_att_text(ncid, varid, name, ip);
}
inline int put_att(int ncid, int varid, const char *name, std::size_t len, const char **op) {
    return nc_put_att_string(ncid, varid, name, len, op);
}
inline int get_att(int ncid, int varid, const char *name, char **ip) {
    return nc_get_att_string(ncid, varid, name, ip);
}
inline int put_att(int ncid, int varid, const char *name, std::size_t len, const unsigned char *op) {
    return nc_put_att_uchar(ncid, varid, name, NC_UBYTE, len, op);
}
inline int get_att(int ncid, int varid, const char *name, unsigned char *ip) {
    return nc_get_att_uchar(ncid, varid, name, ip);
}
inline int put_att(int ncid, int varid, const char *name, std::size_t len, const signed char *op) {
    return nc_put_att_schar(ncid, varid, name, NC_BYTE, len, op);
}
inline int get_att(int ncid, int varid, const char *name, signed char *ip) {
    return nc_get_att_schar(ncid, varid, name, ip);
}
inline int put_att(int ncid, int varid, const char *name, std::size_t len, const short *op) {
    return nc_put_att_short(ncid, varid, name, NC_SHORT, len, op);
}
inline int get_att(int ncid, int varid, const char *name, short *ip) {
    return nc_get_att_short(ncid, varid, name, ip);
}
inline int put_att(int ncid, int varid, const char *name, std::size_t len, const int *op) {
    return nc_put_att_int(ncid, varid, name, NC_INT, len, op);
}
inline int get_att(int ncid, int varid, const char *name, int *ip) {
    return nc_get_att_int(ncid, varid, name, ip);
}
inline int put_att(int ncid, int varid, const char *name, std::size_t len, const long *op) {
    return nc_put_att_long(ncid, varid, name, NC_LONG, len, op);
}
inline int get_att(int ncid, int varid, const char *name, long *ip) {
    return nc_get_att_long(ncid, varid, name, ip);
}
inline int put_att(int ncid, int varid, const char *name, std::size_t len, const float *op) {
    return nc_put_att_float(ncid, varid, name, NC_FLOAT, len, op);
}
inline int get_att(int ncid, int varid, const char *name, float *ip) {
    return nc_get_att_float(ncid, varid, name, ip);
}
inline int put_att(int ncid, int varid, const char *name, std::size_t len, const double *op) {
    return nc_put_att_double(ncid, varid, name, NC_DOUBLE, len, op);
}
inline int get_att(int ncid, int varid, const char *name, double *ip) {
    return nc_get_att_double(ncid, varid, name, ip);
}
inline int put_att(int ncid, int varid, const char *name, std::size_t len, const unsigned short *op) {
    return nc_put_att_ushort(ncid, varid, name, NC_USHORT, len, op);
}
inline int get_att(int ncid, int varid, const char *name, unsigned short *ip) {
    return nc_get_att_ushort(ncid, varid, name, ip);
}
inline int put_att(int ncid, int varid, const char *name, std::size_t len, const unsigned int *op) {
    return nc_put_att_uint(ncid, varid, name, NC_UINT, len, op);
}
inline int get_att(int ncid, int varid, const char *name, unsigned int *ip) {
    return nc_get_att_uint(ncid, varid, name, ip);
}
inline int put_att(int ncid, int varid, const char *name, std::size_t len, const long long *op) {
    return nc_put_att_longlong(ncid, varid, name, NC_INT64, len, op);
}
inline int get_att(int ncid, int varid, const char *name, long long *ip) {
    return nc_get_att_longlong(ncid, varid, name, ip);
}
inline int put_att(int ncid, int varid, const char *name, std::size_t len, const unsigned long long *op) {
    return nc_put_att_ulonglong(ncid, varid, name, NC_UINT64, len, op);
}
inline int get_att(int ncid, int varid, const char *name, unsigned long long *ip) {
    return nc_get_att_ulonglong(ncid, varid, name, ip);
}

#pragma endregion

} // namespace detail
} // namespace ncpp

#endif // NCPP_DISPATCH_HPP