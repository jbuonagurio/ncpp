// Copyright (c) 2020 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_ITERATOR_HPP
#define NCPP_ITERATOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ncpp/config.hpp>

#include <ncpp/functions/dataset.hpp>
#include <ncpp/functions/variable.hpp>
#include <ncpp/functions/ndarray.hpp>
#include <ncpp/error.hpp>
#include <ncpp/types.hpp>

#include <cstddef>
#include <iterator>
#include <vector>

namespace ncpp {

struct block_iterator
{
    block_iterator(int ncid, int varid, const index_type& start, const index_type& shape, std::size_t blocksize)
        : ncid_(ncid), varid_(varid), next_(start), start_(start), shape_(shape),
          count_(index_type(shape.size(), 0)), init_blocksize_(blocksize)
    {
        if (start.size() != shape.size())
            detail::throw_error(error::argument_out_of_domain); // NC_EEDGE
    }

    block_iterator(int ncid, int varid, const index_type& start, const index_type& shape)
        : ncid_(ncid), varid_(varid), next_(start), start_(start), shape_(shape),
          count_(index_type(shape.size(), 0))
    {
        if (start.size() != shape.size())
            detail::throw_error(error::argument_out_of_domain); // NC_EEDGE
        
        // Calculate the number of elements per block using default buffer size.
        // Use buffer size for contiguous variables, otherwise total chunk size.
        std::size_t elemsize = api::inq_type_size(ncid, api::inq_vartype(ncid, varid));
        std::size_t chunksize = api::inq_var_chunksize(ncid, varid);
        if (chunksize == 0)
            init_blocksize_ = NCPP_DEFAULT_BUFFER_SIZE / elemsize;
        else
            init_blocksize_ = chunksize / elemsize;
    }

    block_iterator(const block_iterator& rhs) = default;
    block_iterator(block_iterator&& rhs) = default;
    block_iterator& operator=(const block_iterator& rhs) = default;
    block_iterator& operator=(block_iterator&& rhs) = default;

    bool operator==(const block_iterator& rhs) const {
        return ncid_ == rhs.ncid_ &&
               varid_ == rhs.varid_ &&
               next_ == rhs.start_ &&
               start_ == rhs.start_ &&
               shape_ == rhs.shape_ &&
               count_ == rhs.count_ &&
               offset_ == rhs.offset_ &&
               blocksize_ == rhs.blocksize_ &&
               init_blocksize_ == rhs.init_blocksize_;
    }

    bool operator!=(const block_iterator& rhs) const { 
        return !(*this == rhs);
    }

    /// Get the current linear offset into the data array.
    std::size_t offset() const {
        return offset_;
    }

    /// Get the start index for the current block.
    index_type start() const {
        return start_;
    }

    /// Get the counts (edge lengths) for the current block.
    index_type count() const {
        return count_;
    }

    /// Get the current block size (number of elements).
    std::size_t blocksize() const {
        return blocksize_;
    }

    /// Prefix increment operator.
    block_iterator operator++() {
        next();
        return *this;
    }

    /// Postfix increment operator.
    block_iterator operator++(int) {
        auto it = *this;
        next();
        return it;
    }

    // Move to the next block. Returns false when past the end.
    bool next()
    {
        if (offset_ >= api::compute_size(shape_))
            return false;
        
        blocksize_ = api::compute_block_size(init_blocksize_, shape_, next_, count_);
        start_ = next_;
        next_ = api::unravel_index(blocksize_, start_, shape_);
        offset_ += blocksize_;
        return true;
    }

    /// Get values as a std::vector.
    template <class T, class A = std::allocator<T>>
    std::vector<T, A> values() const
    {
        const stride_type stride(shape_.size(), 1);
        return api::get_vars<std::vector<T, A>>(ncid_, varid_, start_, count_, stride);
    }

private:
    int ncid_;
    int varid_;
    index_type next_;
    index_type start_;
    index_type shape_;
    index_type count_;
    std::size_t offset_ = 0;
    std::size_t blocksize_ = 0;
    std::size_t init_blocksize_ = 0;
};

} // namespace ncpp

#endif // NCPP_ITERATOR_HPP
