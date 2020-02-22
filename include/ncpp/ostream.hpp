// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_OSTREAM_HPP
#define NCPP_OSTREAM_HPP 

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#ifdef NCPP_USE_VARIANT // Requires std::variant (C++17)

#include <ncpp/dataset.hpp>
#include <ncpp/dimensions.hpp>
#include <ncpp/variables.hpp>
#include <ncpp/attributes.hpp>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <variant>

namespace ncpp {

/// Streaming operator for variant types.
inline std::ostream& operator<<(std::ostream& os, const ncpp::variant& v)
{
    std::visit([&os](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            os << "\"";
            std::string token;
            std::istringstream iss(arg);
            while (std::getline(iss, token)) {
                os << token;
                if (iss.eof())
                    break;
                os << "\\n";
            }
            os << "\"";
        }
        else {
            std::string separator;
            for (const auto& x : arg) {
                os << separator << x;
                separator = ", ";
            }
        }
    }, v);

    return os;
}

/// Streaming operator for dimension name and size in CDL format.
inline std::ostream& operator<<(std::ostream& os, const ncpp::dimension& rhs)
{
    if (rhs.is_unlimited())
        os << rhs.name() << " = UNLIMITED ; // (" << rhs.length() << " currently)";
    else
        os << rhs.name() << " = " << rhs.length() << " ;";
    return os;
}

/// Streaming operator for variable type, name and dimensions in CDL format.
inline std::ostream& operator<<(std::ostream& os, const ncpp::variable& rhs)
{
    os << ncpp::nctype_name(rhs.nctype()) << " ";
    os << rhs.name() << "(";
    std::string separator;
    for (const auto& dim : rhs.dims) {
        os << separator << dim.name();
        separator = ", ";
    }
    os << ") ;";
    return os;
}

/// Streaming operator for attribute name and value in CDL format.
inline std::ostream& operator<<(std::ostream& os, const ncpp::attribute& rhs)
{
    ncpp::variant v;
    rhs.read(v);
    os << rhs.name() << " = " << v << " ;";
    return os;
}

/// Streaming operator for dataset headers in CDL format.
inline std::ostream& operator<<(std::ostream& os, const ncpp::dataset& rhs)
{
    os << "dimensions:\n";
    for (const auto& dim : rhs.dims) {
        os << "\t" << dim << "\n";
    }
    
    os << "variables:\n";
    for (const auto& var : rhs.vars) {
        std::cout << "\t" << var << "\n";
        for (const auto& att : var.atts) {
            os << "\t\t" << var.name() << ":" << att << "\n";
        }
    }
    
    os << "// global attributes:\n";
    for (const auto& att : rhs.atts) {
        os << "\t:" << att << "\n";
    }
    
    return os;
}

} // namespace ncpp

#endif // NCPP_USE_VARIANT
#endif // NCPP_OSTREAM_HPP 