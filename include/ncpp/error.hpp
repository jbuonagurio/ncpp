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

#include <ncpp/config.hpp>

#include <netcdf.h>
#include <string>
#include <system_error>

namespace ncpp {
namespace error {

enum netcdf_errors
{
    invalid_id                       = NC_EBADID,          // Not a valid ID
    too_many_files_open              = NC_ENFILE,          // Too many files open
    file_exists                      = NC_EEXIST,          // File exists && NC_NOCLOBBER
    invalid_argument                 = NC_EINVAL,          // Invalid argument
    operation_not_permitted          = NC_EPERM,           // Write to read only
    not_in_define_mode               = NC_ENOTINDEFINE,    // Operation not allowed in data mode
    in_define_mode                   = NC_EINDEFINE,       // Operation not allowed in define mode
    invalid_coordinates              = NC_EINVALCOORDS,    // Index exceeds dimension bound
    too_many_dimensions              = NC_EMAXDIMS,        // NC_MAX_DIMS exceeded
    name_in_use                      = NC_ENAMEINUSE,      // String match to name in use
    attribute_not_found              = NC_ENOTATT,         // Attribute not found
    too_many_attributes              = NC_EMAXATTS,        // NC_MAX_ATTRS exceeded
    invalid_data_type                = NC_EBADTYPE,        // Not a valid data type or _FillValue type mismatch
    invalid_dimension                = NC_EBADDIM,         // Invalid dimension ID or name
    bad_unlimited_index              = NC_EUNLIMPOS,       // NC_UNLIMITED in the wrong index
    too_many_variables               = NC_EMAXVARS,        // NC_MAX_VARS exceeded
    variable_not_found               = NC_ENOTVAR,         // Variable not found
    global_variable                  = NC_EGLOBAL,         // Action prohibited on NC_GLOBAL varid
    not_a_netcdf_file                = NC_ENOTNC,          // Unknown file format
    string_too_short                 = NC_ESTS,            // In Fortran string too short
    name_too_long                    = NC_EMAXNAME,        // NC_MAX_NAME exceeded
    unlimited_id_in_use              = NC_EUNLIMIT,        // NC_UNLIMITED size already in use
    no_record_variables              = NC_ENORECVARS,      // nc_rec op when there are no record vars
    invalid_conversion               = NC_ECHAR,           // Attempt to convert between text & numbers
    argument_out_of_domain           = NC_EEDGE,           // Start+count exceeds dimension bound
    illegal_stride                   = NC_ESTRIDE,         // Illegal stride
    invalid_name                     = NC_EBADNAME,        // Name contains illegal characters
    result_out_of_range              = NC_ERANGE,          // Numeric conversion not representable
    not_enough_memory                = NC_ENOMEM,          // Memory allocation (malloc) failure
    invalid_variable_size            = NC_EVARSIZE,        // One or more variable sizes violate format constraints
    invalid_dimension_size           = NC_EDIMSIZE,        // Invalid dimension size
    file_truncated                   = NC_ETRUNC,          // File likely truncated or possibly corrupted
    invalid_axis_type                = NC_EAXISTYPE,       // Illegal axis type
    
    // DAP Errors

    dap_error                        = NC_EDAP,            // DAP failure
    libcurl_error                    = NC_ECURL,           // libcurl failure
    io_error                         = NC_EIO,             // I/O failure
    no_message_available             = NC_ENODATA,         // Variable has no data in DAP request
    dap_server_error                 = NC_EDAPSVC,         // DAP server error
    invalid_das                      = NC_EDAS,            // Malformed or inaccessible DAP DAS
    invalid_dds                      = NC_EDDS,            // Malformed or inaccessible DAP DDS
    invalid_datadds                  = NC_EDATADDS,        // Malformed or inaccessible DAP DATADDS
    invalid_dap_url                  = NC_EDAPURL,         // Malformed URL
    invalid_dap_constraint           = NC_EDAPCONSTRAINT,  // Malformed or unexpected Constraint
    translation_error                = NC_ETRANSLATION,    // Untranslatable construct
    permission_denied                = NC_EACCESS,         // Access failure
    authorization_error              = NC_EAUTH,           // Authorization failure

    // Misc Errors

    file_not_found                   = NC_ENOTFOUND,       // file not found
    file_delete_error                = NC_ECANTREMOVE,     // cannot delete file
    internal_error                   = NC_EINTERNAL,       // internal library error; Please contact Unidata support
    pnetcdf_error                    = NC_PNETCDF,         // PnetCDF error
    
    // HDF5 Errors (netCDF-4)

