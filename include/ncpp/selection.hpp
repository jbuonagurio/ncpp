// Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_SELECTION_HPP
#define NCPP_SELECTION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>

namespace ncpp {

template <typename T>
struct selection
{   
    std::string dimension_name;
    T min_coordinate;
    T max_coordinate;
};

} // namespace ncpp

#endif // NCPP_SELECTION_HPP