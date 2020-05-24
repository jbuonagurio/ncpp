// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_DATASET_HPP
#define NCPP_DATASET_HPP 

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <netcdf.h>

#include <ncpp/config.hpp>

#include <ncpp/functions/dataset.hpp>
#include <ncpp/file.hpp>
#include <ncpp/dimensions.hpp>
#include <ncpp/variables.hpp>
#include <ncpp/attributes.hpp>
#include <ncpp/check.hpp>

namespace ncpp {

/// netCDF dataset type.
class dataset
{
public:
    explicit dataset(const file& file)
        : dims(file.ncid_), vars(file.ncid_), atts(file.ncid_), ncid_(file.ncid_)
    {}

    /// Dimensions associated with the netCDF dataset.
    dimensions_type dims;

    /// Variables associated with the netCDF dataset.
    variables_type vars;
    
    /// Global attributes associated with the netCDF dataset.
    attributes_type atts;

private:
    int ncid_;
};

} // namespace ncpp

#endif // NCPP_DATASET_HPP 