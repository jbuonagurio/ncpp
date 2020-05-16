// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_FUNCTIONS_HPP
#define NCPP_FUNCTIONS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ncpp/config.hpp>

#include <ncpp/check.hpp>
#include <ncpp/detail/functions.hpp>
#include <ncpp/types.hpp>

#include <netcdf.h>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// This file provides free function wrappers for various netCDF-C
// API functions.
//
// Overloads which do not take a `std:error_code&` parameter throw
// `std::system_error` on underlying netCDF API errors.
// 
// Overloads which take a `std:error_code&` parameter set it to the
// netCDF error value if an API call fails. Any overload not marked
// `noexcept` may throw `std::bad_alloc` if memory allocation fails.

namespace ncpp {

inline std::string inq_libvers()
{
    return "netCDF " + std::string(nc_inq_libvers());
}

inline int inq_format(int ncid, std::error_code& ec) noexcept {
    return detail::inq_format(ncid, &ec);
}
inline int inq_format(int ncid) {
    return detail::inq_format(ncid);
}

inline std::vector<int> inq_dimids(int ncid, std::error_code& ec) {
    return detail::inq_dimids(ncid, &ec);
}
inline std::vector<int> inq_dimids(int ncid) {
    return detail::inq_dimids(ncid);
}

inline std::vector<int> inq_unlimdims(int ncid, std::error_code& ec) {
    return detail::inq_unlimdims(ncid, &ec);
}
inline std::vector<int> inq_unlimdims(int ncid) {
    return detail::inq_unlimdims(ncid);
}

inline std::vector<int> inq_varids(int ncid, std::error_code& ec) {
    return detail::inq_varids(ncid, &ec);
}
inline std::vector<int> inq_varids(int ncid) {
    return detail::inq_varids(ncid);
}

inline int inq_natts(int ncid, std::error_code& ec) noexcept {
    return detail::inq_natts(ncid, &ec);
}
inline int inq_natts(int ncid) {
    return detail::inq_natts(ncid);
}

inline std::optional<int> inq_dimid(int ncid, const std::string& dimname, std::error_code& ec) noexcept {
    return detail::inq_dimid(ncid, dimname, &ec);
}
inline std::optional<int> inq_dimid(int ncid, const std::string& dimname) {
    return detail::inq_dimid(ncid, dimname);
}

inline std::string inq_dimname(int ncid, int dimid, std::error_code& ec) {
    return detail::inq_dimname(ncid, dimid, &ec);
}
inline std::string inq_dimname(int ncid, int dimid) {
    return detail::inq_dimname(ncid, dimid);
}

inline std::size_t inq_dimlen(int ncid, int dimid, std::error_code& ec) noexcept {
    return detail::inq_dimlen(ncid, dimid, &ec);
}
inline std::size_t inq_dimlen(int ncid, int dimid) {
    return detail::inq_dimlen(ncid, dimid);
}

inline std::optional<int> inq_varid(int ncid, const std::string& varname, std::error_code& ec) noexcept {
    return detail::inq_varid(ncid, varname, &ec);
}
inline std::optional<int> inq_varid(int ncid, const std::string& varname) {
    return detail::inq_varid(ncid, varname);
}

inline std::string inq_varname(int ncid, int varid, std::error_code& ec) {
    return detail::inq_varname(ncid, varid, &ec);
}
inline std::string inq_varname(int ncid, int varid) {
    return detail::inq_varname(ncid, varid);
}

inline int inq_vartype(int ncid, int varid, std::error_code& ec) noexcept {
    return detail::inq_vartype(ncid, varid, &ec);
}
inline int inq_vartype(int ncid, int varid) {
    return detail::inq_vartype(ncid, varid);
}

inline int inq_varnatts(int ncid, int varid, std::error_code& ec) noexcept {
    return detail::inq_varnatts(ncid, varid, &ec);
}
inline int inq_varnatts(int ncid, int varid) {
    return detail::inq_varnatts(ncid, varid);
}

inline int inq_varndims(int ncid, int varid, std::error_code& ec) noexcept {
    return detail::inq_varndims(ncid, varid, &ec);
}
inline int inq_varndims(int ncid, int varid) {
    return detail::inq_varndims(ncid, varid);
}

inline std::vector<int> inq_vardimid(int ncid, int varid, std::error_code& ec) {
    return detail::inq_vardimid(ncid, varid, &ec);
}
inline std::vector<int> inq_vardimids(int ncid, int varid) {
    return detail::inq_vardimid(ncid, varid);
}

template <typename T>
inline std::optional<T> inq_var_fill(int ncid, int varid, std::error_code& ec) noexcept {
    return detail::inq_var_fill<T>(ncid, varid, &ec);
}
template <typename T>
inline std::optional<T> inq_var_fill(int ncid, int varid) {
    return detail::inq_var_fill<T>(ncid, varid);
}

inline std::optional<int> inq_var_chunking_storage(int ncid, int varid, std::error_code& ec) noexcept {
    return detail::inq_var_chunking_storage(ncid, varid, &ec);
}
inline std::optional<int> inq_var_chunking_storage(int ncid, int varid) {
    return detail::inq_var_chunking_storage(ncid, varid);
}

inline std::vector<std::size_t> inq_var_chunking_chunksizes(int ncid, int varid, std::error_code& ec) {
    return detail::inq_var_chunking_chunksizes(ncid, varid, &ec);
}
inline std::vector<std::size_t> inq_var_chunking_chunksizes(int ncid, int varid) {
    return detail::inq_var_chunking_chunksizes(ncid, varid);
}

inline std::optional<unsigned int> inq_var_filter_id(int ncid, int varid, std::error_code& ec) noexcept {
    return detail::inq_var_filter_id(ncid, varid, &ec);
}
inline std::optional<unsigned int> inq_var_filter_id(int ncid, int varid) {
    return detail::inq_var_filter_id(ncid, varid);
}

inline std::string inq_var_filter_name(int ncid, int varid, std::error_code& ec) {
    return detail::inq_var_filter_name(ncid, varid, &ec);
}
inline std::string inq_var_filter_name(int ncid, int varid) {
    return detail::inq_var_filter_name(ncid, varid);
}

inline var_chunk_cache get_var_chunk_cache(int ncid, int varid, std::error_code &ec) {
    return detail::get_var_chunk_cache(ncid, varid, &ec);
}
inline var_chunk_cache get_var_chunk_cache(int ncid, int varid) {
    return detail::get_var_chunk_cache(ncid, varid);
}

inline std::optional<int> inq_attid(int ncid, int varid, const std::string& attname, std::error_code& ec) noexcept {
    return detail::inq_attid(ncid, varid, attname, &ec);
}
inline std::optional<int> inq_attid(int ncid, int varid, const std::string& attname) {
    return detail::inq_attid(ncid, varid, attname);
}

inline std::string inq_attname(int ncid, int varid, int attnum, std::error_code& ec) {
    return detail::inq_attname(ncid, varid, attnum, &ec);
}
inline std::string inq_attname(int ncid, int varid, int attnum) {
    return detail::inq_attname(ncid, varid, attnum);
}

inline int inq_atttype(int ncid, int varid, const std::string& attname, std::error_code& ec) noexcept {
    return detail::inq_atttype(ncid, varid, attname, &ec);
}
inline int inq_atttype(int ncid, int varid, const std::string& attname) {
    return detail::inq_atttype(ncid, varid, attname);
}

inline std::size_t inq_attlen(int ncid, int varid, const std::string& attname, std::error_code& ec) noexcept {
    return detail::inq_attlen(ncid, varid, attname, &ec);
}
inline std::size_t inq_attlen(int ncid, int varid, const std::string& attname) {
    return detail::inq_attlen(ncid, varid, attname);
}

} // namespace ncpp

#endif // NCPP_FUNCTIONS_HPP


