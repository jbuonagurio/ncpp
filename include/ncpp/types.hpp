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

#include <ncpp/config.hpp>

#include <string>
#include <type_traits>

namespace ncpp {

inline const char* filter_name(int id)
{
    switch (id) {
    case 305:   return "LZO";
    case 307:   return "BZIP2";
    case 32000: return "LZF";
    case 32001: return "Blosc";
    case 32002: return "MAFISC";
    case 32003: return "Snappy";
    case 32004: return "LZ4";
    case 32005: return "APAX";
    case 32006: return "CBF";
    case 32007: return "JPEG_XR";
    case 32008: return "bitshuffle";
    case 32009: return "SPDP";
    case 32010: return "LPC-Rice";
    case 32011: return "CCSDS-123";
    case 32012: return "JPEG-LS";
    case 32013: return "zfp";
    case 32014: return "fpzip";
    case 32015: return "Zstandard";
    case 32016: return "B3D";
    case 32017: return "SZ";
    case 32018: return "FCIDECOMP";
    case 32019: return "JPEG";
    case 32020: return "VBZ";
    default:    return "unknown";
    }
}

inline const char* data_type_name(int id)
{
    switch (id) {
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
    default:          return "unknown";
    }
}

// Map from netCDF type to value type.
// Example: typeid(data_type_to_type<NC_INT>::type).name()

template<int I> struct data_type_to_type;

template<>
struct data_type_to_type<NC_CHAR> {
    using type = char;
};
template<>
struct data_type_to_type<NC_BYTE> {
    using type = signed char;
};
template<>
struct data_type_to_type<NC_UBYTE> {
    using type = unsigned char;
};
template<>
struct data_type_to_type<NC_SHORT> {
    using type = short;
};
template<>
struct data_type_to_type<NC_INT> {
    using type = int;
};
template<>
struct data_type_to_type<NC_FLOAT> {
    using type = float;
};
template<>
struct data_type_to_type<NC_DOUBLE> {
    using type = double;
};
template<>
struct data_type_to_type<NC_USHORT> {
    using type = unsigned short;
};
template<>
struct data_type_to_type<NC_UINT> {
    using type = unsigned int;
};
template<>
struct data_type_to_type<NC_INT64> {
    using type = long long;
};
template<>
struct data_type_to_type<NC_UINT64> {
    using type = unsigned long long;
};
template<>
struct data_type_to_type<NC_STRING> {
    using type = char *;
};

// Map from fundamental type to netCDF type.
// Example: type_to_data_type<int>::value // NC_INT

namespace impl {

template <typename T> struct type_to_data_type;

template<>
struct type_to_data_type<char> {
    static constexpr int value = NC_CHAR;
};
template<>
struct type_to_data_type<signed char> {
    static constexpr int value = NC_BYTE;
};
template<>
struct type_to_data_type<unsigned char> {
    static constexpr int value = NC_BYTE;
};
template<>
struct type_to_data_type<short> {
    static constexpr int value = NC_SHORT;
};
template<>
struct type_to_data_type<int> {
    static constexpr int value = NC_INT;
};
template<>
struct type_to_data_type<float> {
    static constexpr int value = NC_FLOAT;
};
template<>
struct type_to_data_type<double> {
    static constexpr int value = NC_DOUBLE;
};
template<>
struct type_to_data_type<unsigned short> {
    static constexpr int value = NC_USHORT;
};
template<>
struct type_to_data_type<unsigned int> {
    static constexpr int value = NC_UINT;
};
template<>
struct type_to_data_type<long long> {
    static constexpr int value = NC_INT64;
};
template<>
struct type_to_data_type<unsigned long long> {
    static constexpr int value = NC_UINT64;
};
template<>
struct type_to_data_type<char *> {
    static constexpr int value = NC_STRING;
};
template<>
struct type_to_data_type<const char *> {
    static constexpr int value = NC_STRING;
};

} // namespace impl

template<typename T>
using type_to_data_type = impl::type_to_data_type<std::decay_t<T>>;

} // namespace ncpp

#endif // NCPP_TYPES_HPP