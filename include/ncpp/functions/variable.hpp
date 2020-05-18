// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_FUNCTIONS_VARIABLE_HPP
#define NCPP_FUNCTIONS_VARIABLE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <netcdf.h>

#include <ncpp/config.hpp>
#include <ncpp/check.hpp>
#include <ncpp/error.hpp>
#include <ncpp/types.hpp>
#include <ncpp/detail/utilities.hpp>

#include <cstddef>
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <optional>
#include <string>
#include <system_error>
#include <type_traits>
#include <vector>

#ifdef NCPP_USE_DATE_H
// Implements C++20 extensions for <chrono>
// https://github.com/HowardHinnant/date
#include <date/date.h>
#include <array>
#include <istream>
#include <sstream>
#endif

namespace ncpp {
namespace impl {

// Get the ID of a variable, or std::nullopt if undefined.
inline std::optional<int> inq_varid(int ncid, const std::string& varname, std::error_code *ec = nullptr)
{
    int varid;
    check(nc_inq_varid(ncid, varname.c_str(), &varid), ec);
    if (ec && ec->value())
        return {};
    return varid;
}

// Get the name of a variable, or empty string if undefined.
inline std::string inq_varname(int ncid, int varid, std::error_code *ec = nullptr)
{
    char varname[NC_MAX_NAME + 1];
    check(nc_inq_varname(ncid, varid, varname), ec);
    return (ec && ec->value()) ? std::string() : varname;
}

// Get the netCDF type of a variable, or NC_NAT if undefined.
inline int inq_vartype(int ncid, int varid, std::error_code *ec = nullptr)
{
    int vartype;
    check(nc_inq_vartype(ncid, varid, &vartype), ec);
    if (ec && ec->value())
        return NC_NAT;
    return vartype;
}

// Get the number of attributes associated with a variable.
inline int inq_varnatts(int ncid, int varid, std::error_code *ec = nullptr)
{
    int natts = 0;
    check(nc_inq_varnatts(ncid, varid, &natts), ec);
    if (ec && ec->value())
        return 0;
    return natts;
}

// Get the number of dimensions associated with a variable.
inline int inq_varndims(int ncid, int varid, std::error_code *ec = nullptr)
{
    int ndims = 0;
    check(nc_inq_varndims(ncid, varid, &ndims), ec);
    if (ec && ec->value())
        return 0;
    return ndims;
}

// Get the dimension IDs associated with a variable.
inline std::vector<int> inq_vardimid(int ncid, int varid, std::error_code *ec = nullptr)
{
    std::vector<int> dimids;
    int ndims = inq_varndims(ncid, varid, ec);
    if ((ec && !ec->value()) || ndims > 0) {
        dimids.resize(static_cast<std::size_t>(ndims));
        check(nc_inq_vardimid(ncid, varid, dimids.data()), ec);
    }
    return dimids;
}

// EXTENSION: get the shape of a variable from the associated dimensions.
inline index_type inq_varshape(int ncid, int varid, std::error_code *ec = nullptr)
{
    index_type shape;
    auto dimids = inq_vardimid(ncid, varid, ec);
    if (ec && ec->value())
        return shape;

    shape.reserve(dimids.size());    
    for (const auto& dimid : dimids) {
        auto dimlen = inq_dimlen(ncid, dimid, ec);
        if (ec && ec->value())
            return shape;
        shape.push_back(dimlen);
    }
    return shape;
}

// EXTENSION: get the length of a variable from the associated dimensions.
inline std::size_t inq_varlen(int ncid, int varid, std::error_code *ec = nullptr)
{
    std::vector<std::size_t> shape = inq_varshape(ncid, varid, ec);
    if (ec && ec->value())
        return 0;

    std::size_t varlen = 1;
    for (const auto& dimlen : shape) {
        std::size_t x = varlen * dimlen;
        if (varlen != 0 && x / varlen != dimlen) { // Handle EOVERFLOW
            if (ec)
                *ec = std::make_error_code(std::errc::value_too_large);
            else
                ncpp::detail::throw_error(std::make_error_code(std::errc::value_too_large));
            return 0;
        }
        varlen = x;
    }
    return varlen;
}

// Get the endianness of a variable, or NC_ENDIAN_NATIVE if not set.
inline var_endian_type inq_var_endian(int ncid, int varid, std::error_code *ec = nullptr)
{
    int endian;
    check(nc_inq_var_endian(ncid, varid, &endian), ec);
    if (ec && ec->value())
        return static_cast<var_endian_type>(NC_ENDIAN_NATIVE);
    return static_cast<var_endian_type>(endian);
}

// Get the fill value of a variable, or std::nullopt if not set.
template <class T>
inline std::optional<T> inq_var_fill(int ncid, int varid, std::error_code *ec = nullptr)
{
    int vartype = inq_vartype(ncid, varid, ec);
    if (vartype != type_to_netcdf_type<T>::type.value) {
        check(NC_EBADTYPE, ec); // Not a valid data type or _FillValue type mismatch
        return {};
    }

    int fill_mode;
    T fill_value;
    check(nc_inq_var_fill(ncid, varid, &fill_mode, &fillvalue), ec);
    if (ec && ec->value())
        return {};
    
    if (fill_mode == NC_NOFILL)
        return {};
    
    return fill_value;
}

// Get the storage type for a variable, or std::nullopt if undefined.
inline std::optional<var_storage_type> inq_var_storage(int ncid, int varid, std::error_code *ec = nullptr)
{
    int storage;
    check(nc_inq_var_chunking(ncid, varid, &storage, nullptr), ec);
    if (ec && ec->value())
        return {};
    return static_cast<var_storage_type>(storage);
}

// Get the chunksizes of a chunked variable.
inline std::vector<std::size_t> inq_var_chunksizes(int ncid, int varid, std::error_code *ec = nullptr)
{
    std::vector<std::size_t> chunksizes;
    int ndims = inq_varndims(ncid, varid, ec);
    if ((ec && !ec->value()) || ndims > 0) {
        chunksizes.resize(ndims, 0);
        check(nc_inq_var_chunking(ncid, varid, nullptr, chunksizes.data()), ec);
    }
    return chunksizes;
}

// Get the filter ID for a variable, or std::nullopt if undefined.
// Known filter IDs: https://portal.hdfgroup.org/display/support/Filters
inline std::optional<unsigned int> inq_var_filter_id(int ncid, int varid, std::error_code *ec = nullptr)
{
    unsigned int filterid = 0;
    check(nc_inq_var_filter(ncid, varid, &filterid, nullptr, nullptr), ec);
    if (ec && ec->value())
        return {};
    return filterid;
}

// EXTENSION: get the filter name for a variable, or empty string if undefined.
// Known filter IDs: https://portal.hdfgroup.org/display/support/Filters
inline std::string inq_var_filter_name(int ncid, int varid, std::error_code *ec = nullptr)
{
    unsigned int filterid = 0;
    check(nc_inq_var_filter(ncid, varid, &filterid, nullptr, nullptr), ec);
    if (ec && ec->value())
        return std::string();
    
    switch (filterid) {
    case 305:   return "LZO";
    case 307:   return "BZIP2";
    case 32000: return "LZF";
    case 32001: return "Blosc";
    case 32002: return "MAFISC";
    case 32003: return "Snappy";
    case 32004: return "LZ4";
    case 32005: return "APAX";
    case 32006: return "CBF";
    case 32007: return "JPEG-XR";
    case 32008: return "bitshuffle";
    case 32009: return "SPDP";
    case 32010: return "LPC-Rice";
    case 32011: return "CCSDS-123";
    case 32012: return "JPEG-LS";
    case 32013: return "zfp";
    case 32014: return "fpzip";
    case 32015: return "Zstandard";
    case 32016: return "B3D";
    case 32017: return "SZ";
    case 32018: return "FCIDECOMP";
    case 32019: return "JPEG";
    case 32020: return "VBZ";
    default:    return "Unknown";
    }
}

// Get the per-variable chunk cache settings from the HDF5 layer.
inline var_chunk_cache get_var_chunk_cache(int ncid, int varid, std::error_code *ec = nullptr)
{
    var_chunk_cache result = { 0 };
    check(nc_get_var_chunk_cache(ncid, varid, &result.size, &result.nelems, &result.preemption), ec);
    return result;
}

namespace detail {

