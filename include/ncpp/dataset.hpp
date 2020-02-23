// Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_DATASET_HPP
#define NCPP_DATASET_HPP 

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ncpp/config.hpp>

#include <netcdf.h>

#include <ncpp/file.hpp>
#include <ncpp/dimensions.hpp>
#include <ncpp/variables.hpp>
#include <ncpp/attributes.hpp>
#include <ncpp/check.hpp>

namespace ncpp {

/// netCDF dataset type.
class dataset {
private:
    int _ncid;

public:
    explicit dataset(const ncpp::file& file)
        : dims(_ncid), vars(_ncid), atts(_ncid), _ncid(file.ncid())
    {}

    /// Dimensions associated with the netCDF dataset.
    ncpp::dimensions_type dims;

    /// Variables associated with the netCDF dataset.
    ncpp::variables_type vars;
    
    /// Global attributes associated with the netCDF dataset.
    ncpp::attributes_type atts;
};

} // namespace ncpp

#endif // NCPP_DATASET_HPP 