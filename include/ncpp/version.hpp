// Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_VERSION_HPP
#define NCPP_VERSION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <netcdf.h>

#include <string>

namespace ncpp {

/// Get the netCDF-C library version string.
inline std::string library_version()
{
    return "netCDF " + std::string(nc_inq_libvers());
}

} // namespace ncpp

#endif // NCPP_VERSION_HPP