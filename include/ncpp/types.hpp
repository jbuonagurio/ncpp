// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
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
#include <netcdf_meta.h>

#include <ncpp/config.hpp>

#include <cstddef>
#include <chrono>
#include <string>
#include <type_traits>
#include <vector>

namespace ncpp {

using index_type = std::vector<std::size_t>;
using stride_type = std::vector<std::ptrdiff_t>;

enum class var_endian_type {
    native = NC_ENDIAN_NATIVE,
    little = NC_ENDIAN_LITTLE,
    big    = NC_ENDIAN_BIG
};

enum class var_storage_type {
    contiguous = NC_CONTIGUOUS,
    chunked    = NC_CHUNKED,
#if NC_VERSION_MAJOR * 100 + NC_VERSION_MINOR * 10 + NC_VERSION_PATCH >= 474
    compact    = NC_COMPACT
#endif
};

enum class var_filter_type : unsigned int {
    lzo        = 305,  
    bzip2      = 307,  
    lzf        = 32000,
    blosc      = 32001,
    mafisc     = 32002,
    snappy     = 32003,
    lz4        = 32004,
    apax       = 32005,
    cbf        = 32006,
    jpeg_xr    = 32007,
    bitshuffle = 32008,
    spdp       = 32009,
    lpc_rice   = 32010,
    ccsds_123  = 32011,
    jpeg_ls    = 32012,
    zfp        = 32013,
    fpzip      = 32014,
    zstandard  = 32015,
    b3d        = 32016,
    sz         = 32017,
    fcidecomp  = 32018,
    jpeg       = 32019,
    vbz        = 32020
};

struct var_chunk_cache {
    std::size_t size;   // total size of the raw data chunk cache in bytes
    std::size_t nelems; // number of chunk slots in the raw data chunk cache hash table
    float preemption;   // premption value, between 0 and 1 (inclusive)
};

// Parsed CF time attribute.

template <class C, class D>
struct cf_time {
    std::chrono::time_point<C, D> start;
    std::chrono::seconds scale;
};

// Map from netCDF type to value type.
// Example: typeid(netcdf_type_to_type<NC_INT>::type).name()

template<int I> struct netcdf_type_to_type;

template<>
struct netcdf_type_to_type<NC_CHAR> {
    using type = char;
};
template<>
struct netcdf_type_to_type<NC_BYTE> {
    using type = signed char;
};
template<>
struct netcdf_type_to_type<NC_UBYTE> {
    using type = unsigned char;
};
template<>
struct netcdf_type_to_type<NC_SHORT> {
    using type = short;
};
template<>
struct netcdf_type_to_type<NC_INT> {
    using type = int;
};
template<>
struct netcdf_type_to_type<NC_FLOAT> {
    using type = float;
};
template<>
struct netcdf_type_to_type<NC_DOUBLE> {
    using type = double;
};
template<>
struct netcdf_type_to_type<NC_USHORT> {
    using type = unsigned short;
};
template<>
struct netcdf_type_to_type<NC_UINT> {
    using type = unsigned int;
};
template<>
struct netcdf_type_to_type<NC_INT64> {
    using type = long long;
};
template<>
struct netcdf_type_to_type<NC_UINT64> {
    using type = unsigned long long;
};
template<>
struct netcdf_type_to_type<NC_STRING> {
    using type = char *;
};

// Map from fundamental type to netCDF type.
// Example: type_to_netcdf_type<int>::type.value // NC_INT

namespace impl {

template <class T> struct type_to_netcdf_type;

template<>
struct type_to_netcdf_type<char> {
    using type = std::integral_constant<int, NC_CHAR>;
};
template<>
struct type_to_netcdf_type<signed char> {
    using type = std::integral_constant<int, NC_BYTE>;
};
template<>
struct type_to_netcdf_type<unsigned char> {
    using type = std::integral_constant<int, NC_BYTE>;
};
template<>
struct type_to_netcdf_type<short> {
    using type = std::integral_constant<int, NC_SHORT>;
};
template<>
struct type_to_netcdf_type<int> {
    using type = std::integral_constant<int, NC_INT>;
};
template<>
struct type_to_netcdf_type<float> {
    using type = std::integral_constant<int, NC_FLOAT>;
};
template<>
struct type_to_netcdf_type<double> {
    using type = std::integral_constant<int, NC_DOUBLE>;
};
template<>
struct type_to_netcdf_type<unsigned short> {
    using type = std::integral_constant<int, NC_USHORT>;
};
template<>
struct type_to_netcdf_type<unsigned int> {
    using type = std::integral_constant<int, NC_UINT>;
};
template<>
struct type_to_netcdf_type<long long> {
    using type = std::integral_constant<int, NC_INT64>;
};
template<>
struct type_to_netcdf_type<unsigned long long> {
    using type = std::integral_constant<int, NC_UINT64>;
};
template<>
struct type_to_netcdf_type<char *> {
    using type = std::integral_constant<int, NC_STRING>;
};
template<>
struct type_to_netcdf_type<const char *> {
    using type = std::integral_constant<int, NC_STRING>;
};

} // namespace impl

template<class T>
using type_to_netcdf_type = impl::type_to_netcdf_type<std::decay_t<T>>;

} // namespace ncpp

#endif // NCPP_TYPES_HPP