    inline int put_var1(int ncid, int varid, const std::size_t *indexp, const char *op)
        { return nc_put_var1_text(ncid, varid, indexp, op); }
    inline int get_var1(int ncid, int varid, const std::size_t *indexp, char *ip)
        { return nc_get_var1_text(ncid, varid, indexp, ip); }
    inline int put_var1(int ncid, int varid, const std::size_t *indexp, const unsigned char *op)
        { return nc_put_var1_uchar(ncid, varid, indexp, op); }
    inline int get_var1(int ncid, int varid, const std::size_t *indexp, unsigned char *ip)
        { return nc_get_var1_uchar(ncid, varid, indexp, ip); }
    inline int put_var1(int ncid, int varid, const std::size_t *indexp, const signed char *op)
        { return nc_put_var1_schar(ncid, varid, indexp, op); }
    inline int get_var1(int ncid, int varid, const std::size_t *indexp, signed char *ip)
        { return nc_get_var1_schar(ncid, varid, indexp, ip); }
    inline int put_var1(int ncid, int varid, const std::size_t *indexp, const short *op)
        { return nc_put_var1_short(ncid, varid, indexp, op); }
    inline int get_var1(int ncid, int varid, const std::size_t *indexp, short *ip)
        { return nc_get_var1_short(ncid, varid, indexp, ip); }
    inline int put_var1(int ncid, int varid, const std::size_t *indexp, const int *op)
        { return nc_put_var1_int(ncid, varid, indexp, op); }
    inline int get_var1(int ncid, int varid, const std::size_t *indexp, int *ip)
        { return nc_get_var1_int(ncid, varid, indexp, ip); }
    inline int put_var1(int ncid, int varid, const std::size_t *indexp, const long *op)
        { return nc_put_var1_long(ncid, varid, indexp, op); }
    inline int get_var1(int ncid, int varid, const std::size_t *indexp, long *ip)
        { return nc_get_var1_long(ncid, varid, indexp, ip); }
    inline int put_var1(int ncid, int varid, const std::size_t *indexp, const float *op)
        { return nc_put_var1_float(ncid, varid, indexp, op); }
    inline int get_var1(int ncid, int varid, const std::size_t *indexp, float *ip)
        { return nc_get_var1_float(ncid, varid, indexp, ip); }
    inline int put_var1(int ncid, int varid, const std::size_t *indexp, const double *op)
        { return nc_put_var1_double(ncid, varid, indexp, op); }
    inline int get_var1(int ncid, int varid, const std::size_t *indexp, double *ip)
        { return nc_get_var1_double(ncid, varid, indexp, ip); }
    inline int put_var1(int ncid, int varid, const std::size_t *indexp, const unsigned short *op)
        { return nc_put_var1_ushort(ncid, varid, indexp, op); }
    inline int get_var1(int ncid, int varid, const std::size_t *indexp, unsigned short *ip)
        { return nc_get_var1_ushort(ncid, varid, indexp, ip); }
    inline int put_var1(int ncid, int varid, const std::size_t *indexp, const unsigned int *op)
        { return nc_put_var1_uint(ncid, varid, indexp, op); }
    inline int get_var1(int ncid, int varid, const std::size_t *indexp, unsigned int *ip)
        { return nc_get_var1_uint(ncid, varid, indexp, ip); }
    inline int put_var1(int ncid, int varid, const std::size_t *indexp, const long long *op)
        { return nc_put_var1_longlong(ncid, varid, indexp, op); }
    inline int get_var1(int ncid, int varid, const std::size_t *indexp, long long *ip)
        { return nc_get_var1_longlong(ncid, varid, indexp, ip); }
    inline int put_var1(int ncid, int varid, const std::size_t *indexp, const unsigned long long *op)
        { return nc_put_var1_ulonglong(ncid, varid, indexp, op); }
    inline int get_var1(int ncid, int varid, const std::size_t *indexp, unsigned long long *ip)
        { return nc_get_var1_ulonglong(ncid, varid, indexp, ip); }
    inline int put_var1(int ncid, int varid, const std::size_t *indexp, const char **op)
        { return nc_put_var1_string(ncid, varid, indexp, op); }
    inline int get_var1(int ncid, int varid, const std::size_t *indexp, char **ip)
        { return nc_get_var1_string(ncid, varid, indexp, ip); }

    inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const char *op)
        { return nc_put_vara_text(ncid, varid, startp, countp, op); }
    inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, char *ip)
        { return nc_get_vara_text(ncid, varid, startp, countp, ip); }
    inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const unsigned char *op)
        { return nc_put_vara_uchar(ncid, varid, startp, countp, op); }
    inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, unsigned char *ip)
        { return nc_get_vara_uchar(ncid, varid, startp, countp, ip); }
    inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const signed char *op)
        { return nc_put_vara_schar(ncid, varid, startp, countp, op); }
    inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, signed char *ip)
        { return nc_get_vara_schar(ncid, varid, startp, countp, ip); }
    inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const short *op)
        { return nc_put_vara_short(ncid, varid, startp, countp, op); }
    inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, short *ip)
        { return nc_get_vara_short(ncid, varid, startp, countp, ip); }
    inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const int *op)
        { return nc_put_vara_int(ncid, varid, startp, countp, op); }
    inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, int *ip)
        { return nc_get_vara_int(ncid, varid, startp, countp, ip); }
    inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const long *op)
        { return nc_put_vara_long(ncid, varid, startp, countp, op); }
    inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, long *ip)
        { return nc_get_vara_long(ncid, varid, startp, countp, ip); }
    inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const float *op)
        { return nc_put_vara_float(ncid, varid, startp, countp, op); }
    inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, float *ip)
        { return nc_get_vara_float(ncid, varid, startp, countp, ip); }
    inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const double *op)
        { return nc_put_vara_double(ncid, varid, startp, countp, op); }
    inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, double *ip)
        { return nc_get_vara_double(ncid, varid, startp, countp, ip); }
    inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const unsigned short *op)
        { return nc_put_vara_ushort(ncid, varid, startp, countp, op); }
    inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, unsigned short *ip)
        { return nc_get_vara_ushort(ncid, varid, startp, countp, ip); }
    inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const unsigned int *op)
        { return nc_put_vara_uint(ncid, varid, startp, countp, op); }
    inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, unsigned int *ip)
        { return nc_get_vara_uint(ncid, varid, startp, countp, ip); }
    inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const long long *op)
        { return nc_put_vara_longlong(ncid, varid, startp, countp, op); }
    inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, long long *ip)
        { return nc_get_vara_longlong(ncid, varid, startp, countp, ip); }
    inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const unsigned long long *op)
        { return nc_put_vara_ulonglong(ncid, varid, startp, countp, op); }
    inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, unsigned long long *ip)
        { return nc_get_vara_ulonglong(ncid, varid, startp, countp, ip); }
    inline int put_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const char **op)
        { return nc_put_vara_string(ncid, varid, startp, countp, op); }
    inline int get_vara(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, char **ip)
        { return nc_get_vara_string(ncid, varid, startp, countp, ip); }

    inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const char *op)
        { return nc_put_vars_text(ncid, varid, startp, countp, stridep, op); }
    inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, char *ip)
        { return nc_get_vars_text(ncid, varid, startp, countp, stridep, ip); }
    inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const unsigned char *op)
        { return nc_put_vars_uchar(ncid, varid, startp, countp, stridep, op); }
    inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, unsigned char *ip)
        { return nc_get_vars_uchar(ncid, varid, startp, countp, stridep, ip); }
    inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const signed char *op)
        { return nc_put_vars_schar(ncid, varid, startp, countp, stridep, op); }
    inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, signed char *ip)
        { return nc_get_vars_schar(ncid, varid, startp, countp, stridep, ip); }
    inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const short *op)
        { return nc_put_vars_short(ncid, varid, startp, countp, stridep, op); }
    inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, short *ip)
        { return nc_get_vars_short(ncid, varid, startp, countp, stridep, ip); }
    inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const int *op)
        { return nc_put_vars_int(ncid, varid, startp, countp, stridep, op); }
    inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, int *ip)
        { return nc_get_vars_int(ncid, varid, startp, countp, stridep, ip); }
    inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const long *op)
        { return nc_put_vars_long(ncid, varid, startp, countp, stridep, op); }
    inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, long *ip)
        { return nc_get_vars_long(ncid, varid, startp, countp, stridep, ip); }
    inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const float *op)
        { return nc_put_vars_float(ncid, varid, startp, countp, stridep, op); }
    inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, float *ip)
        { return nc_get_vars_float(ncid, varid, startp, countp, stridep, ip); }
    inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const double *op)
        { return nc_put_vars_double(ncid, varid, startp, countp, stridep, op); }
    inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, double *ip)
        { return nc_get_vars_double(ncid, varid, startp, countp, stridep, ip); }
    inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const unsigned short *op)
        { return nc_put_vars_ushort(ncid, varid, startp, countp, stridep, op); }
    inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, unsigned short *ip)
        { return nc_get_vars_ushort(ncid, varid, startp, countp, stridep, ip); }
    inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const unsigned int *op)
        { return nc_put_vars_uint(ncid, varid, startp, countp, stridep, op); }
    inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, unsigned int *ip)
        { return nc_get_vars_uint(ncid, varid, startp, countp, stridep, ip); }
    inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const long long *op)
        { return nc_put_vars_longlong(ncid, varid, startp, countp, stridep, op); }
    inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, long long *ip)
        { return nc_get_vars_longlong(ncid, varid, startp, countp, stridep, ip); }
    inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const unsigned long long *op)
        { return nc_put_vars_ulonglong(ncid, varid, startp, countp, stridep, op); }
    inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, unsigned long long *ip)
        { return nc_get_vars_ulonglong(ncid, varid, startp, countp, stridep, ip); }
    inline int put_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const char **op)
        { return nc_put_vars_string(ncid, varid, startp, countp, stridep, op); }
    inline int get_vars(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, char **ip)
        { return nc_get_vars_string(ncid, varid, startp, countp, stridep, ip); }

    inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const char *op)
        { return nc_put_varm_text(ncid, varid, startp, countp, stridep, imapp, op); }
    inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, char *ip)
        { return nc_get_varm_text(ncid, varid, startp, countp, stridep, imapp, ip); }
    inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const unsigned char *op)
        { return nc_put_varm_uchar(ncid, varid, startp, countp, stridep, imapp, op); }
    inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, unsigned char *ip)
        { return nc_get_varm_uchar(ncid, varid, startp, countp, stridep, imapp, ip); }
    inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const signed char *op)
        { return nc_put_varm_schar(ncid, varid, startp, countp, stridep, imapp, op); }
    inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, signed char *ip)
        { return nc_get_varm_schar(ncid, varid, startp, countp, stridep, imapp, ip); }
    inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const short *op)
        { return nc_put_varm_short(ncid, varid, startp, countp, stridep, imapp, op); }
    inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, short *ip)
        { return nc_get_varm_short(ncid, varid, startp, countp, stridep, imapp, ip); }
    inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const int *op)
        { return nc_put_varm_int(ncid, varid, startp, countp, stridep, imapp, op); }
    inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, int *ip)
        { return nc_get_varm_int(ncid, varid, startp, countp, stridep, imapp, ip); }
    inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const long *op)
        { return nc_put_varm_long(ncid, varid, startp, countp, stridep, imapp, op); }
    inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, long *ip)
        { return nc_get_varm_long(ncid, varid, startp, countp, stridep, imapp, ip); }
    inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const float *op)
        { return nc_put_varm_float(ncid, varid, startp, countp, stridep, imapp, op); }
    inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, float *ip)
        { return nc_get_varm_float(ncid, varid, startp, countp, stridep, imapp, ip); }
    inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const double *op)
        { return nc_put_varm_double(ncid, varid, startp, countp, stridep, imapp, op); }
    inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, double *ip)
        { return nc_get_varm_double(ncid, varid, startp, countp, stridep, imapp, ip); }
    inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const unsigned short *op)
        { return nc_put_varm_ushort(ncid, varid, startp, countp, stridep, imapp, op); }
    inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, unsigned short *ip)
        { return nc_get_varm_ushort(ncid, varid, startp, countp, stridep, imapp, ip); }
    inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const unsigned int *op)
        { return nc_put_varm_uint(ncid, varid, startp, countp, stridep, imapp, op); }
    inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, unsigned int *ip)
        { return nc_get_varm_uint(ncid, varid, startp, countp, stridep, imapp, ip); }
    inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const long long *op)
        { return nc_put_varm_longlong(ncid, varid, startp, countp, stridep, imapp, op); }
    inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, long long *ip)
        { return nc_get_varm_longlong(ncid, varid, startp, countp, stridep, imapp, ip); }
    inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const unsigned long long *op)
        { return nc_put_varm_ulonglong(ncid, varid, startp, countp, stridep, imapp, op); }
    inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, unsigned long long *ip)
        { return nc_get_varm_ulonglong(ncid, varid, startp, countp, stridep, imapp, ip); }
    inline int put_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, const char **op)
        { return nc_put_varm_string(ncid, varid, startp, countp, stridep, imapp, op); }
    inline int get_varm(int ncid, int varid, const std::size_t *startp, const std::size_t *countp, const ptrdiff_t *stridep, const ptrdiff_t *imapp, char **ip)
        { return nc_get_varm_string(ncid, varid, startp, countp, stridep, imapp, ip); }

    inline int put_var(int ncid, int varid, const char *op)
        { return nc_put_var_text(ncid, varid, op); }
    inline int get_var(int ncid, int varid, char *ip)
        { return nc_get_var_text(ncid, varid, ip); }
    inline int put_var(int ncid, int varid, const unsigned char *op)
        { return nc_put_var_uchar(ncid, varid, op); }
    inline int get_var(int ncid, int varid, unsigned char *ip)
        { return nc_get_var_uchar(ncid, varid, ip); }
    inline int put_var(int ncid, int varid, const signed char *op)
        { return nc_put_var_schar(ncid, varid, op); }
    inline int get_var(int ncid, int varid, signed char *ip)
        { return nc_get_var_schar(ncid, varid, ip); }
    inline int put_var(int ncid, int varid, const short *op)
        { return nc_put_var_short(ncid, varid, op); }
    inline int get_var(int ncid, int varid, short *ip)
        { return nc_get_var_short(ncid, varid, ip); }
    inline int put_var(int ncid, int varid, const int *op)
        { return nc_put_var_int(ncid, varid, op); }
    inline int get_var(int ncid, int varid, int *ip)
        { return nc_get_var_int(ncid, varid, ip); }
    inline int put_var(int ncid, int varid, const long *op)
        { return nc_put_var_long(ncid, varid, op); }
    inline int get_var(int ncid, int varid, long *ip)
        { return nc_get_var_long(ncid, varid, ip); }
    inline int put_var(int ncid, int varid, const float *op)
        { return nc_put_var_float(ncid, varid, op); }
    inline int get_var(int ncid, int varid, float *ip)
        { return nc_get_var_float(ncid, varid, ip); }
    inline int put_var(int ncid, int varid, const double *op)
        { return nc_put_var_double(ncid, varid, op); }
    inline int get_var(int ncid, int varid, double *ip)
        { return nc_get_var_double(ncid, varid, ip); }
    inline int put_var(int ncid, int varid, const unsigned short *op)
        { return nc_put_var_ushort(ncid, varid, op); }
    inline int get_var(int ncid, int varid, unsigned short *ip)
        { return nc_get_var_ushort(ncid, varid, ip); }
    inline int put_var(int ncid, int varid, const unsigned int *op)
        { return nc_put_var_uint(ncid, varid, op); }
    inline int get_var(int ncid, int varid, unsigned int *ip)
        { return nc_get_var_uint(ncid, varid, ip); }
    inline int put_var(int ncid, int varid, const long long *op)
        { return nc_put_var_longlong(ncid, varid, op); }
    inline int get_var(int ncid, int varid, long long *ip)
        { return nc_get_var_longlong(ncid, varid, ip); }
    inline int put_var(int ncid, int varid, const unsigned long long *op)
        { return nc_put_var_ulonglong(ncid, varid, op); }
    inline int get_var(int ncid, int varid, unsigned long long *ip)
        { return nc_get_var_ulonglong(ncid, varid, ip); }
    inline int put_var(int ncid, int varid, const char **op)
        { return nc_put_var_string(ncid, varid, op); }
    inline int get_var(int ncid, int varid, char **ip)
        { return nc_get_var_string(ncid, varid, ip); }

} // namespace detail


