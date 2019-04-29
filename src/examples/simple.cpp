/**
 * Sample command line build using MSVC:
 *
 * cl simple.cpp /EHsc /MT /std:c++17 /I..\..\include /I..\..\deps\include /link /LIBPATH:..\..\deps\lib
 */

#include <iomanip>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

// Implements C++20 extensions for <chrono>
// https://github.com/HowardHinnant/date
#include <date/date.h>

#define NCPP_USE_DATE_H
#define NCPP_USE_VARIANT // Requires std::variant (C++17)
#include <ncpp/ncpp.hpp>

// Automatically link static libraries for MSVC.
#include "auto_link.h"

// Helper to print the contents of a variant.
static std::ostream& operator<<(std::ostream& os, const ncpp::variant& v)
{
    std::visit([&os](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            os << std::quoted(arg);
        }
        else {
            std::string separator;
            for (const auto& x : arg) {
                os << separator << x;
                separator = ",";
            }
        }
    }, v);

    return os;
}

// Helper to print an attribute name/value pair.
static std::ostream& operator<<(std::ostream& os, const ncpp::attribute& rhs)
{
    ncpp::variant v;
    rhs.read(v);
    os << rhs.name() << " = " << v;
    return os;
}

int main(int argc, char *argv[])
{
    // Test file: surface data for July 2002 from the ECMWF 40 Years Re-Analysis
    // https://www.unidata.ucar.edu/software/netcdf/examples/files.html
    
    std::string filename;
    if (argc > 1)
        filename = argv[1];
    else
        filename = "./data/ECMWF_ERA-40_subset.nc";
    
    // netCDF library version
    std::cout << ncpp::library_version() << "\n\n";
    
    try {
        ncpp::file f(filename, ncpp::file::read);
        ncpp::dataset ds(f);
        
        // Range-based access to metadata
        std::cout << "dimensions:\n";
        for (const auto& dim : ds.dims) {
            std::cout << "\t" << dim.name() << "\n";
        }
        
        std::cout << "variables:\n";
        for (const auto& var : ds.vars) {
            std::cout << "\t" << var.name() << "(";
            std::string separator;
            for (const auto& dim : var.dims) {
                std::cout << separator << dim.name();
                separator = ",";
            }
            std::cout << ")\n";
            for (const auto& att : var.atts) {
                std::cout << "\t\t" << att << "\n";
            }
        }
        
        std::cout << "attributes:\n";
        for (const auto& att : ds.atts) {
            std::cout << "\t" << att << "\n";
        }
        
        // Subset selection
        std::cout << "selection:\n";
        auto tcw = ds.vars["tcw"];
        
        date::sys_days start = date::year{2002}/7/1;
        date::sys_days end = date::year{2002}/7/3;
        auto slice = tcw
            .select<double>({
                {"latitude", 80, 80},
                {"longitude", 10, 10}
            })
            .select<date::sys_days>({
                {"time", start, end}
            });
        
        // Selection shape
        std::cout << "\tshape: (";
        std::string separator;
        for (const auto& i : slice.shape()) {
            std::cout << separator << i;
            separator = ",";
        }
        std::cout << ")\n";
        
        // Print coordinates and values
        auto coordinates = slice.coordinates<date::sys_seconds, double, double>();
        auto values = slice.values<double>();

        for (int i = 0; i < coordinates.size(); ++i) {
            auto c = coordinates.at(i);
            auto v = values.at(i);
            std::cout << "\ttcw(" << date::format("%F %R",std::get<0>(c))
                      << "," << std::get<1>(c)
                      << "," << std::get<2>(c)
                      << ") = " << v << "\n";
        }
    }
    catch (std::system_error& e) {
        std::cerr << e.code() << "\n";
        std::cerr << e.what() << "\n";
        return 1;
    }
    catch (std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
