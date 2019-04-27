// Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_DIMENSIONS_HPP
#define NCPP_DIMENSIONS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <netcdf.h>

#include <ncpp/dimension.hpp>
#include <ncpp/check.hpp>

#include <string>
#include <vector>

namespace ncpp {

class dataset;
class variable;

class dimensions_type
{
private:
    using storage_type = std::vector<ncpp::dimension>;
    using value_type = typename storage_type::value_type;

    int _ncid;
    int _varid;
    storage_type _dims;

    explicit dimensions_type(int ncid)
        : _ncid(ncid), _varid(-1)
    {
        std::vector<int> dimids;
        int ndims, ndims1;

        // Safely get dimids, in case dimensions are currently being added.
        // Based on netCDF-C implementation (dumplib.c).
        do {
            ncpp::check(nc_inq_dimids(_ncid, &ndims, nullptr, 0));
            dimids.resize(ndims + 1);
            ncpp::check(nc_inq_dimids(_ncid, &ndims1, dimids.data(), 0));
        } while (ndims != ndims1);
        
        _dims.reserve(ndims);
        for (int i = 0; i < ndims; ++i) {
            _dims.emplace_back(ncpp::dimension(_ncid, dimids.at(i)));
        }
    }

    dimensions_type(int ncid, int varid)
        : _ncid(ncid), _varid(varid)
    {
        std::vector<int> dimids;
        int ndims;

        ncpp::check(nc_inq_varndims(_ncid, _varid, &ndims));
        dimids.resize(ndims);
        ncpp::check(nc_inq_vardimid(_ncid, _varid, dimids.data()));

        _dims.reserve(ndims);
        for (int i = 0; i < ndims; ++i) {
            _dims.emplace_back(ncpp::dimension(_ncid, dimids.at(i)));
        }
    }
    
public:
    using const_iterator = storage_type::const_iterator;
    using const_reference = storage_type::const_reference;
    
    const_iterator begin() const noexcept {
        return _dims.begin();
    }

    const_iterator end() const noexcept {
        return _dims.end();
    }

    const_reference front() const noexcept {
        return *_dims.cbegin();
    }

    const_reference back() const noexcept {
        return *_dims.cend();
    }

    std::size_t size() const noexcept {
        return _dims.size();
    }

    /// Get a dimension from its name.
    const_reference operator[](const std::string& name) const
    {
        int dimid;
        ncpp::check(nc_inq_dimid(_ncid, name.c_str(), &dimid));

        const auto it = std::find(_dims.begin(), _dims.end(), ncpp::dimension(_ncid, dimid));
        if (it == _dims.end())
            ncpp::detail::throw_error(ncpp::error::invalid_dimension);

        return *it;
    }

    /// Get a dimension by index.
    const_reference at(std::size_t n) const
    {
        if (n >= _dims.size())
            ncpp::detail::throw_error(ncpp::error::invalid_dimension);

        auto it = _dims.begin();
        std::advance(it, n);
        return *it;
    }

    friend class dataset;
    friend class variable;
};

} // namespace ncpp

#endif // NCPP_DIMENSIONS_HPP