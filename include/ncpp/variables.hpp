// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_VARIABLES_HPP
#define NCPP_VARIABLES_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <netcdf.h>

#include <ncpp/config.hpp>

#include <ncpp/variable.hpp>
#include <ncpp/check.hpp>

#include <iterator>
#include <set>
#include <string>
#include <vector>

namespace ncpp {

class dataset;

/// netCDF variable set.
class variables_type
{
    friend class dataset;

public:
    using storage_type = std::set<variable>;
    using value_type = typename storage_type::value_type;
    using const_iterator = storage_type::const_iterator;
    using const_reference = storage_type::const_reference;

    explicit variables_type(int ncid)
        : ncid_(ncid)
    {
        auto varids = inq_varids(ncid);
        for (const auto& varid : varids)
            vars_.emplace(variable(ncid, varid));
    }

    const_iterator begin() const noexcept {
        return vars_.begin();
    }

    const_iterator end() const noexcept {
        return vars_.end();
    }

    const_reference front() const noexcept {
        return *vars_.cbegin();
    }

    const_reference back() const noexcept {
        return *vars_.cend();
    }

    std::size_t size() const noexcept {
        return vars_.size();
    }

    bool empty() const noexcept {
        return vars_.empty();
    }

    /// Get a variable by name.
    const_reference operator[](const std::string& name) const
    {
        auto varid = inq_varid(ncid_, name);
        if (!varid.has_value())
            detail::throw_error(error::variable_not_found);
        
        const auto it = vars_.find(variable(ncid_, varid.value()));
        if (it == vars_.end())
            detail::throw_error(error::variable_not_found);
        
        return *it;
    }

    /// Get a variable by index.
    const_reference at(std::size_t n) const
    {
        if (n >= vars_.size())
            detail::throw_error(error::variable_not_found);

        return *std::next(vars_.begin(), n);
    }

    /// Determine if a variable is present.
    bool contains(const std::string& name) const noexcept
    {
        std::error_code ec;
        auto varid = inq_varid(ncid_, name, ec);
        if (ec.value() || !varid.has_value())
            return false;
        
        const auto it = std::find(vars_.begin(), vars_.end(), variable(ncid_, varid.value()));
        return (it != vars_.end());
    }

private:
    int ncid_;
    storage_type vars_;
};

} // namespace ncpp

#endif // NCPP_VARIABLES_HPP