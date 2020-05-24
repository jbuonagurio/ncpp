// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_FUNCTIONS_GLOBAL_HPP
#define NCPP_FUNCTIONS_GLOBAL_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <netcdf.h>

#include <ncpp/config.hpp>

#include <ncpp/types.hpp>
#include <ncpp/check.hpp>

#include <string>
#include <system_error>

namespace ncpp {
namespace api {
namespace impl {

inline chunk_cache get_chunk_cache(std::error_code *ec = nullptr)
{
    chunk_cache result = { 0 };
    check(nc_get_chunk_cache(&result.size, &result.nelems, &result.preemption), ec);
    return result;
}

} // namespace impl


// Get the netCDF library version string.
inline std::string inq_libvers()
{
    return "netCDF " + std::string(nc_inq_libvers());
}


inline chunk_cache get_chunk_cache(std::error_code &ec)
    { return impl::get_chunk_cache(&ec); }
inline chunk_cache get_chunk_cache()
    { return impl::get_chunk_cache(); }


} // namespace api
} // namespace ncpp

#endif // NCPP_FUNCTIONS_GLOBAL_HPP


