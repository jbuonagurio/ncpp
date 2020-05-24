// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_HPP
#define NCPP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <netcdf.h>

#include <ncpp/config.hpp>

#include <ncpp/error.hpp>
#include <ncpp/file.hpp>
#include <ncpp/dataset.hpp>
#include <ncpp/dimensions.hpp>
#include <ncpp/variables.hpp>
#include <ncpp/attributes.hpp>
#include <ncpp/iterator.hpp>

#include <ncpp/functions/attribute.hpp>
#include <ncpp/functions/dataset.hpp>
#include <ncpp/functions/dimension.hpp>
#include <ncpp/functions/variable.hpp>
#include <ncpp/functions/global.hpp>

#endif // NCPP_HPP 