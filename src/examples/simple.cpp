// Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_USE_BOOST
#define NCPP_USE_BOOST
#endif

#ifndef NCPP_USE_DATE_H
#define NCPP_USE_DATE_H
#endif

#include <ncpp/ncpp.hpp>
#include <ncpp/ostream.hpp>

#include <iostream>
#include <string>

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
    std::cout << ncpp::inq_libvers() << "\n\n";
    
    try {
        ncpp::file f(filename, ncpp::file::read);
        ncpp::dataset ds(f);
        
        // Print internal attributes
        std::cout << "_IsNetcdf4 = " << f.is_netcdf4() << "\n";
        std::cout << "_NCProperties = " << f.properties() << "\n";
        std::cout << "_SuperblockVersion = " << f.superblock_version() << "\n\n";
        
        // Print CDL metadata (ncdump format)
        std::cout << ds << "\n";
        
        // Subset selection
        auto tcw = ds.vars["tcw"];
        
        date::sys_days start = date::year{2002}/7/1;
        date::sys_days end = date::year{2002}/7/4;
        auto slice = tcw.select(
            ncpp::selection<date::sys_days>{"time", start, end, 2 /* stride */},
            ncpp::selection<double>{"latitude", 77.5, 80},
            ncpp::selection<double>{"longitude", 7.5, 10}
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
                      << ")\t= " << v << "\n";
        }
        
        // Iterator support
        auto it = slice.begin<double>();
        while (it != slice.end<double>()) {
            std::cout << "\t";
            for (const auto& idx : it.index()) {
                std::cout << idx << ", ";
            }
            std::cout << "\t" << *it << "\n";
            std::advance(it, 1);
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
