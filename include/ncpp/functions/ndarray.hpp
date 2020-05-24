// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_FUNCTIONS_NDARRAY_HPP
#define NCPP_FUNCTIONS_NDARRAY_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ncpp/config.hpp>

#include <ncpp/types.hpp>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iterator>
#include <numeric>
#include <system_error>
#include <vector>

namespace ncpp {
namespace api {

// Calculate number of elements required to step to next dimension when
// traversing an array. Assumes row-major order.
inline stride_type compute_strides(const index_type& shape)
{
    stride_type strides(shape.size());
    std::ptrdiff_t product = 1;
    for (std::size_t i = shape.size(); i != 0; --i) {
        strides[i-1] = shape[i-1] == 1 ? 0 : product;
        product = strides[i-1] * static_cast<std::ptrdiff_t>(shape[i-1]);
    }
    return strides;
}

// Calculate number of elements required to step from the end of a dimension
// to its beginning. This is the dimension length minus 1, multiplied by the
// stride. Assumes row-major order.
inline stride_type compute_backstrides(const index_type& shape)
{
    stride_type backstrides(shape.size());
    std::ptrdiff_t product = 1;
    for (std::size_t i = shape.size(); i != 0; --i) {
        backstrides[i-1] = shape[i-1] == 1 ? 0 : product * static_cast<std::ptrdiff_t>(shape[i-1] - 1);
        product *= static_cast<std::ptrdiff_t>(shape[i-1]);
    }
    return backstrides;
}

// Calculate linear offset from an index vector and shape.
inline std::size_t ravel_index(const index_type& index, const index_type& shape)
{
    assert(index.size() == shape.size());
    stride_type strides = compute_strides(shape);
    return std::inner_product(index.begin(), index.end(), strides.begin(), std::size_t(0));
}

// Create an index vector from a linear offset and shape.
inline index_type unravel_index(std::size_t offset, const index_type& shape)
{
    stride_type strides = compute_strides(shape);
    index_type result(shape.size(), 0);
    for (std::size_t i = 0; i < shape.size(); ++i) {
        if (strides[i] != 0) {
            auto q = std::div(static_cast<std::ptrdiff_t>(offset), strides[i]);
            offset = q.rem;
            result[i] = q.quot;
        }
    }
    return result;
}

// Create an index vector from a linear offset, start index vector and shape.
inline index_type unravel_index(std::size_t offset, const index_type& start, const index_type& shape)
{
    assert(shape.size() == start.size());
    offset += ravel_index(start, shape);
    return unravel_index(offset, shape);
}

// Remove single-dimensional entries from the shape of an array.
inline index_type squeeze(const index_type& shape)
{
    index_type result;
    std::copy_if(shape.cbegin(), shape.cend(), std::back_inserter(result),
        [](std::size_t i) { return i != 1; });
    return result;
}

// Calculate array size as product of dimensions, checking for overflow.
inline std::size_t compute_size(const index_type& shape)
{
    std::size_t n = 1;
    for (const auto& i : shape) {
        std::size_t x = n * i;
        if (n != 0 && x / n != i)
            throw std::system_error(std::make_error_code(std::errc::value_too_large)); // EOVERFLOW
        n = x;
    }
    return n;
}

// Calculate edge lengths using a maximum block size (number of elements) for
// block iteration. Block size is adjusted to ensure blocks are contiguous
// subarrays. Returns the adjusted block size.
inline std::size_t compute_block_size(std::size_t blocksize, const index_type& shape, const index_type& start, index_type& count)
{
    assert(shape.size() == start.size() == count.size());

    // Find the first dimension where blocksize is less than stride.
    stride_type strides = compute_strides(shape);
    auto it = std::partition_point(strides.begin(), strides.end(),
        [=](auto x) { return static_cast<std::ptrdiff_t>(blocksize) < x; });

    auto dim = std::distance(strides.begin(), it);

    // Adjust the buffer size to match the start and strides.
    if (it != strides.end()) {
        std::size_t inc = blocksize / strides.at(dim);
        if (start.at(dim) + inc > shape.at(dim))
            inc = shape.at(dim) - start.at(dim);
        blocksize = strides.at(dim) * inc;
    }
    
    // Set the edge lengths to shape, then update through the partition point.
    count = shape;
    if (it != strides.end()) {
        std::size_t n = blocksize;
        for (std::size_t i = 0; i < static_cast<std::size_t>(dim + 1); ++i) {
            if (strides.at(i) != 0) {
                auto q = std::div(static_cast<std::ptrdiff_t>(n), strides.at(i));
                n = q.rem;
                count.at(i) = std::max(q.quot, 1LL);
            }
        }
    }

    return blocksize;
}

} // namespace api
} // namespace ncpp

#endif // NCPP_FUNCTIONS_NDARRAY_HPP
