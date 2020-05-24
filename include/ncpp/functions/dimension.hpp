// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_FUNCTIONS_DIMENSION_HPP
#define NCPP_FUNCTIONS_DIMENSION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <netcdf.h>

#include <ncpp/config.hpp>
#include <ncpp/check.hpp>

#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <system_error>
#include <vector>

namespace ncpp {
namespace impl {

// Get the ID of a dimension.
inline std::optional<int> inq_dimid(int ncid, const std::string& dimname, std::error_code *ec = nullptr)
{
    int dimid;
    check(nc_inq_dimid(ncid, dimname.c_str(), &dimid), ec);
    if (ec && ec->value())
        return {};
    return dimid;
}

// Get the name of a dimension.
inline std::string inq_dimname(int ncid, int dimid, std::error_code *ec = nullptr)
{
    char dimname[NC_MAX_NAME + 1];
    std::fill(std::begin(dimname), std::end(dimname), '\0');
    check(nc_inq_dimname(ncid, dimid, dimname), ec);
    return (ec && ec->value()) ? std::string() : std::string(dimname);
}

// Get the length of a dimension.
inline std::size_t inq_dimlen(int ncid, int dimid, std::error_code *ec = nullptr)
{
    std::size_t dimlen;
    check(nc_inq_dimlen(ncid, dimid, &dimlen), ec);
    if (ec && ec->value())
        return 0;
    return dimlen;
}

} // namespace impl


inline std::optional<int> inq_dimid(int ncid, const std::string& dimname, std::error_code& ec) noexcept
    { return impl::inq_dimid(ncid, dimname, &ec); }
inline std::optional<int> inq_dimid(int ncid, const std::string& dimname)
    { return impl::inq_dimid(ncid, dimname); }


inline std::string inq_dimname(int ncid, int dimid, std::error_code& ec)
    { return impl::inq_dimname(ncid, dimid, &ec); }
inline std::string inq_dimname(int ncid, int dimid)
    { return impl::inq_dimname(ncid, dimid); }


inline std::size_t inq_dimlen(int ncid, int dimid, std::error_code& ec) noexcept
    { return impl::inq_dimlen(ncid, dimid, &ec); }
inline std::size_t inq_dimlen(int ncid, int dimid)
    { return impl::inq_dimlen(ncid, dimid); }


} // namespace ncpp

#endif // NCPP_FUNCTIONS_DIMENSION_HPP


