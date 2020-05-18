// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_FUNCTIONS_VERSION_HPP
#define NCPP_FUNCTIONS_VERSION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <netcdf.h>

#include <ncpp/config.hpp>

#include <string>

namespace ncpp {

inline std::string inq_libvers()
{
    return "netCDF " + std::string(nc_inq_libvers());
}

} // namespace ncpp

#endif // NCPP_FUNCTIONS_VERSION_HPP