    hdf5_error                       = NC_EHDFERR,         // HDF error
    file_read_error                  = NC_ECANTREAD,       // Can't read file
    file_write_error                 = NC_ECANTWRITE,      // Can't write file
    file_create_error                = NC_ECANTCREATE,     // Can't create file
    invalid_file_metadata            = NC_EFILEMETA,       // Can't add HDF5 file metadata
    invalid_dimension_metadata       = NC_EDIMMETA,        // Can't define dimensional metadata
    invalid_attribute_metadata       = NC_EATTMETA,        // Can't open HDF5 attribute
    invalid_variable_metadata        = NC_EVARMETA,        // Problem with variable metadata.
    not_a_compound_type              = NC_ENOCOMPOUND,     // Can't create HDF5 compound type
    attribute_exists                 = NC_EATTEXISTS,      // Attempt to create attribute that already exists
    invalid_netcdf3_operation        = NC_ENOTNC4,         // Attempting netcdf-4 operation on netcdf-3 file
    invalid_strict_netcdf3_operation = NC_ESTRICTNC3,      // Attempting netcdf-4 operation on strict nc3 netcdf-4 file
    invalid_netcdf4_operation        = NC_ENOTNC3,         // Attempting netcdf-3 operation on netcdf-4 file
    parallel_not_supported           = NC_ENOPAR,          // Parallel operation on file opened for non-parallel access
    parallel_initialization_error    = NC_EPARINIT,        // Error initializing for parallel access
    invalid_group_id                 = NC_EBADGRPID,       // Bad group ID
    invalid_type_id                  = NC_EBADTYPID,       // Bad type ID
    type_already_defined             = NC_ETYPDEFINED,     // Type has already been defined and may not be edited
    invalid_field_id                 = NC_EBADFIELD,       // Bad field ID
    invalid_class                    = NC_EBADCLASS,       // Bad class
    no_mapped_access                 = NC_EMAPTYPE,        // Mapped access for atomic types only
    fill_value_exists                = NC_ELATEFILL,       // Attempt to define fill value when data already exists.
    late_definition                  = NC_ELATEDEF,        // Attempt to define var properties like deflate after enddef.
    hdf5_dimscale_error              = NC_EDIMSCALE,       // Probem with HDF5 dimscales.
    group_not_found                  = NC_ENOGRP,          // No group found.
    invalid_storage_spec             = NC_ESTORAGE,        // Cannot specify both contiguous and chunking.
    bad_chunk_size                   = NC_EBADCHUNK,       // Bad chunk sizes.
    not_supported                    = NC_ENOTBUILT,       // Attempt to use feature that was not turned on when netCDF was built.
    diskless_access_error            = NC_EDISKLESS,       // Error in using diskless access
    extend_dimension_error           = NC_ECANTEXTEND,     // Attempt to extend dataset during NC_INDEPENDENT I/O operation. Use nc_var_par_access to set mode NC_COLLECTIVE before extending variable.
    mpi_error                        = NC_EMPI,            // MPI operation failed.
    filter_operation_error           = NC_EFILTER,         // Filter error: bad id or parameters or filter library non-existent
    header_bytes_not_null_padded     = NC_ENULLPAD,        // File fails strict Null-Byte Header check.
    in_memory_file_error             = NC_INMEMORY         // In-memory File operation failed.
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

inline std::error_code make_error_code(netcdf_errors c) {
    return std::error_code(static_cast<int>(c), get_netcdf_category());
}

inline std::error_code make_error_code(int c) {
    return std::error_code(c, get_netcdf_category());
}

} // namespace error
} // namespace ncpp


namespace ncpp {
namespace error {
namespace detail {

class netcdf_category : public std::error_category {
public:
    virtual const char *name() const noexcept override final { return "netCDF"; }

    virtual std::string message(int c) const override final { return nc_strerror(c); }

    virtual std::error_condition default_error_condition(int c) const noexcept override final
    {
        switch (c) {
        case ENFILE:
            return make_error_condition(std::errc::too_many_files_open_in_system);
        case EEXIST:
            return make_error_condition(std::errc::file_exists);
        case EINVAL:
            return make_error_condition(std::errc::invalid_argument);
        case EPERM:
            return make_error_condition(std::errc::operation_not_permitted);
        case ERANGE:
            return make_error_condition(std::errc::result_out_of_range);
        case ENOMEM:
            return make_error_condition(std::errc::not_enough_memory);
        case EIO:
            return make_error_condition(std::errc::io_error);
        default:
            return std::error_condition(c, *this);
        }
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