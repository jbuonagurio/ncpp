// Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_ATTRIBUTES_HPP
#define NCPP_ATTRIBUTES_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ncpp/config.hpp>

#include <ncpp/attribute.hpp>
#include <ncpp/check.hpp>

#include <netcdf.h>
#include <set>
#include <string>

namespace ncpp {

class dataset;
class variable;

/// netCDF attribute set.
class attributes_type
{
private:
    using storage_type = std::set<ncpp::attribute>;
    using value_type = typename storage_type::value_type;

    int ncid_;
    int varid_;
    storage_type atts_;

    attributes_type(int ncid, int varid = NC_GLOBAL)
        : ncid_(ncid), varid_(varid)
    {
        int natts;
        ncpp::check(nc_inq_varnatts(ncid_, varid_, &natts));
        
        for (int attnum = 0; attnum < natts; ++attnum) {
            char attname[NC_MAX_NAME + 1];
            ncpp::check(nc_inq_attname(ncid_, varid_, attnum, attname));
            atts_.emplace(ncpp::attribute(ncid_, varid_, attname));
        }
    }

public:
    using const_iterator = storage_type::const_iterator;
    using const_reference = storage_type::const_reference;
    
    const_iterator begin() const noexcept {
        return atts_.begin();
    }

    const_iterator end() const noexcept {
        return atts_.end();
    }

    const_reference front() const noexcept {
        return *atts_.cbegin();
    }

    const_reference back() const noexcept {
        return *atts_.cend();
    }

    std::size_t size() const noexcept {
        return atts_.size();
    }

    /// Get an attribute by name.
    const_reference operator[](const std::string& name) const
    {
        const auto it = atts_.find(ncpp::attribute(ncid_, varid_, name));
        if (it == atts_.end())
            ncpp::detail::throw_error(ncpp::error::attribute_not_found);

        return *it;
    }

    /// Determine if an attribute is present.
    bool contains(const std::string& name) const noexcept
    {
        std::size_t count = atts_.count(ncpp::attribute(ncid_, varid_, name));
        return (count > 0) ? true : false;
    }

    friend class dataset;
    friend class variable;
};

} // namespace ncpp

#endif // NCPP_ATTRIBUTES_HPP