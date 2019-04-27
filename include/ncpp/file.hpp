// Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_FILE_HPP
#define NCPP_FILE_HPP 

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <netcdf.h>

#include <ncpp/check.hpp>

#include <string>

namespace ncpp {

class dataset;

class file
{
private:
    int _ncid;

    /// Get the netCDF ID.
    int ncid() const {
        return _ncid;
    }

public:
    using openmode = int;

    static constexpr openmode read = NC_NOWRITE;
    static constexpr openmode write = NC_WRITE;
    static constexpr openmode append = NC_NETCDF4 | NC_NOCLOBBER;
    static constexpr openmode truncate = NC_NETCDF4 | NC_CLOBBER;

    file(const std::string& path, openmode mode = read)
    {
        int rc = NC_EINVAL;

        switch (mode) {
        case read:
            // File exists, open read-only.
            rc = nc_open(path.c_str(), NC_NOWRITE, &_ncid);
            break;
        case write:
            // File exists, open for writing.
            rc = nc_open(path.c_str(), NC_WRITE, &_ncid);
            break;
        case append:
            // Create new file, fail if already exists.
            rc = nc_create(path.c_str(), NC_NETCDF4 | NC_NOCLOBBER, &_ncid);
            break;
        case truncate:
            // Create new file, even if already exists.
            rc = nc_create(path.c_str(), NC_NETCDF4 | NC_CLOBBER, &_ncid);
            break;
        default:
            break;
        }

        ncpp::check(rc);
    }

    ~file() {
        nc_close(_ncid);
    }

    friend class dataset;
};

} // namespace ncpp

#endif // NCPP_FILE_HPP 