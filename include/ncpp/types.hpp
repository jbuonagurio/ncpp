// Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_TYPES_HPP
#define NCPP_TYPES_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <netcdf.h>

#include <string>
#include <type_traits>

namespace ncpp {

static inline const char* nctype_name(int nctype)
{
    switch (nctype) {
        case NC_NAT:      return "nat";
        case NC_BYTE:     return "byte";
        case NC_CHAR:     return "char";
        case NC_SHORT:    return "short";
        case NC_INT:      return "int";
        case NC_FLOAT:    return "float";
        case NC_DOUBLE:   return "double";
        case NC_UBYTE:    return "ubyte";
        case NC_USHORT:   return "ushort";
        case NC_UINT:     return "uint";
        case NC_INT64:    return "int64";
        case NC_UINT64:   return "uint64";
        case NC_STRING:   return "string";
        case NC_VLEN:     return "vlen";
        case NC_OPAQUE:   return "opaque";
        case NC_ENUM:     return "enum";
        case NC_COMPOUND: return "compound";
        default:          return "nat";
    }
}

/**
 * Map from netCDF type to value type.
 * Example: typeid(nctype_to_type<NC_INT>::type).name()
 */

template<int I>
struct nctype_to_type;

template<>
struct nctype_to_type<NC_CHAR> {
    using type = char;
};
template<>
struct nctype_to_type<NC_BYTE> {
    using type = signed char;
};
template<>
struct nctype_to_type<NC_UBYTE> {
    using type = unsigned char;
};
template<>
struct nctype_to_type<NC_SHORT> {
    using type = short;
};
template<>
struct nctype_to_type<NC_INT> {
    using type = int;
};
template<>
struct nctype_to_type<NC_FLOAT> {
    using type = float;
};
template<>
struct nctype_to_type<NC_DOUBLE> {
    using type = double;
};
template<>
struct nctype_to_type<NC_USHORT> {
    using type = unsigned short;
};
template<>
struct nctype_to_type<NC_UINT> {
    using type = unsigned int;
};
template<>
struct nctype_to_type<NC_INT64> {
    using type = long long;
};
template<>
struct nctype_to_type<NC_UINT64> {
    using type = unsigned long long;
};
template<>
struct nctype_to_type<NC_STRING> {
    using type = char *;
};

/**
 * Map from primitive type to netCDF type.
 * Example: type_to_nctype<int>::value // NC_INT
 */

namespace impl {

    template <typename T> struct type_to_nctype;

    template<>
    struct type_to_nctype<char> {
        static constexpr int value = NC_CHAR;
    };
    template<>
    struct type_to_nctype<signed char> {
        static constexpr int value = NC_BYTE;
    };
    template<>
    struct type_to_nctype<unsigned char> {
        static constexpr int value = NC_BYTE;
    };
    template<>
    struct type_to_nctype<short> {
        static constexpr int value = NC_SHORT;
    };
    template<>
    struct type_to_nctype<int> {
        static constexpr int value = NC_INT;
    };
    template<>
    struct type_to_nctype<float> {
        static constexpr int value = NC_FLOAT;
    };
    template<>
    struct type_to_nctype<double> {
        static constexpr int value = NC_DOUBLE;
    };
    template<>
    struct type_to_nctype<unsigned short> {
        static constexpr int value = NC_USHORT;
    };
    template<>
    struct type_to_nctype<unsigned int> {
        static constexpr int value = NC_UINT;
    };
    template<>
    struct type_to_nctype<long long> {
        static constexpr int value = NC_INT64;
    };
    template<>
    struct type_to_nctype<unsigned long long> {
        static constexpr int value = NC_UINT64;
    };
    template<>
    struct type_to_nctype<char *> {
        static constexpr int value = NC_STRING;
    };
    template<>
    struct type_to_nctype<const char *> {
        static constexpr int value = NC_STRING;
    };

} // namespace impl

template<typename T>
using type_to_nctype = impl::type_to_nctype<std::decay_t<T>>;

} // namespace ncpp

#endif // NCPP_TYPES_HPP