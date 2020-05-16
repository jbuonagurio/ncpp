// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_DETAIL_FUNCTIONS_HPP
#define NCPP_DETAIL_FUNCTIONS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ncpp/config.hpp>

#include <ncpp/check.hpp>
#include <ncpp/types.hpp>
#include <ncpp/detail/dispatch.hpp>

#include <netcdf.h>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace ncpp {
namespace detail {

// dataset:
//   nc_inq_format
//   nc_inq_dimids
//   nc_inq_unlimdims
//   nc_inq_varids
//   nc_inq_varid

inline int inq_format(int ncid, std::error_code *ec = nullptr)
{
    int format = 0;
    check(nc_inq_format(ncid, &format), ec);
    return format;
}

inline std::vector<int> inq_dimids(int ncid, std::error_code *ec = nullptr)
{
    std::vector<int> dimids;
    int ndims = 0, ndims1 = 0;

    // Safely get dimids, in case dimensions are currently being added.
    // Based on netCDF-C implementation (dumplib.c).
    do {
        check(nc_inq_dimids(ncid, &ndims, nullptr, 0), ec);
        dimids.resize(ndims + 1);
        check(nc_inq_dimids(ncid, &ndims1, dimids.data(), 0), ec);
    } while (ndims != ndims1);

    return dimids;
}

inline std::vector<int> inq_unlimdims(int ncid, std::error_code *ec = nullptr)
{
    int nunlimdims = 0;
    std::vector<int> unlimdims;
    check(nc_inq_unlimdims(ncid, &nunlimdims, nullptr), ec);
    if ((ec && !ec->value()) || nunlimdims > 0) {
        unlimdims.resize(static_cast<std::size_t>(nunlimdims));
        check(nc_inq_unlimdims(ncid, nullptr, unlimdims.data()), ec);
    }
    return unlimdims;
}

inline std::vector<int> inq_varids(int ncid, std::error_code *ec = nullptr)
{
    std::vector<int> varids;
    int nvars = 0, nvars1 = 0;

    // Safely get varids, in case variables are currently being added.
    // Based on netCDF-C implementation (dumplib.c).
    do {
        check(nc_inq_varids(ncid, &nvars, nullptr), ec);
        varids.resize(nvars + 1);
        check(nc_inq_varids(ncid, &nvars1, varids.data()), ec);
    } while (nvars != nvars1);

    return varids;
}

inline int inq_natts(int ncid, std::error_code *ec = nullptr)
{
    int natts = 0;
    check(nc_inq_natts(ncid, &natts), ec);
    return natts;
}

// dimension:
//   nc_inq_dimid
//   nc_inq_dimname
//   nc_inq_dimlen

inline std::optional<int> inq_dimid(int ncid, const std::string& dimname, std::error_code *ec = nullptr)
{
    int dimid;
    check(nc_inq_dimid(ncid, dimname.c_str(), &dimid), ec);
    if (ec && ec->value())
        return {};
    return dimid;
}

inline std::optional<std::string> inq_dimname(int ncid, int dimid, std::error_code *ec = nullptr)
{
    char dimname[NC_MAX_NAME + 1];
    check(nc_inq_dimname(ncid, dimid, dimname), ec);
    if (ec && ec->value())
        return {};
    return std::string(dimname);
}

inline std::size_t inq_dimlen(int ncid, int dimid, std::error_code *ec = nullptr)
{
    std::size_t dimlen;
    check(nc_inq_dimlen(ncid, dimid, &dimlen), ec);
    if (ec && ec->value())
        return 0;
    return dimlen;
}

// variable:
//   nc_inq_varid
//   nc_inq_varname
//   nc_inq_vartype
//   nc_inq_varnatts
//   nc_inq_varndims
//   nc_inq_vardimid
//   nc_inq_var_chunk
//   nc_inq_var_chunking
//   nc_inq_var_filter
//   nc_get_var_chunk_cache

inline std::optional<int> inq_varid(int ncid, const std::string& varname, std::error_code *ec = nullptr)
{
    int varid;
    check(nc_inq_varid(ncid, varname.c_str(), &varid), ec);
    if (ec && ec->value())
        return {};
    return varid;
}

inline std::optional<std::string> inq_varname(int ncid, int varid, std::error_code *ec = nullptr)
{
    char varname[NC_MAX_NAME + 1];
    check(nc_inq_varname(ncid, varid, varname), ec);
    if (ec && ec->value())
        return {};
    return std::string(varname);
}

inline int inq_vartype(int ncid, int varid, std::error_code *ec = nullptr)
{
    int vartype;
    check(nc_inq_vartype(ncid, varid, &vartype), ec);
    if (ec && ec->value())
        return NC_NAT;
    return vartype;
}

inline int inq_varnatts(int ncid, int varid, std::error_code *ec = nullptr)
{
    int natts;
    check(nc_inq_varnatts(ncid, varid, &natts), ec);
    if (ec && ec->value())
        return 0;
    return natts;
}

inline int inq_varndims(int ncid, int varid, std::error_code *ec = nullptr)
{
    int ndims = 0;
    check(nc_inq_varndims(ncid, varid, &ndims), ec);
    if (ec && ec->value())
        return 0;
    return ndims;
}

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

template <typename T>
inline std::optional<T> inq_var_fill(int ncid, int varid, std::error_code *ec = nullptr)
{
    int vartype = inq_vartype(ncid, varid, ec);
    if (vartype != type_to_netcdf_type<T>::type.value) {
        check(NC_EBADTYPE, ec);
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

inline std::optional<int> inq_var_chunking_storage(int ncid, int varid, std::error_code *ec = nullptr)
{
    int storage;
    check(nc_inq_var_chunking(ncid, varid, &storage, nullptr), ec);
    if (ec && ec->value())
        return {};
    return storage;
}

inline std::vector<std::size_t> inq_var_chunking_chunksizes(int ncid, int varid, std::error_code *ec = nullptr)
{
    std::vector<std::size_t> chunksizes;
    int ndims = inq_varndims(ncid, varid, ec);
    if ((ec && !ec->value()) || ndims > 0) {
        chunksizes.resize(ndims, 0);
        check(nc_inq_var_chunking(ncid, varid, nullptr, chunksizes.data()), ec);
    }
    return chunksizes;
}

inline std::optional<unsigned int> inq_var_filter_id(int ncid, int varid, std::error_code *ec = nullptr)
{
    unsigned int filterid;
    check(nc_inq_var_filter(ncid, varid, &filterid, nullptr, nullptr), ec);
    if (ec && ec->value())
        return {};
    return filterid;
}

inline std::optional<std::string> inq_var_filter_name(int ncid, int varid, std::error_code *ec = nullptr)
{
    unsigned int filterid;
    check(nc_inq_var_filter(ncid, varid, &filterid, nullptr, nullptr), ec);
    if (ec && ec->value())
        return {};
    
    // https://portal.hdfgroup.org/display/support/Filters
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

inline var_chunk_cache get_var_chunk_cache(int ncid, int varid, std::error_code *ec = nullptr)
{
    var_chunk_cache result = { 0 };
    check(nc_get_var_chunk_cache(ncid, varid, &result.size, &result.nelems, &result.preemption), ec);
    return result;
}

// attribute:
//   nc_inq_attid
//   nc_inq_attname
//   nc_inq_atttype
//   nc_inq_attlen

inline std::optional<int> inq_attid(int ncid, int varid, const std::string& attname, std::error_code *ec = nullptr)
{
    int attid;
    check(nc_inq_attid(ncid, varid, attname.c_str(), &attid), ec);
    if (ec && ec->value())
        return {};
    return attid;
}

inline std::optional<std::string> inq_attname(int ncid, int varid, int attnum, std::error_code *ec = nullptr)
{
    char attname[NC_MAX_NAME + 1];
    check(nc_inq_attname(ncid, varid, attnum, attname), ec);
    if (ec && ec->value())
        return {};
    return std::string(attname);
}

inline int inq_atttype(int ncid, int varid, const std::string& attname, std::error_code *ec = nullptr)
{
    int atttype;
    check(nc_inq_atttype(ncid, varid, attname.c_str(), &atttype), ec);
    if (ec && ec->value())
        return NC_NAT;
    return atttype;
}

inline std::size_t inq_attlen(int ncid, int varid, const std::string& attname, std::error_code *ec = nullptr)
{
    std::size_t attlen;
    check(nc_inq_attlen(ncid, varid, attname.c_str(), &attlen), ec);
    if (ec && ec->value())
        return 0;
    return attlen;
}

// libdispatch get_var wrapper for arithmetic types.

template <class Container>
typename std::enable_if<std::is_arithmetic<typename Container::value_type>::value, Container>::type
get_var(int ncid, int varid, std::error_code *ec = nullptr)
{
    Container result;
    auto dimids = inq_vardimid(ncid, varid, ec);
    if (dimids.size() == 0)
        return result;
    
    std::size_t size = 1;
    for (const auto& dimid : dimids) {
        std::size_t dimlen = inq_dimlen(ncid, dimid, ec);
        if (dimlen == 0)
            return result;
        size *= dimlen;
    }

    result.resize(size);
    check(get_var(ncid, varid, result.data()), ec);
    return result;
}

// libdispatch get_att wrapper for arithmetic types.

template <class Container>
typename std::enable_if_t<std::is_arithmetic_v<typename Container::value_type>, Container>
get_att(int ncid, int varid, const std::string& attname, std::error_code *ec = nullptr)
{
    Container result;
    std::size_t attlen = inq_attlen(ncid, varid, attname, ec);
    if (attlen == 0)
        return result;
    
    result.resize(attlen);
    check(get_att(ncid, attid, result.data()), ec);
    return result;
}

// libdispatch get_att wrapper for NC_CHAR strings.

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

// libdispatch get_att wrapper for NC_STRING strings.

template <class Vector>
typename std::enable_if_t<std::is_same_v<typename Vector::value_type, std::string>, Vector>
get_att(int ncid, int varid, const std::string& attname, std::error_code *ec = nullptr)
{
    Vector<std::string> result;

    std::size_t attlen = inq_attlen(ncid, varid, attname, ec);
    if (attlen == 0)
        return result;
    
    std::vector<char *> pv(attlen, nullptr);
    if (check(nc_get_att_string(ncid, varid, attname.c_str(), pv.data()), ec))
        return result;

    result.reserve(attlen);
    for (const auto p : pv) {
        if (p) result.emplace_back(std::string(p));
    }
    
    nc_free_string(attlen, pv.data());
    return result;
}

} // namespace detail
} // namespace ncpp

#endif // NCPP_DETAIL_FUNCTIONS_HPP


