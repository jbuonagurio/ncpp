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

#include <ncpp/attribute.hpp>
#include <ncpp/check.hpp>

#include <set>
#include <string>

#include <netcdf.h>

namespace ncpp {

class dataset;
class variable;

class attributes_type
{
private:
    using storage_type = std::set<ncpp::attribute>;
    using value_type = typename storage_type::value_type;

    int _ncid;
    int _varid;
    storage_type _atts;

    attributes_type(int ncid, int varid = NC_GLOBAL)
        : _ncid(ncid), _varid(varid)
    {
        int natts;
        ncpp::check(nc_inq_varnatts(_ncid, _varid, &natts));
        
        for (int attnum = 0; attnum < natts; ++attnum) {
            char attname[NC_MAX_NAME + 1];
            ncpp::check(nc_inq_attname(_ncid, _varid, attnum, attname));
            _atts.emplace(ncpp::attribute(_ncid, _varid, attname));
        }
    }

public:
    using const_iterator = storage_type::const_iterator;
    using const_reference = storage_type::const_reference;
    
    const_iterator begin() const noexcept {
        return _atts.begin();
    }

    const_iterator end() const noexcept {
        return _atts.end();
    }

    const_reference front() const noexcept {
        return *_atts.cbegin();
    }

    const_reference back() const noexcept {
        return *_atts.cend();
    }

    std::size_t size() const noexcept {
        return _atts.size();
    }

    /// Get an attribute by name.
    const_reference operator[](const std::string& name) const
    {
        const auto it = _atts.find(ncpp::attribute(_ncid, _varid, name));
        if (it == _atts.end())
            ncpp::detail::throw_error(ncpp::error::attribute_not_found);

        return *it;
    }

    /// Determine if an attribute is present.
    bool contains(const std::string& name) const noexcept
    {
        std::size_t count = _atts.count(ncpp::attribute(_ncid, _varid, name));
        return (count > 0) ? true : false;
    }

    friend class dataset;
    friend class variable;
};

} // namespace ncpp

#endif // NCPP_ATTRIBUTES_HPP