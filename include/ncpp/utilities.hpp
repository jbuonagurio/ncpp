// Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_UTILITIES_HPP
#define NCPP_UTILITIES_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ncpp/config.hpp>

#include <chrono>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace ncpp {
namespace detail {

// Type traits for std::chrono.

template <typename T>
struct is_chrono_duration : std::false_type {};

template <typename R, typename P>
struct is_chrono_duration<std::chrono::duration<R, P>> : std::true_type {};

template <typename T>
struct is_chrono_time_point : std::false_type {};

template <typename C, typename D>
struct is_chrono_time_point<std::chrono::time_point<C, D>> : std::true_type {};

// Implement index_sequence class templates for C++11 compatibility.

#if __cplusplus < 201703 || (defined(_MSVC_LANG) && _MSVC_LANG < 201703L)
template <size_t ...I>
struct index_sequence {};

template <size_t N, size_t ...I>
struct make_index_sequence : public make_index_sequence<N - 1, N - 1, I...> {};

template <size_t ...I>
struct make_index_sequence<0, I...> : public index_sequence<I...> {};

template< typename ... T >
struct index_sequence_for : public make_index_sequence<sizeof...(T)> {};
#endif

// Apply function with signature (index, value) to elements of tuple.
// Based on N3915: "apply() call a function with arguments from a tuple (V3)"
// http://www.open-std.org/jtc1/sc22/WG21/docs/papers/2014/n3915.pdf

template <typename F, typename Tuple, std::size_t... Is>
constexpr void apply_index_impl(F&& f, Tuple&& t, std::index_sequence<Is...>)
{
    (void)std::initializer_list<int> { 
        ( f(std::integral_constant<std::size_t, Is>{}, std::get<Is>(std::forward<Tuple>(t))), void(), 0 )...
    };
}

template <typename F, typename Tuple>
constexpr void apply_index(F&& f, Tuple&& t)
{
    using Indices = std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>;
    apply_index_impl(std::forward<F>(f), std::forward<Tuple>(t), Indices{});
}

// Recursively calculate the cartesian product of vectors.
// Adapted from: https://stackoverflow.com/a/13841673

template <typename F>
constexpr void cartesian_product_impl(F f)
{
    f();
}

template <typename F, typename H, typename... Ts>
constexpr void cartesian_product_impl(F f, const std::vector<H>& h, const std::vector<Ts>&... t)
{
    for (const H& he : h) {
        cartesian_product_impl([&](const Ts&... ts) {
            f(he, ts...);
        }, t...);
    }
}

template <typename... Ts>
std::vector<std::tuple<Ts...>> cartesian_product(const std::vector<Ts>&... in)
{
    std::vector<std::tuple<Ts...>> res;
    cartesian_product_impl([&](const Ts&... ts) {
        res.emplace_back(ts...);
    }, in...);
    return res;
}

template <typename... Ts, size_t... Is>
auto tuple_cartesian_product_impl(const std::tuple<Ts...> &t, std::index_sequence<Is...>) {
    return cartesian_product(std::get<Is>(t)...);
}

template<typename... Ts>
auto tuple_cartesian_product(const std::tuple<Ts...> &t)
{
    return tuple_cartesian_product_impl(t, std::index_sequence_for<Ts...> {});
}

} // namespace detail
} // namespace ncpp

#endif // NCPP_UTILITIES_HPP