// Get a variable with arithmetic type as an array.
template <class Container>
typename std::enable_if_t<std::is_arithmetic_v<typename Container::value_type>, Container>
get_vars(int ncid, int varid,
         const index_type& start,
         const index_type& shape,
         const stride_type& stride,
         std::error_code *ec = nullptr)
{
    Container result;

    int ndims = inq_varndims(ncid, varid, ec);
    if (ec && ec->value())
        return result;

    if (ndims <= 0 || start.size() != ndims || shape.size() != ndims || stride.size() != ndims) {
        check(NC_EINVALCOORDS, ec); // Index exceeds dimension bound
        return result;
    }

    std::size_t varlen = inq_varlen(ncid, varid, ec);
    if (ec && ec->value())
        return result;

    std::size_t n = std::accumulate(shape.begin(), shape.end(), 1ull,
        std::multiplies<std::size_t>());
    
    if (n > 0 && n <= varlen) {
        result.resize(n);
        check(detail::get_vars(ncid, varid, start.data(), shape.data(), stride.data(), result.data()), ec);
    }

    return result;
}

// Read a single datum from a variable with arithmetic type.
template <class T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
get_var1(int ncid, int varid, const index_type& start, std::error_code *ec = nullptr)
{
    T result = {};
    
    int ndims = inq_varndims(ncid, varid, ec);
    if (ec && ec->value())
        return result;

    if (ndims <= 0 || start.size() != ndims) {
        check(NC_EINVALCOORDS, ec); // Index exceeds dimension bound
        return result;
    }

    check(detail::get_var1(ncid, varid, start.data(), &result), ec);
    return result;
}

