# Copyright (c) 2018 John Buonagurio (jbuonagurio at exponent dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.13)

if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
  set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "")
endif()

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(ncpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Options
option(NCPP_USE_BOOST "Enable Boost support" ON)
option(NCPP_USE_DATE_H "Enable Date support" ON)
option(NCPP_BUILD_EXAMPLES "Build examples" ON)
option(NCPP_BUILD_DOCS "Build documentation" OFF)

# Dependencies
find_package(netCDF CONFIG REQUIRED)

message("-- netCDF Version: ${netCDF_VERSION}")

# Target
set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/detail/dispatch.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/detail/functions.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/detail/utilities.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/attribute.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/attributes.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/check.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/config.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/dataset.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/dimension.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/dimensions.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/error.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/file.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/inquire.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/ncpp.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/selection.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/types.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/variable.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/variables.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/variant.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ncpp/version.hpp)

add_library(ncpp INTERFACE)

target_include_directories(ncpp INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_directories(ncpp INTERFACE ${netCDF_LIB_DIR})
target_link_libraries(ncpp INTERFACE netcdf)

if(NCPP_USE_BOOST)
  find_package(Boost REQUIRED)
  target_compile_definitions(ncpp INTERFACE NCPP_USE_BOOST)
  target_include_directories(ncpp INTERFACE
      "$<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>")
endif()

if(NCPP_USE_DATE_H)
  find_package(date CONFIG REQUIRED)
  target_compile_definitions(ncpp INTERFACE NCPP_USE_DATE_H)
  target_link_libraries(ncpp INTERFACE date::date)
endif()

if(NCPP_BUILD_DOCS)
  find_package(standardese REQUIRED)
  standardese_generate(ncpp CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/doc/standardese.config
                       INCLUDE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
                       INPUT ${HEADERS})
endif()

if(NCPP_BUILD_EXAMPLES)
  add_executable(simple ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/simple.cpp)
  target_link_libraries(simple PRIVATE ncpp)
endif()
