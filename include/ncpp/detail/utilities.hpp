// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_DETAIL_UTILITIES_HPP
#define NCPP_DETAIL_UTILITIES_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <initializer_list>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace ncpp {
namespace detail {

// Type trait to detect narrowing conversions. Adapted from MPark.Variant:
// Copyright Michael Park, 2015-2017
// Distributed under the Boost Software License, Version 1.0.
// https://github.com/mpark/variant

template <class From, class To>
struct is_non_narrowing_convertible {
    template <class T>
    static std::true_type test(T(&&)[1]);

    template <class T>
    static auto impl(int) -> decltype(test<T>({ std::declval<From>() }));

    template <class>
    static auto impl(...) -> std::false_type;

    static constexpr bool value = decltype(impl<To>(0))::value;
};


// Apply function with signature (index, value) to elements of tuple.
// Based on N3915: "apply() call a function with arguments from a tuple (V3)"
// http://www.open-std.org/jtc1/sc22/WG21/docs/papers/2014/n3915.pdf

template <class F, class Tuple, std::size_t... Is>
constexpr void apply_index_impl(F&& f, Tuple&& t, std::index_sequence<Is...>)
{
    (void)std::initializer_list<int> { 
        ( f(std::integral_constant<std::size_t, Is>{}, std::get<Is>(std::forward<Tuple>(t))), void(), 0 )...
    };
}

template <class F, class Tuple>
constexpr void apply_index(F&& f, Tuple&& t)
{
    using Indices = std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>;
    apply_index_impl(std::forward<F>(f), std::forward<Tuple>(t), Indices{});
}

template <class F>
constexpr void cartesian_product_impl(F f)
{
    f();
}


// Recursively calculate the cartesian product of vectors.
// Based on: https://stackoverflow.com/a/13841673

template <class F, class H, class... Ts>
constexpr void cartesian_product_impl(F f, const std::vector<H>& h, const std::vector<Ts>&... t)
{
    for (const H& he : h) {
        cartesian_product_impl([&](const Ts&... ts) {
            f(he, ts...);
        }, t...);
    }
}

template <class... Ts>
std::vector<std::tuple<Ts...>> cartesian_product(const std::vector<Ts>&... in)
{
    std::vector<std::tuple<Ts...>> res;
    cartesian_product_impl([&](const Ts&... ts) {
        res.emplace_back(ts...);
    }, in...);
    return res;
}

template <class... Ts, size_t... Is>
auto tuple_cartesian_product_impl(const std::tuple<Ts...> &t, std::index_sequence<Is...>)
{
    return cartesian_product(std::get<Is>(t)...);
}

template<class... Ts>
auto tuple_cartesian_product(const std::tuple<Ts...> &t)
{
    return tuple_cartesian_product_impl(t, std::index_sequence_for<Ts...> {});
}

} // namespace detail
} // namespace ncpp

#endif // NCPP_DETAIL_UTILITIES_HPP
