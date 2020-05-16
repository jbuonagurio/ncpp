// Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_VARIANT_HPP
#define NCPP_VARIANT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ncpp/config.hpp>
#include <ncpp/error.hpp>

#include <netcdf.h>
#include <string>
#include <variant>
#include <vector>

namespace ncpp {

// Variant for attribute values.
using variant = std::variant<
    std::vector<signed char>,
    std::string,
    std::vector<short>,
    std::vector<int>,
    std::vector<float>,
    std::vector<double>,
    std::vector<unsigned char>,
    std::vector<unsigned short>,
    std::vector<unsigned int>,
    std::vector<long long>,
    std::vector<unsigned long long>,
    std::vector<std::string>
>;

// The variant index follows the order of the netCDF data type IDs.
static_assert(std::is_same_v<std::variant_alternative_t<NC_BYTE   - 1, variant>, std::vector<signed char>>);
static_assert(std::is_same_v<std::variant_alternative_t<NC_CHAR   - 1, variant>, std::string>);
static_assert(std::is_same_v<std::variant_alternative_t<NC_SHORT  - 1, variant>, std::vector<short>>);
static_assert(std::is_same_v<std::variant_alternative_t<NC_INT    - 1, variant>, std::vector<int>>);
static_assert(std::is_same_v<std::variant_alternative_t<NC_FLOAT  - 1, variant>, std::vector<float>>);
static_assert(std::is_same_v<std::variant_alternative_t<NC_DOUBLE - 1, variant>, std::vector<double>>);
static_assert(std::is_same_v<std::variant_alternative_t<NC_UBYTE  - 1, variant>, std::vector<unsigned char>>);
static_assert(std::is_same_v<std::variant_alternative_t<NC_USHORT - 1, variant>, std::vector<unsigned short>>);
static_assert(std::is_same_v<std::variant_alternative_t<NC_UINT   - 1, variant>, std::vector<unsigned int>>);
static_assert(std::is_same_v<std::variant_alternative_t<NC_INT64  - 1, variant>, std::vector<long long>>);
static_assert(std::is_same_v<std::variant_alternative_t<NC_UINT64 - 1, variant>, std::vector<unsigned long long>>);
static_assert(std::is_same_v<std::variant_alternative_t<NC_STRING - 1, variant>, std::vector<std::string>>);

} // namespace ncpp

#endif // NCPP_VARIANT_HPP