// Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_DIMENSION_HPP
#define NCPP_DIMENSION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <netcdf.h>

#include <ncpp/check.hpp>
#include <ncpp/dispatch.hpp>

#include <string>
#include <vector>

namespace ncpp {

class dimensions_type;

class dimension {
private:
    dimension(int ncid, int dimid)
        : _ncid(ncid), _dimid(dimid) {}

    int _ncid;
    int _dimid;

public:
    bool operator<(const dimension& rhs) const {
        return (_dimid < rhs._dimid);
    }

    bool operator==(const dimension& rhs) const {
        return (_ncid == rhs._ncid && _dimid == rhs._dimid);
    }

    bool operator!=(const dimension& rhs) const {
        return !(*this == rhs);
    }

    /// Get the dimension name.
    std::string name() const
    {
        char dimname[NC_MAX_NAME + 1];
        ncpp::check(nc_inq_dimname(_ncid, _dimid, dimname));
        return std::string(dimname);
    }

    /// Get the dimension length.
    std::size_t length() const
    {
        std::size_t dimlen;
        ncpp::check(nc_inq_dimlen(_ncid, _dimid, &dimlen));
        return dimlen;
    }

    /// Get the coordinate variable id associated with the dimension, if any.
    /// See also: iscoordvar function in netcdf-c/ncdump/dumplib.c
    int coordvarid() const noexcept
    {
        // Check for a variable with the same name as this dimension.
        int cvarid;
        if (nc_inq_varid(_ncid, this->name().c_str(), &cvarid) != NC_NOERR)
            return -1;

        // Ensure the variable is one-dimensional.
        int cvarndims;
        if (nc_inq_varndims(_ncid, cvarid, &cvarndims) != NC_NOERR)
            return -1;
        
        if (cvarndims != 1)
            return -1;

        // Ensure the variable is indexed by this dimension.
        int cvardimid;
        if (nc_inq_vardimid(_ncid, cvarid, &cvardimid) != NC_NOERR)
            return -1;

        if (cvardimid != _dimid)
            return -1;
        
        return cvarid;
    }

    friend class dimensions_type;
};

} // namespace ncpp

#endif // NCPP_DIMENSION_HPP