/**
 * Sample command line build using Visual Studio 2017:
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
#include <auto_link.h>

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
            std::cout << "\t" << var.name() << "\n";
            for (const auto& att : var.atts) {
                std::cout << "\t\t" << att << "\n";
            }
        }
        
        std::cout << "\nATTRIBUTES:\n";
        for (const auto& att : ds.atts) {
            std::cout << "\t" << att << "\n";
        }
        
        // Print the time variable array.
        std::cout << "\nTIME VARIABLE:\n";
        auto time = ds.vars["time"].value<int>();
        std::cout << "\t" << time << "\n";
    }
    catch (std::system_error& e) {
        std::cerr << e.code() << "\n";
        std::cerr << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
