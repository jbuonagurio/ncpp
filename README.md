<!--
  Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  https://www.boost.org/LICENSE_1_0.txt)
-->

## ncpp - netCDF header-only C++ Library

ncpp provides a convenient, high-level C++17 interface to the netCDF-C library, inspired by [BlueBrain/HighFive](https://github.com/BlueBrain/HighFive), [xtensor-stack/xframe](https://github.com/xtensor-stack/xframe) and [pydata/xarray](https://github.com/pydata/xarray). Licensed under the [Boost Software License](http://www.boost.org/LICENSE_1_0.txt).

ncpp is in an early stage of development, and currently supports read-only access to netCDF datasets.

### Features

* STL-compatible iterators for dimensions, variables and attributes
* Flexible indexing methods for data selection using coordinate variables
* Adaptors for STL containers, Boost.MultiArray and Boost.uBLAS
* CF-compliant date and time conversion using [HowardHinnant/date](https://github.com/HowardHinnant/date)
* Streaming operators for CDL metadata
* Error handling based on `std::error_code`

### Example

The following example illustrates subset selection using the `ECMWF_ERA-40_subset.nc` file from [Unidata Example netCDF files](https://www.unidata.ucar.edu/software/netcdf/examples/files.html).

```cpp
#include <iostream>
#include <date/date.h>

#define NCPP_USE_DATE_H
#include <ncpp/ncpp.hpp>

int main(int argc, char *argv[])
{
    ncpp::file f("ECMWF_ERA-40_subset.nc", ncpp::file::read);
    ncpp::dataset ds(f);
    
    // Select the "tcw" variable
    auto tcw = ds.vars["tcw"];
    
    // Subset selection using coordinate variables: "latitude", "longitude", "time"
    date::sys_days start = date::year{2002}/7/1;
    date::sys_days end = date::year{2002}/7/4;
    auto slice = tcw.select(
        ncpp::selection<date::sys_days>{"time", start, end, 2 /* stride */},
        ncpp::selection<double>{"latitude", 77.5, 80},
        ncpp::selection<double>{"longitude", 7.5, 10}
    );

    // Print the selection shape
    std::cout << "shape: (";
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
        std::cout << "tcw(" << date::format("%F %R",std::get<0>(c))
                  << "," << std::get<1>(c)
                  << "," << std::get<2>(c)
                  << ")\t= " << v << "\n";
}

```

Output:

```
shape: (4,2,2)
tcw(2002-07-01 12:00,80,7.5)    = -23261
tcw(2002-07-01 12:00,80,10)     = -23675
tcw(2002-07-01 12:00,77.5,7.5)  = -23473
tcw(2002-07-01 12:00,77.5,10)   = -23216
tcw(2002-07-02 12:00,80,7.5)    = -22941
tcw(2002-07-02 12:00,80,10)     = -23111
tcw(2002-07-02 12:00,77.5,7.5)  = -22842
tcw(2002-07-02 12:00,77.5,10)   = -22928
tcw(2002-07-03 12:00,80,7.5)    = -24562
tcw(2002-07-03 12:00,80,10)     = -24639
tcw(2002-07-03 12:00,77.5,7.5)  = -22612
tcw(2002-07-03 12:00,77.5,10)   = -22277
tcw(2002-07-04 12:00,80,7.5)    = -25866
tcw(2002-07-04 12:00,80,10)     = -26102
tcw(2002-07-04 12:00,77.5,7.5)  = -22928
tcw(2002-07-04 12:00,77.5,10)   = -22186
```