// Get a variable with string type (`NC_CHAR` or `NC_STRING`) as an array.
template <class Container>
typename std::enable_if_t<std::is_same_v<typename Container::value_type, std::string>, Container>
get_vars(int ncid, int varid,
         const index_type& start,
         const index_type& shape,
         const stride_type& stride,
         std::error_code *ec = nullptr)
{
    Container result;

    int ndims = inq_varndims(ncid, varid, ec);
    if (ec && ec->value())
        return result;

    if (ndims <= 0 || start.size() != ndims || shape.size() != ndims || stride.size() != ndims) {
        check(NC_EINVALCOORDS, ec); // Index exceeds dimension bound
        return result;
    }

    const int nct = inq_vartype(ncid, varid, ec);
    if (ec && ec->value())
        return result;

    if (nct == NC_CHAR) {
        // For classic strings, the character position is the last dimension.
        std::size_t vlen = std::accumulate(shape.begin(), std::prev(shape.end()), (std::size_t)1,
            std::multiplies<std::size_t>());
        std::size_t slen = shape.back();

        // Read the array into a buffer.
        std::string buffer;
        buffer.resize(vlen * slen);
        check(nc_get_vars_text(ncid, varid, start.data(), shape.data(), stride.data(), &buffer[0]), ec);

        // Iterate over the buffer and extract fixed-width strings.
        result.reserve(vlen);
        for (int i = 0; i < vlen; ++i) {
            std::string s = buffer.substr(i * slen, slen);
            s.erase(s.find_last_not_of(' ') + 1);
            result.push_back(s);
        }
    }
    else if (nct == NC_STRING) {
        std::size_t n = std::accumulate(shape.begin(), shape.end(), (std::size_t)1,
            std::multiplies<std::size_t>());
        std::vector<char *> pv(n, nullptr);

        check(nc_get_vars_string(ncid, varid, start.data(), shape.data(), stride.data(), pv.data()), ec);
        if (ec && ec->value())
            return result;
        
        result.reserve(n);
        for (const auto& p : pv) {
            if (p)
                result.emplace_back(std::string(p));
        }
        
        nc_free_string(n, pv.data());
    }
    else {
        check(NC_ECHAR, ec); // Attempt to convert between text & numbers
    }
    
    return result;
}

