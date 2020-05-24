// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
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

#include <ncpp/config.hpp>

#include <ncpp/functions/dataset.hpp>
#include <ncpp/functions/dimension.hpp>
#include <ncpp/functions/variable.hpp>
#include <ncpp/check.hpp>

#include <algorithm>
#include <string>
#include <vector>

namespace ncpp {

class dimensions_type;
class variable;

/// netCDF dimension type.
class dimension
{
    friend class dimensions_type;
    friend class variable;

public:
    dimension(int ncid, int dimid)
        : ncid_(ncid), dimid_(dimid), cvarid_(-1)
    {
        // Get the coordinate variable id associated with the dimension, if any.
        // See also: iscoordvar function in netcdf-c/ncdump/dumplib.c

        int cvarid = api::inq_varid(ncid, name()).value_or(-1);
        if (cvarid == -1)
            return;
        
        int cvartype = api::inq_vartype(ncid, cvarid);
        auto cvardimids = api::inq_vardimid(ncid, cvarid);

        // Ensure the variable is one-dimensional; allow two dimensions for classic strings.
        if ((cvartype != NC_CHAR && cvardimids.size() != 1) ||
            (cvartype == NC_CHAR && cvardimids.size() > 2))
            return;

        // Ensure the variable is indexed by this dimension.
        if (cvardimids.at(0) != dimid)
            return;
        
        cvarid_ = cvarid;
    }

    bool operator<(const dimension& rhs) const {
        return (dimid_ < rhs.dimid_);
    }

    bool operator==(const dimension& rhs) const {
        return (ncid_ == rhs.ncid_ && dimid_ == rhs.dimid_);
    }

    bool operator!=(const dimension& rhs) const {
        return !(*this == rhs);
    }

    /// Get the dimension name.
    std::string name() const
    {
        return api::inq_dimname(ncid_, dimid_);
    }

    /// Get the netCDF ID.
    int ncid() const {
        return ncid_;
    }

    /// Get the dimension ID.
    int dimid() const
    {
        return dimid_;
    }

    /// Get the dimension length.
    std::size_t length() const
    {
        return api::inq_dimlen(ncid_, dimid_);
    }
    
    /// Returns true if the dimension is unlimited.
    bool is_unlimited() const
    {
        std::vector<int> unlim = api::inq_unlimdims(ncid_);
        auto it = std::find(unlim.begin(), unlim.end(), dimid_);
        return (it != unlim.end());
    }

private:
    int ncid_;
    int dimid_;
    int cvarid_;
};

} // namespace ncpp

#endif // NCPP_DIMENSION_HPP