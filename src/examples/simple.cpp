/**
 * Sample command line build using MSVC:
 *
 * cl simple.cpp /EHsc /MT /std:c++17 /I..\..\include /I..\..\deps\include /link /LIBPATH:..\..\deps\lib
 */

#include <iostream>
#include <string>

// Implements C++20 extensions for <chrono>
// https://github.com/HowardHinnant/date
#include <date/date.h>

#define NCPP_USE_DATE_H
#define NCPP_USE_VARIANT // std::variant required for ostream operators.

#include <ncpp/ncpp.hpp>
#include <ncpp/ostream.hpp>

// Automatically link static libraries for MSVC.
#include "auto_link.h"

int main(int argc, char *argv[])
{
    // Test file: surface data for July 2002 from the ECMWF 40 Years Re-Analysis
    // https://www.unidata.ucar.edu/software/netcdf/examples/files.html
    
    std::string filename;
    if (argc > 1)
        filename = argv[1];
    else
        filename = "./data/ECMWF_ERA-40_subset.nc";
    
    // Print netCDF library version
    std::cout << ncpp::library_version() << "\n\n";
    
    try {
        ncpp::file f(filename, ncpp::file::read);
        ncpp::dataset ds(f);
        
        // Print CDL metadata (ncdump format)
        std::cout << ds << "\n";
        
        // Subset selection
        std::cout << "selection:\n";
        auto tcw = ds.vars["tcw"];
        
        date::sys_days start = date::year{2002}/7/1;
        date::sys_days end = date::year{2002}/7/6;
        auto slice = tcw.select(
            ncpp::selection<date::sys_days>{"time", start, end, 2 /* stride */},
            ncpp::selection<double>{"latitude", 80, 80},
            ncpp::selection<double>{"longitude", 10, 10}
        );
        
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