// Read a single datum from a variable with string type (`NC_CHAR` or `NC_STRING`).
template <class T>
typename std::enable_if_t<std::is_same_v<T, std::string>, T>
get_var1(int ncid, int varid, const index_type& start, std::error_code *ec = nullptr)
{
    T result = {};
    
    int ndims = inq_varndims(ncid, varid, ec);
    if (ec && ec->value())
        return result;
    
    if (start.size() != shape.size()) {
        check(NC_EINVALCOORDS, ec); // Index exceeds dimension bound
        return result;
    }

    const int nct = inq_vartype(ncid, varid, ec);
    if (ec && ec->value())
        return result;

    if (nct == NC_CHAR) {
        // For classic strings, the character position is the last dimension.
        index_type shape = get_varshape(ncid, varid, ec);
        if (ec && ec->value())
            return result;
        
        std::fill(shape.begin(), std::prev(shape.end()), (std::size_t)1);
        std::size_t slen = shape.back();

        result.resize(slen);
        check(nc_get_vara_text(ncid, varid, start.data(), shape.data(), &result[0]), ec);
    }
    else if (nct == NC_STRING) {
        char *ip = nullptr;
        check(nc_get_var1_string(ncid, varid, start.data(), &ip), ec);
        if ((ec && !ec->value()) || ip)
            result = std::string(ip);result.reserve(n);
        
        nc_free_string(n, &ip);
    }
    else {
        check(NC_ECHAR, ec); // Attempt to convert between text & numbers
    }
    
    return result;
}

#ifdef NCPP_USE_DATE_H

