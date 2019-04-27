/**
 * Handle automatic linking to netCDF with MSVC.
 * 
 * -- HDF5 configured with SZip and zlib support.
 * -- libcurl configured with SSPI and WinSSL.
 * -- netCDF configured with OpeNDAP support.
 * 
 * Naming convention is the same as Boost:
 * 
 *   LIB_PREFIX
 *     + LIB_NAME
 *     + "_"
 *     + LIB_TOOLSET
 *     + LIB_THREAD_OPT
 *     + LIB_RT_OPT
 *     + LIB_ARCH_AND_MODEL_OPT
 *     "-"
 *     + LIB_VERSION
 */

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
 #ifndef _DEBUG
  #ifdef _MT
   #ifdef _WIN64
    #pragma comment(lib, "libcurl_vc141-mt-s-x64-7_62_0.lib")
    #pragma comment(lib, "libszip_vc141-mt-s-x64-2_1_1.lib")
    #pragma comment(lib, "libzlib_vc141-mt-s-x64-1.2.11.lib")
    #pragma comment(lib, "libhdf5_vc141-mt-s-x64-1_8_21.lib")
    #pragma comment(lib, "libhdf5_hl_vc141-mt-s-x64-1_8_21.lib")
    #pragma comment(lib, "libnetcdf_vc141-mt-s-x64-4_6_2.lib")
   #else
    #pragma comment(lib, "libcurl_vc141-mt-s-x32-7_62_0.lib")
    #pragma comment(lib, "libszip_vc141-mt-s-x32-2_1_1.lib")
    #pragma comment(lib, "libzlib_vc141-mt-s-x32-1.2.11.lib")
    #pragma comment(lib, "libhdf5_vc141-mt-s-x32-1_8_21.lib")
    #pragma comment(lib, "libhdf5_hl_vc141-mt-s-x32-1_8_21.lib")
    #pragma comment(lib, "libnetcdf_vc141-mt-s-x32-4_6_2.lib")
   #endif
  #else
   #ifdef _WIN64
    #pragma comment(lib, "libcurl_vc141-md-s-x64-7_62_0.lib")
    #pragma comment(lib, "libszip_vc141-md-s-x64-2_1_1.lib")
    #pragma comment(lib, "libzlib_vc141-md-s-x64-1.2.11.lib")
    #pragma comment(lib, "libhdf5_vc141-md-s-x64-1_8_21.lib")
    #pragma comment(lib, "libhdf5_hl_vc141-md-s-x64-1_8_21.lib")
    #pragma comment(lib, "libnetcdf_vc141-md-s-x64-4_6_2.lib")
   #else
    #pragma comment(lib, "libcurl_vc141-md-s-x32-7_62_0.lib")
    #pragma comment(lib, "libszip_vc141-md-s-x32-2_1_1.lib")
    #pragma comment(lib, "libzlib_vc141-md-s-x32-1.2.11.lib")
    #pragma comment(lib, "libhdf5_vc141-md-s-x32-1_8_21.lib")
    #pragma comment(lib, "libhdf5_hl_vc141-md-s-x32-1_8_21.lib")
    #pragma comment(lib, "libnetcdf_vc141-md-s-x32-4_6_2.lib")
   #endif
  #endif
 #endif
#endif

// SSPI and WinSSL dependencies:

#if defined(_MSC_VER)
 #pragma comment(lib, "crypt32.lib")
 #pragma comment(lib, "wldap32.lib")
 #pragma comment(lib, "normaliz.lib")
 #pragma comment(lib, "wsock32.lib")
 #pragma comment(lib, "ws2_32.lib")
 #pragma comment(lib, "kernel32.lib")
 #pragma comment(lib, "user32.lib")
 #pragma comment(lib, "advapi32.lib")
 #pragma comment(lib, "Ws2_32.lib")
#endif