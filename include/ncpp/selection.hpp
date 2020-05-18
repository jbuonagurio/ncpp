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

struct strided_slice
{
    std::vector<std::size_t> start_;
    std::vector<std::size_t> shape_;
    std::vector<std::ptrdiff_t> stride_;
};

/// netCDF coordinate variable selection type.
template <class T>
struct selection
{
    std::string dimension_name;
    T min_coordinate;
    T max_coordinate;
    std::ptrdiff_t stride = 1;
};

} // namespace ncpp

#endif // NCPP_SELECTION_HPP