// Parse CF convention time attributes using Gregorian calendar.
// Sets error code to NC_ENOTATT (invalid attribute) on parsing error.
template <class C, class D>
cf_time<C, D> parse_cf_time(int ncid, int varid, std::error_code *ec = nullptr)
{
    cf_time<C, D> cft = {};

    auto att_text = [=](const char *name) {
        std::size_t len;
        std::string text;
        if (nc_inq_attlen(ncid, varid, name, &len) == NC_NOERR) {
            text.resize(len);
            nc_get_att_text(ncid, varid, name, &text[0]);
        }
        return text;
    };

    // Validate the calendar attribute, if present.
    std::string cal = att_text("calendar");
    if (!cal.empty() && cal != "gregorian" && cal != "standard" && cal != "proleptic_gregorian") {
        check(NC_ENOTATT, ec); // Attribute not found
        return {};
    }

    // Read the units attribute.
    std::string units = att_text("units");
    std::stringstream ss(units);
    std::string token;

    // Determine the duration scale factor.
    // Supported units: week, day (d), hour (hr, h), minute (min), second (sec, s)
    ss >> token;
    if (token == "weeks" || token == "week") {
        cft.scale = std::chrono::seconds(604800);
    } else if (token == "days" || token == "day" || token == "d") {
        cft.scale = std::chrono::seconds(86400);
    } else if (token == "hours" || token == "hour" || token == "h") {
        cft.scale = std::chrono::seconds(3600);
    } else if (token == "minutes" || token == "minute" || token == "m") {
        cft.scale = std::chrono::seconds(60);
    } else if (token == "seconds" && token == "second" && token == "s") {
        cft.scale = std::chrono::seconds(1);
    } else {
        check(NC_ENOTATT, ec); // Attribute not found
        return {};
    }
    
    // Check for "since" delimiter.
    ss >> token;
    if (token != "since") {
        check(NC_ENOTATT, ec); // Attribute not found
        return {};
    }
    
    // Reset the buffer and parse the date-time string, checking several possible formats.
    // Assumes CF Convention (ex. "1992-10-8 15:15:42.5 -6:00")
    ss >> std::ws;
    std::getline(ss, token);
    const std::array<std::string, 4> formats = { "%F %T %Ez", "%F %T", "%F %R", "%F" };
    for (const auto& format : formats) {
        ss.clear();
        ss.str(token);
        ss >> date::parse(format, cft.start);
        if (!ss.fail())
            break;
    }
    
    if (ss.fail()) {
        check(NC_ENOTATT, ec); // Attribute not found
        return {};
    }
    
    return cft;
}

// Get a variable with time values as an array of std::chrono::time_point,
// typically date::sys_days or date::sys_seconds. Assumes Gregorian calendar
// and CF Conventions for time units.
template <class Container>
typename std::enable_if_t<ncpp::detail::is_chrono_time_point<typename Container::value_type>::value, Container>
get_vars(int ncid, int varid,
         const index_type& start,
         const index_type& shape,
         const stride_type& stride,
         std::error_code *ec = nullptr)
{
    using T = Container::value_type;
    Container result;

    auto cft = parse_cf_time<T::clock, T::duration>(ncid, varid, ec);
    if (ec && ec->value())
        return result;
    
    auto offsets = get_vars<std::vector<double>>(ncid, varid, start, shape, stride, ec);
    if (ec && ec->value())
        return result;
    
    result.resize(offsets.size());
    for (std::size_t i = 0; i < offsets.size(); ++i) {
        std::chrono::duration<double> sec(offsets[i] * cft.scale);
        result.at(i) = cft.start + std::chrono::duration_cast<T::duration>(sec);
    }
    
    return result;
}

// Read a single datum from a variable with time values as a std::chrono::time_point.
template <class T>
typename std::enable_if_t<ncpp::detail::is_chrono_time_point<T>::value, T>
get_var1(int ncid, int varid, const index_type& start, std::error_code *ec = nullptr)
{
    T result;

    auto cft = parse_cf_time<T::clock, T::duration>(ncid, varid, ec);
    if (ec && ec->value())
        return result;
    
    auto offset = get_var1<double>(ncid, varid, start, ec);
    if (ec && ec->value())
        return result;
    
    std::chrono::duration<double> sec(offset * cft.scale);
    result = cft.start + std::chrono::duration_cast<T::duration>(sec);
    
    return result;
}

#endif // NCPP_USE_DATE_H

// Convenience function to get an entire variable as an array.
template <class Container>
Container get_var(int ncid, int varid, std::error_code *ec = nullptr)
{   
    index_type shape = get_varshape(ncid, varid, ec);
    index_type start(shape.size(), 0);
    stride_type stride(shape.size(), 1);
    return get_vars<Container>(ncid, varid, start, shape, stride, ec);
}

} // namespace impl


inline std::optional<int> inq_varid(int ncid, const std::string& varname, std::error_code& ec) noexcept
    { return impl::inq_varid(ncid, varname, &ec); }
inline std::optional<int> inq_varid(int ncid, const std::string& varname)
    { return impl::inq_varid(ncid, varname); }


