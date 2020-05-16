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

#include <ncpp/config.hpp>

#include <ncpp/dimension.hpp>
#include <ncpp/check.hpp>

#include <netcdf.h>
#include <string>
#include <vector>

namespace ncpp {

class dataset;
class variable;

/// netCDF dimension sequence container.
class dimensions_type
{
private:
    using storage_type = std::vector<ncpp::dimension>;
    using value_type = typename storage_type::value_type;

    int ncid_;
    int varid_;
    storage_type dims_;

    explicit dimensions_type(int ncid)
        : ncid_(ncid), varid_(-1)
    {
        std::vector<int> dimids;
        int ndims, ndims1;

        // Safely get dimids, in case dimensions are currently being added.
        // Based on netCDF-C implementation (dumplib.c).
        do {
            ncpp::check(nc_inq_dimids(ncid_, &ndims, nullptr, 0));
            dimids.resize(ndims + 1);
            ncpp::check(nc_inq_dimids(ncid_, &ndims1, dimids.data(), 0));
        } while (ndims != ndims1);
        
        dims_.reserve(ndims);
        for (int i = 0; i < ndims; ++i) {
            dims_.emplace_back(ncpp::dimension(ncid_, dimids.at(i)));
        }
    }

    dimensions_type(int ncid, int varid)
        : ncid_(ncid), varid_(varid)
    {
        std::vector<int> dimids;
        int ndims;

        ncpp::check(nc_inq_varndims(ncid_, varid_, &ndims));
        dimids.resize(ndims);
        ncpp::check(nc_inq_vardimid(ncid_, varid_, dimids.data()));

        dims_.reserve(ndims);
        for (int i = 0; i < ndims; ++i) {
            dims_.emplace_back(ncpp::dimension(ncid_, dimids.at(i)));
        }
    }
    
public:
    using const_iterator = storage_type::const_iterator;
    using const_reference = storage_type::const_reference;
    
    const_iterator begin() const noexcept {
        return dims_.begin();
    }

    const_iterator end() const noexcept {
        return dims_.end();
    }

    const_reference front() const noexcept {
        return *dims_.cbegin();
    }

    const_reference back() const noexcept {
        return *dims_.cend();
    }

    std::size_t size() const noexcept {
        return dims_.size();
    }

    /// Get a dimension from its name.
    const_reference operator[](const std::string& name) const
    {
        int dimid;
        ncpp::check(nc_inq_dimid(ncid_, name.c_str(), &dimid));

        const auto it = std::find(dims_.begin(), dims_.end(), ncpp::dimension(ncid_, dimid));
        if (it == dims_.end())
            ncpp::detail::throw_error(ncpp::error::invalid_dimension);

        return *it;
    }

    /// Get a dimension by index.
    const_reference at(std::size_t n) const
    {
        if (n >= dims_.size())
            ncpp::detail::throw_error(ncpp::error::invalid_dimension);

        auto it = dims_.begin();
        std::advance(it, n);
        return *it;
    }

    friend class dataset;
    friend class variable;
};

} // namespace ncpp

#endif // NCPP_DIMENSIONS_HPP