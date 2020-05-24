// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_SELECTION_HPP
#define NCPP_SELECTION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ncpp/config.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace ncpp {

/// netCDF coordinate variable selection type.
template <class T>
struct selection
{
    std::string coordinate;
    T min_value;
    T max_value;
    std::ptrdiff_t stride = 1;
};

} // namespace ncpp

#endif // NCPP_SELECTION_HPP