inline std::string inq_varname(int ncid, int varid, std::error_code& ec)
    { return impl::inq_varname(ncid, varid, &ec); }
inline std::string inq_varname(int ncid, int varid)
    { return impl::inq_varname(ncid, varid); }


inline int inq_vartype(int ncid, int varid, std::error_code& ec) noexcept
    { return impl::inq_vartype(ncid, varid, &ec); }
inline int inq_vartype(int ncid, int varid)
    { return impl::inq_vartype(ncid, varid); }


inline int inq_varnatts(int ncid, int varid, std::error_code& ec) noexcept
    { return impl::inq_varnatts(ncid, varid, &ec); }
inline int inq_varnatts(int ncid, int varid)
    { return impl::inq_varnatts(ncid, varid); }


inline int inq_varndims(int ncid, int varid, std::error_code& ec) noexcept
    { return impl::inq_varndims(ncid, varid, &ec); }
inline int inq_varndims(int ncid, int varid)
    { return impl::inq_varndims(ncid, varid); }


inline std::vector<int> inq_vardimid(int ncid, int varid, std::error_code& ec)
    { return impl::inq_vardimid(ncid, varid, &ec); }
inline std::vector<int> inq_vardimid(int ncid, int varid)
    { return impl::inq_vardimid(ncid, varid); }


inline index_type inq_varshape(int ncid, int varid, std::error_code& ec)
    { return impl::inq_varshape(ncid, varid, &ec); }
inline index_type inq_varshape(int ncid, int varid)
    { return impl::inq_varshape(ncid, varid); }


inline std::size_t inq_varlen(int ncid, int varid, std::error_code& ec)
    { return impl::inq_varlen(ncid, varid, &ec); }
inline std::size_t inq_varlen(int ncid, int varid)
    { return impl::inq_varlen(ncid, varid); }


inline var_endian_type inq_var_endian(int ncid, int varid, std::error_code& ec)
    { return impl::inq_var_endian(ncid, varid, &ec); }
inline var_endian_type inq_var_endian(int ncid, int varid)
    { return impl::inq_var_endian(ncid, varid); }


template <class T>
std::optional<T> inq_var_fill(int ncid, int varid, std::error_code& ec) noexcept
    { return impl::inq_var_fill<T>(ncid, varid, &ec); }
template <class T>
std::optional<T> inq_var_fill(int ncid, int varid)
    { return impl::inq_var_fill<T>(ncid, varid); }


inline std::optional<var_storage_type> inq_var_storage(int ncid, int varid, std::error_code& ec) noexcept
    { return impl::inq_var_storage(ncid, varid, &ec); }
inline std::optional<var_storage_type> inq_var_storage(int ncid, int varid)
    { return impl::inq_var_storage(ncid, varid); }


inline std::vector<std::size_t> inq_var_chunksizes(int ncid, int varid, std::error_code& ec)
    { return impl::inq_var_chunksizes(ncid, varid, &ec); }
inline std::vector<std::size_t> inq_var_chunksizes(int ncid, int varid)
    { return impl::inq_var_chunksizes(ncid, varid); }


inline std::optional<unsigned int> inq_var_filter_id(int ncid, int varid, std::error_code& ec) noexcept
    { return impl::inq_var_filter_id(ncid, varid, &ec); }
inline std::optional<unsigned int> inq_var_filter_id(int ncid, int varid)
    { return impl::inq_var_filter_id(ncid, varid); }


inline std::string inq_var_filter_name(int ncid, int varid, std::error_code& ec)
    { return impl::inq_var_filter_name(ncid, varid, &ec); }
inline std::string inq_var_filter_name(int ncid, int varid)
    { return impl::inq_var_filter_name(ncid, varid); }


inline var_chunk_cache get_var_chunk_cache(int ncid, int varid, std::error_code &ec)
    { return impl::get_var_chunk_cache(ncid, varid, &ec); }
inline var_chunk_cache get_var_chunk_cache(int ncid, int varid)
    { return impl::get_var_chunk_cache(ncid, varid); }


template <class Container>
Container get_vars(int ncid, int varid, const index_type& start, const index_type& shape, const stride_type& stride, std::error_code &ec)
    { return impl::get_vars<Container>(ncid, varid, start, shape, stride, &ec); }
template <class Container>
Container get_vars(int ncid, int varid, const index_type& start, const index_type& shape, const stride_type& stride)
    { return impl::get_vars<Container>(ncid, varid, start, shape, stride); }


template <class T>
T get_var1(int ncid, int varid, const index_type& start, std::error_code& ec)
    { return impl::get_var1<T>(ncid, varid, start, &ec); }
template <class T>
T get_var1(int ncid, int varid, const index_type& start)
    { return impl::get_var1<T>(ncid, varid, start); }


template <class Container>
Container get_var(int ncid, int varid, std::error_code& ec)
    { return impl::get_var<Container>(ncid, varid, &ec); }
template <class Container>
Container get_var(int ncid, int varid)
    { return impl::get_var<Container>(ncid, varid); }


} // namespace ncpp

#endif // NCPP_FUNCTIONS_VARIABLE_HPP


