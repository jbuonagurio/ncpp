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

#include <ncpp/config.hpp>

#include <ncpp/check.hpp>
#include <ncpp/dispatch.hpp>

#include <algorithm>
#include <string>
#include <vector>

namespace ncpp {

class dimensions_type;
class variable;

/// netCDF dimension type.
class dimension {
private:
    dimension(int ncid, int dimid)
        : _ncid(ncid), _dimid(dimid), _cvarid(-1)
    {
        // Get the coordinate variable id associated with the dimension, if any.
        // See also: iscoordvar function in netcdf-c/ncdump/dumplib.c

        int cvarid, cvartype, cvarndims;
        int cvardimids[2];

        if ((nc_inq_varid(_ncid, this->name().c_str(), &cvarid) != NC_NOERR)||
            (nc_inq_vartype(_ncid, cvarid, &cvartype) != NC_NOERR) ||
            (nc_inq_varndims(_ncid, cvarid, &cvarndims) != NC_NOERR))
            return;
        
        // Ensure the variable is one-dimensional; allow two dimensions for classic strings.
        if ((cvartype != NC_CHAR && cvarndims != 1) ||
            (cvartype == NC_CHAR && cvarndims > 2))
            return;

        // Ensure the variable is indexed by this dimension.
        if (nc_inq_vardimid(_ncid, cvarid, &cvardimids[0]) != NC_NOERR)
            return;
        
        if (cvardimids[0] != _dimid)
            return;
        
        _cvarid = cvarid;
    }

    int _ncid;
    int _dimid;
    int _cvarid;

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
    
    /// Returns true if the dimension is unlimited.
    bool is_unlimited() const
    {
        std::vector<int> unlim;
        int nunlim;
        ncpp::check(nc_inq_unlimdims(_ncid, &nunlim, nullptr));
        if (nunlim <= 0)
            return false;
        unlim.resize(nunlim);
        ncpp::check(nc_inq_unlimdims(_ncid, &nunlim, unlim.data()));
        auto it = std::find(unlim.begin(), unlim.end(), _dimid);
        return (it != unlim.end());
    }

    friend class dimensions_type;
    friend class variable;
};

} // namespace ncpp

#endif // NCPP_DIMENSION_HPP