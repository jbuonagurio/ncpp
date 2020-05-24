// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_FUNCTIONS_DATASET_HPP
#define NCPP_FUNCTIONS_DATASET_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <netcdf.h>

#include <ncpp/config.hpp>

#include <ncpp/check.hpp>

#include <algorithm>
#include <cstddef>
#include <string>
#include <system_error>
#include <vector>

namespace ncpp {
namespace impl {

// Get the format flags associated with a dataset or group.
inline int inq_format(int ncid, std::error_code *ec = nullptr)
{
    int flags = 0;
    check(nc_inq_format(ncid, &flags), ec);
    return flags;
}

inline std::size_t inq_type_size(int ncid, int nctype, std::error_code *ec = nullptr)
{
    std::size_t size = 0;
    check(nc_inq_type(ncid, nctype, nullptr, &size));
    return size;
}

inline std::string inq_type_name(int ncid, int nctype, std::error_code *ec = nullptr)
{
    char name[NC_MAX_NAME + 1];
    std::fill(std::begin(name), std::end(name), '\0');
    check(nc_inq_type(ncid, nctype, name, nullptr), ec);
    return (ec && ec->value()) ? std::string() : std::string(name);
}

// Get the dimension IDs associated with a dataset or group.
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

// Get the unlimited dimension IDs associated with a dataset or group.
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

// Get the variable IDs associated with a dataset or group.
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

// Get the number of attributes associated with a dataset or group.
inline int inq_natts(int ncid, std::error_code *ec = nullptr)
{
    int natts = 0;
    check(nc_inq_natts(ncid, &natts), ec);
    return natts;
}

} // namespace impl

inline int inq_format(int ncid, std::error_code& ec) noexcept
    { return impl::inq_format(ncid, &ec); }
inline int inq_format(int ncid)
    { return impl::inq_format(ncid); }


inline std::size_t inq_type_size(int ncid, int nctype, std::error_code &ec) noexcept
    { return impl::inq_type_size(ncid, nctype, &ec); }
inline std::size_t inq_type_size(int ncid, int nctype)
    { return impl::inq_type_size(ncid, nctype); }


inline std::string inq_type_name(int ncid, int nctype, std::error_code& ec)
    { return impl::inq_type_name(ncid, nctype, &ec); }
inline std::string inq_type_name(int ncid, int nctype)
    { return impl::inq_type_name(ncid, nctype); }


inline std::vector<int> inq_dimids(int ncid, std::error_code& ec)
    { return impl::inq_dimids(ncid, &ec); }
inline std::vector<int> inq_dimids(int ncid)
    { return impl::inq_dimids(ncid); }


inline std::vector<int> inq_unlimdims(int ncid, std::error_code& ec)
    { return impl::inq_unlimdims(ncid, &ec); }
inline std::vector<int> inq_unlimdims(int ncid)
    { return impl::inq_unlimdims(ncid); }


inline std::vector<int> inq_varids(int ncid, std::error_code& ec)
    { return impl::inq_varids(ncid, &ec); }
inline std::vector<int> inq_varids(int ncid)
    { return impl::inq_varids(ncid); }


inline int inq_natts(int ncid, std::error_code& ec) noexcept
    { return impl::inq_natts(ncid, &ec); }
inline int inq_natts(int ncid)
    { return impl::inq_natts(ncid); }

} // namespace ncpp

#endif // NCPP_FUNCTIONS_DATASET_HPP


