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

#ifdef NCPP_USE_VARIANT

#include <netcdf.h>

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace ncpp {

// Runtime netCDF type to type mapping. Requires std::variant (C++17).

using variant = std::variant<
    std::string,
    std::vector<signed char>,
    std::vector<unsigned char>,
    std::vector<short>,
    std::vector<int>,
    std::vector<float>,
    std::vector<double>,
    std::vector<unsigned short>,
    std::vector<unsigned int>,
    std::vector<long long>,
    std::vector<unsigned long long>,
    std::vector<std::string>
>;

static inline auto nctype_to_variant = std::unordered_map<int, variant>{
    { NC_CHAR,   std::string()                     },
    { NC_BYTE,   std::vector<signed char>()        },
    { NC_UBYTE,  std::vector<unsigned char>()      },
    { NC_SHORT,  std::vector<short>()              },
    { NC_INT,    std::vector<int>()                },
    { NC_FLOAT,  std::vector<float>()              },
    { NC_DOUBLE, std::vector<double>()             },
    { NC_USHORT, std::vector<unsigned short>()     },
    { NC_UINT,   std::vector<unsigned int>()       },
    { NC_INT64,  std::vector<long long>()          },
    { NC_UINT64, std::vector<unsigned long long>() },
    { NC_STRING, std::vector<std::string>()        }
};

} // namespace ncpp

#endif // NCPP_USE_VARIANT
#endif // NCPP_VARIANT_HPP