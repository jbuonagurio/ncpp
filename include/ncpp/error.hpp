// Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NCPP_ERROR_HPP
#define NCPP_ERROR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <netcdf.h>

#include <string>
#include <system_error>

namespace ncpp {
namespace error {

enum netcdf_errors
{
    invalid_id                       = NC_EBADID,
    too_many_files_open              = NC_ENFILE,
    file_exists                      = NC_EEXIST,
    invalid_argument                 = NC_EINVAL,
    operation_not_permitted          = NC_EPERM,
    not_in_define_mode               = NC_ENOTINDEFINE,
    in_define_mode                   = NC_EINDEFINE,
    invalid_coordinates              = NC_EINVALCOORDS,
    too_many_dimensions              = NC_EMAXDIMS,
    name_in_use                      = NC_ENAMEINUSE,
    attribute_not_found              = NC_ENOTATT,
    too_many_attributes              = NC_EMAXATTS,
    invalid_data_type                = NC_EBADTYPE,
    invalid_dimension                = NC_EBADDIM,
    bad_unlimited_index              = NC_EUNLIMPOS,
    too_many_variables               = NC_EMAXVARS,
    variable_not_found               = NC_ENOTVAR,
    global_variable                  = NC_EGLOBAL,
    not_a_netcdf_file                = NC_ENOTNC,
    string_too_short                 = NC_ESTS,
    name_too_long                    = NC_EMAXNAME,
    unlimited_id_in_use              = NC_EUNLIMIT,
    no_record_variables              = NC_ENORECVARS,
    invalid_conversion               = NC_ECHAR,
    argument_out_of_domain           = NC_EEDGE,
    illegal_stride                   = NC_ESTRIDE,
    invalid_name                     = NC_EBADNAME,
    result_out_of_range              = NC_ERANGE,
    not_enough_memory                = NC_ENOMEM,
    invalid_variable_size            = NC_EVARSIZE,
    invalid_dimension_size           = NC_EDIMSIZE,
    file_truncated                   = NC_ETRUNC,
    invalid_axis_type                = NC_EAXISTYPE,
    
    // DAP Errors

    dap_error                        = NC_EDAP,
    libcurl_error                    = NC_ECURL,
    io_error                         = NC_EIO,
    no_message_available             = NC_ENODATA,
    dap_server_error                 = NC_EDAPSVC,
    invalid_das                      = NC_EDAS,
    invalid_dds                      = NC_EDDS,
    invalid_datadds                  = NC_EDATADDS,
    invalid_dap_url                  = NC_EDAPURL,
    invalid_dap_constraint           = NC_EDAPCONSTRAINT,
    translation_error                = NC_ETRANSLATION,
    permission_denied                = NC_EACCESS,
    authorization_error              = NC_EAUTH,

    // Misc Errors

    file_not_found                   = NC_ENOTFOUND,
    file_delete_error                = NC_ECANTREMOVE,
    internal_error                   = NC_EINTERNAL,
    pnetcdf_error                    = NC_PNETCDF,
    
    // HDF5 Errors (netCDF-4)

    hdf5_error                       = NC_EHDFERR,
    file_read_error                  = NC_ECANTREAD,
    file_write_error                 = NC_ECANTWRITE,
    file_create_error                = NC_ECANTCREATE,
    invalid_file_metadata            = NC_EFILEMETA,
    invalid_dimension_metadata       = NC_EDIMMETA,
    invalid_attribute_metadata       = NC_EATTMETA,
    invalid_variable_metadata        = NC_EVARMETA,
    not_a_compound_type              = NC_ENOCOMPOUND,
    attribute_exists                 = NC_EATTEXISTS,
    invalid_netcdf3_operation        = NC_ENOTNC4,
    invalid_strict_netcdf3_operation = NC_ESTRICTNC3,
    invalid_netcdf4_operation        = NC_ENOTNC3,
    parallel_not_supported           = NC_ENOPAR,
    parallel_initialization_error    = NC_EPARINIT,
    invalid_group_id                 = NC_EBADGRPID,
    invalid_type_id                  = NC_EBADTYPID,
    type_already_defined             = NC_ETYPDEFINED,
    invalid_field_id                 = NC_EBADFIELD,
    invalid_class                    = NC_EBADCLASS,
    no_mapped_access                 = NC_EMAPTYPE,
    fill_value_exists                = NC_ELATEFILL,
    late_definition                  = NC_ELATEDEF,
    hdf5_dimscale_error              = NC_EDIMSCALE,
    group_not_found                  = NC_ENOGRP,
    invalid_storage_spec             = NC_ESTORAGE,
    bad_chunk_size                   = NC_EBADCHUNK,
    not_supported                    = NC_ENOTBUILT,
    diskless_access_error            = NC_EDISKLESS,
    extend_dimension_error           = NC_ECANTEXTEND,
    mpi_error                        = NC_EMPI,
    filter_operation_error           = NC_EFILTER,
    header_bytes_not_null_padded     = NC_ENULLPAD,
    in_memory_file_error             = NC_INMEMORY
};

extern inline const std::error_category& get_netcdf_category();

static const std::error_category& netcdf_category = get_netcdf_category();

} // namespace error
} // namespace ncpp


namespace std {

template<>
struct is_error_code_enum<ncpp::error::netcdf_errors> : public true_type {};

} // namespace std


namespace ncpp {
namespace error {

inline std::error_code make_error_code(netcdf_errors e) {
    return std::error_code(static_cast<int>(e), get_netcdf_category());
}

inline std::error_code make_error_code(int e) {
    return std::error_code(e, get_netcdf_category());
}

} // namespace error
} // namespace ncpp


namespace ncpp {
namespace error {
namespace detail {

class netcdf_category : public std::error_category {
public:
    const char* name() const noexcept {
        return "netCDF";
    }

    std::string message(int value) const {
        return std::string(nc_strerror(value));
    }
};

} // namespace detail

const std::error_category& get_netcdf_category() {
    static detail::netcdf_category instance;
    return instance;
}

} // namespace error
} // namespace ncpp


namespace ncpp {
namespace detail {

template <typename Exception>
void throw_exception(const Exception& e);

// Only define the throw_exception function when exceptions are enabled.
// Otherwise, it is up to the application to provide a definition of this
// function.

#ifndef NCPP_NO_EXCEPTIONS
template <typename Exception>
void throw_exception(const Exception& e)
{
    throw e;
}
#endif // !NCPP_NO_EXCEPTIONS

inline void throw_error(const std::error_code& err)
{
    if (err) {
        std::system_error e(err);
        ncpp::detail::throw_exception(e);
    }
}

inline void throw_error(const std::error_code& err, const char* location)
{
    if (err) {
        std::system_error e(err, location);
        ncpp::detail::throw_exception(e);
    }
}

inline void throw_error(const int err)
{
    if (err) {
        std::error_code ec = ncpp::error::make_error_code(err);
        std::system_error e(ec);
        ncpp::detail::throw_exception(e);
    }
}

inline void throw_error(const int err, const char* location)
{
    if (err) {
        std::error_code ec = ncpp::error::make_error_code(err);
        std::system_error e(ec, location);
        ncpp::detail::throw_exception(e);
    }
}

} // namespace detail
} // namespace ncpp

#endif // NCPP_ERROR_HPP