// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_ATTRIBUTES_HPP
#define NCPP_ATTRIBUTES_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <netcdf.h>

#include <ncpp/config.hpp>

#include <ncpp/functions/variable.hpp>
#include <ncpp/functions/attribute.hpp>
#include <ncpp/attribute.hpp>
#include <ncpp/check.hpp>

#include <set>
#include <string>

namespace ncpp {

class dataset;
class variable;

/// netCDF attribute set.
class attributes_type
{
    friend class dataset;
    friend class variable;

public:
    using storage_type = std::set<attribute>;
    using value_type = typename storage_type::value_type;
    using iterator = storage_type::iterator;
    using const_iterator = storage_type::const_iterator;
    using reference = storage_type::reference;
    using const_reference = storage_type::const_reference;

    attributes_type(int ncid, int varid = NC_GLOBAL)
        : ncid_(ncid), varid_(varid)
    {
        int natts = api::inq_varnatts(ncid, varid);
        for (int attnum = 0; attnum < natts; ++attnum) {
            std::string attname = api::inq_attname(ncid, varid, attnum);
            atts_.emplace(attribute(ncid, varid, attname));
        }
    }
    
    iterator begin() noexcept {
        return atts_.begin();
    }

    iterator end() noexcept {
        return atts_.end();
    }

    const_iterator begin() const noexcept {
        return atts_.begin();
    }

    const_iterator end() const noexcept {
        return atts_.end();
    }
    
    const_reference front() const {
        return *atts_.cbegin();
    }

    const_reference back() const {
        return *atts_.cend();
    }

    std::size_t size() const noexcept {
        return atts_.size();
    }

    bool empty() const noexcept {
        return atts_.empty();
    }

    /// Get an attribute by name.
    const_reference operator[](const std::string& name) const
    {
        const auto it = atts_.find(attribute(ncid_, varid_, name));
        if (it == atts_.end())
            detail::throw_error(error::attribute_not_found);

        return *it;
    }

    /// Get an attribute by index.
    const_reference at(std::size_t n) const
    {
        if (n >= atts_.size())
            detail::throw_error(error::attribute_not_found);

        return *std::next(atts_.begin(), n);
    }

    /// Determine if an attribute is present.
    bool contains(const std::string& name) const noexcept
    {
        const auto it = std::find(atts_.begin(), atts_.end(), attribute(ncid_, varid_, name));
        return (it != atts_.end());
    }

private:
    int ncid_;
    int varid_;
    storage_type atts_;
};

} // namespace ncpp

#endif // NCPP_ATTRIBUTES_HPP