// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
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

#include <ncpp/config.hpp>

#include <ncpp/functions/dimension.hpp>
#include <ncpp/functions/variable.hpp>
#include <ncpp/dimension.hpp>
#include <ncpp/check.hpp>

#include <string>
#include <vector>

namespace ncpp {

class dataset;
class variable;

/// netCDF dimension sequence container.
class dimensions_type
{
    friend class dataset;
    friend class variable;

public:
    using storage_type = std::vector<dimension>;
    using value_type = typename storage_type::value_type;
    using iterator = storage_type::iterator;
    using const_iterator = storage_type::const_iterator;
    using reference = storage_type::reference;
    using const_reference = storage_type::const_reference;
    
    explicit dimensions_type(int ncid)
        : ncid_(ncid), varid_(NC_GLOBAL)
    {
        auto dimids = api::inq_dimids(ncid);
        dims_.reserve(dimids.size());
        for (const auto& dimid : dimids)
            dims_.emplace_back(dimension(ncid, dimid));
    }

    dimensions_type(int ncid, int varid)
        : ncid_(ncid), varid_(varid)
    {
        auto dimids = api::inq_vardimid(ncid, varid);
        dims_.reserve(dimids.size());
        for (const auto& dimid : dimids)
            dims_.emplace_back(dimension(ncid, dimid));
    }

    iterator begin() noexcept {
        return dims_.begin();
    }

    iterator end() noexcept {
        return dims_.end();
    }
    
    const_iterator begin() const noexcept {
        return dims_.begin();
    }

    const_iterator end() const noexcept {
        return dims_.end();
    }

    const_reference front() const {
        return *dims_.cbegin();
    }

    const_reference back() const {
        return *dims_.cend();
    }

    std::size_t size() const noexcept {
        return dims_.size();
    }

    bool empty() const noexcept {
        return dims_.empty();
    }

    /// Get a dimension from its name.
    const_reference operator[](const std::string& name) const
    {
        auto dimid = api::inq_dimid(ncid_, name);
        if (!dimid.has_value())
            detail::throw_error(error::invalid_dimension);
        
        const auto it = std::find(dims_.begin(), dims_.end(), dimension(ncid_, dimid.value()));
        if (it == dims_.end())
            detail::throw_error(error::invalid_dimension);

        return *it;
    }

    /// Get a dimension by index.
    const_reference at(std::size_t n) const
    {
        if (n >= dims_.size())
            detail::throw_error(error::invalid_dimension);

        auto it = dims_.begin();
        std::advance(it, n);
        return *it;
    }

    /// Determine if a dimension is present.
    bool contains(const std::string& name) const noexcept
    {
        std::error_code ec;
        auto dimid = api::inq_dimid(ncid_, name, ec);
        if (ec.value() || !dimid.has_value())
            return false;
        
        const auto it = std::find(dims_.begin(), dims_.end(), dimension(ncid_, dimid.value()));
        return (it != dims_.end());
    }

private:
    int ncid_;
    int varid_;
    storage_type dims_;
};

} // namespace ncpp

#endif // NCPP_DIMENSIONS_HPP