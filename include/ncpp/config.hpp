// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef NCPP_CONFIG_HPP
#define NCPP_CONFIG_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Default buffer size for block iterator in bytes.
// nccopy default: 5000000 (~5 MB); ncgen default: 0x40000 (256 kB).
#ifndef NCPP_DEFAULT_BUFFER_SIZE
#define NCPP_DEFAULT_BUFFER_SIZE 5000000
#endif

//#define NCPP_NO_EXCEPTIONS
//#define NCPP_USE_BOOST
//#define NCPP_USE_DATE_H

#endif // NCPP_CONFIG_HPP