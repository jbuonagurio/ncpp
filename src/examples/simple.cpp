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
    // Using sea surface temperature example from Unidata.
    std::string filename;
    if (argc > 1)
        filename = argv[1];
    else
        filename = "./data/tos_O1_2001-2002.nc";
    
    std::cout << ncpp::library_version() << "\n";
    
    try {
        ncpp::file f(filename, ncpp::file::read);
        ncpp::dataset ds(f);
        
        // Range-based access to metadata.
        std::cout << "\nDIMENSIONS:\n";
        for (const auto& dim : ds.dims) {
            std::cout << "\t" << dim.name() << "\n";
        }
        
        std::cout << "\nVARIABLES:\n";
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
        
        std::cout << "\nATTRIBUTES:\n";
        for (const auto& att : ds.atts) {
            std::cout << "\t" << att << "\n";
        }
        
        // Coordinate variables for dimensions.
        std::cout << "\nCOORDINATES TEST:\n";
        auto time = ds.dims["time"].coordinates<int>();
        std::cout << "time: \t" << time << "\n\n";
        auto lat = ds.dims["lat"].coordinates<double>();
        std::cout << "lat: \t" << lat << "\n\n";
        auto lon = ds.dims["lon"].coordinates<double>();
        std::cout << "lon: \t" << lon << "\n\n";
        
        // Selection API.
        std::cout << "\nSELECTION TEST:\n";
        
        auto sst = ds.vars["tos"];
        auto slice = sst
            .select<double>({
                {"time", 375, 375},    // time(12)
                {"lat", -15.5, -15.5}, // lat(64)
                {"lon", 125, 125}      // lon(62)
            });
        
        std::cout << "shape:";
        for (const auto& i : slice.shape()) {
            std::cout << " " << i;
        }
        std::cout << "\n";
        
        auto coordinates = slice.coordinates<int, double, double>();
        auto values = slice.values<double>();

        for (int i = 0; i < coordinates.size(); ++i) {
            auto c = coordinates.at(i);
            auto v = values.at(i);
            std::cout << "[" << std::get<0>(c) 
                      << "," << std::get<1>(c)
                      << "," << std::get<2>(c)
                      << "]\t" << std::fixed << std::setprecision(4) << v << "\n";
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
