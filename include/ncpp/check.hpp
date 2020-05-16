// Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_CHECK_HPP
#define NCPP_CHECK_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ncpp/config.hpp>

#include <ncpp/error.hpp>

namespace ncpp {

/// Utility function to check netCDF error codes.
inline void check(int rc, std::error_code *ec = nullptr)
{
    if (rc != NC_NOERR) {
        if (ec)
            *ec = ncpp::error::make_error_code(rc);
        else
            ncpp::detail::throw_error(rc);
    }
    else {
        if (ec)
            ec->clear();
    }
}

} // namespace ncpp

#endif // NCPP_CHECK_HPP