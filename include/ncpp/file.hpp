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

#include <ncpp/config.hpp>

#include <ncpp/check.hpp>

#include <string>

namespace ncpp {

class dataset;

/// netCDF file type.
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
    
    /// Returns true if this is a netCDF-4 file.
    bool is_netcdf4() const noexcept
    {
        int value;
        if (nc_get_att_int(_ncid, NC_GLOBAL, "_IsNetcdf4", &value) != NC_NOERR)
            return false;
        return value > 0;
    }
    
    /// Returns the netCDF-4 internal _NCProperties attribute or empty string if undefined.
    std::string properties() const noexcept
    {
        std::size_t len;
        if (nc_inq_att(_ncid, NC_GLOBAL, "_NCProperties", nullptr, &len) != NC_NOERR)
            return {};
        
        std::string value;
        value.resize(len);
        if (nc_get_att_text(_ncid, NC_GLOBAL, "_NCProperties", &value[0]) != NC_NOERR)
            return {};
        
        return value;
    }
    
    /// Returns the netCDF-4 internal _SuperblockVersion attribute or -1 if undefined.
    int superblock_version() const noexcept
    {
        int value;
        if (nc_get_att_int(_ncid, NC_GLOBAL, "_SuperblockVersion", &value) != NC_NOERR)
            return -1;
        return value;
    }

    friend class dataset;
};

} // namespace ncpp

#endif // NCPP_FILE_HPP 