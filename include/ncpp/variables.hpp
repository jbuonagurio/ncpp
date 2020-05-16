// Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_VARIABLES_HPP
#define NCPP_VARIABLES_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ncpp/config.hpp>

#include <ncpp/variable.hpp>
#include <ncpp/check.hpp>

#include <netcdf.h>
#include <iterator>
#include <set>
#include <string>
#include <vector>

namespace ncpp {

class dataset;

/// netCDF variable set.
class variables_type
{
private:
    using storage_type = std::set<ncpp::variable>;
    using value_type = typename storage_type::value_type;

    int _ncid;
    storage_type _vars;

    explicit variables_type(int ncid)
        : _ncid(ncid)
    {
        std::vector<int> varids;
        int nvars, nvars1;

        // Safely get varids, in case variables are currently being added.
        // Based on netCDF-C implementation (dumplib.c).
        do {
            ncpp::check(nc_inq_varids(_ncid, &nvars, nullptr));
            varids.resize(nvars + 1);
            ncpp::check(nc_inq_varids(_ncid, &nvars1, varids.data()));
        } while (nvars != nvars1);

        for (int i = 0; i < nvars; ++i) {
            _vars.emplace(ncpp::variable(_ncid, varids.at(i)));
        }
    }

public:
    using const_iterator = storage_type::const_iterator;
    using const_reference = storage_type::const_reference;

    const_iterator begin() const noexcept {
        return _vars.begin();
    }

    const_iterator end() const noexcept {
        return _vars.end();
    }

    const_reference front() const noexcept {
        return *_vars.cbegin();
    }

    const_reference back() const noexcept {
        return *_vars.cend();
    }

    std::size_t size() const noexcept {
        return _vars.size();
    }

    /// Get a variable by name.
    const_reference operator[](const std::string& name) const
    {
        int varid;
        ncpp::check(nc_inq_varid(_ncid, name.c_str(), &varid));
        
        const auto it = _vars.find(ncpp::variable(_ncid, varid));
        if (it == _vars.end())
            ncpp::detail::throw_error(ncpp::error::variable_not_found);
        
        return *it;
    }

    /// Get a variable by index.
    const_reference at(std::size_t n) const
    {
        if (n >= _vars.size())
            ncpp::detail::throw_error(ncpp::error::variable_not_found);

        return *std::next(_vars.begin(), n);
    }

    /// Determine if a variable is present.
    bool contains(const std::string& name) const noexcept
    {
        int varid;
        int rc = nc_inq_varid(_ncid, name.c_str(), &varid);
        if (rc == NC_NOERR && _vars.count(ncpp::variable(_ncid, varid)) > 0)
            return true;
        else
            return false;
    }

    friend class dataset;
};

} // namespace ncpp

#endif // NCPP_VARIABLES_HPP