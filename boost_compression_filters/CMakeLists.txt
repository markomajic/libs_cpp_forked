cmake_minimum_required(VERSION 4.1)
project(boost_compression_filters LANGUAGES C CXX)

# The patched CMake supplied for C++Builder knows the Modern Win64x runtime flags.
get_filename_component(compiler_name "${CMAKE_CXX_COMPILER}" NAME_WE)
if(NOT WIN32 OR NOT compiler_name STREQUAL "bcc64x" OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "This project builds Win64x libraries with bcc64x.exe.")
endif()
if(NOT COMMAND set_embt_target)
    message(FATAL_ERROR "Use Embarcadero's CMake build; set_embt_target() is required.")
endif()
if(CMAKE_CONFIGURATION_TYPES)
    message(FATAL_ERROR "Use the single-configuration Ninja generator.")
endif()
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Debug or Release" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)
if(NOT CMAKE_BUILD_TYPE MATCHES "^(Debug|Release)$")
    message(FATAL_ERROR "CMAKE_BUILD_TYPE must be Debug or Release.")
endif()

set(BOOST_ROOT "$ENV{BDSBOOSTROOT}" CACHE PATH "Boost headers shipped with RAD Studio")
set(BDS_COMMON_DIR "$ENV{BDSCOMMONDIR}" CACHE PATH "RAD Studio common output directory")
if(NOT EXISTS "${BOOST_ROOT}/boost/version.hpp")
    message(FATAL_ERROR "Set BDSBOOSTROOT or pass -DBOOST_ROOT=<Boost header directory>.")
endif()
if(NOT BDS_COMMON_DIR)
    message(FATAL_ERROR "Set BDSCOMMONDIR or pass -DBDS_COMMON_DIR=<output directory>.")
endif()
file(TO_CMAKE_PATH "${BOOST_ROOT}" BOOST_ROOT)
file(TO_CMAKE_PATH "${BDS_COMMON_DIR}" BDS_COMMON_DIR)
file(STRINGS "${BOOST_ROOT}/boost/version.hpp" boost_version_line
    REGEX "^#define BOOST_LIB_VERSION ")
string(REGEX REPLACE ".*\"([^\"]+)\".*" "\\1" boost_version "${boost_version_line}")
string(REGEX MATCH "^[0-9]+" compiler_major "${CMAKE_CXX_COMPILER_VERSION}")
set(debug_code "")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(debug_code "-d")
endif()
set(lib_name_suffix "-mgw${compiler_major}-mt${debug_code}-x64-${boost_version}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BDS_COMMON_DIR}/lib/Win64x/${CMAKE_BUILD_TYPE}")
set(boost_src_dir "${CMAKE_CURRENT_SOURCE_DIR}/boost_src")
get_filename_component(compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(BOOST_FILTERS_NM NAMES llvm-nm HINTS "${compiler_dir}" REQUIRED)

function(add_compression_libraries target source_dir)
    # DLL wrappers need dllexport/dllimport; static wrappers must be compiled separately.
    add_library(${target} STATIC ${ARGN})
    add_library(${target}_dll SHARED ${ARGN})
    foreach(build_target IN ITEMS ${target} ${target}_dll)
        set_embt_target(${build_target} DynamicRuntime)
        target_include_directories(${build_target} PRIVATE "${BOOST_ROOT}" "${source_dir}")
        set_target_properties(${build_target} PROPERTIES
            PREFIX "" OUTPUT_NAME "${target}${lib_name_suffix}")
    endforeach()
    set_target_properties(${target} PROPERTIES SUFFIX ".a")
    set_target_properties(${target}_dll PROPERTIES
        SUFFIX ".dll" IMPORT_PREFIX "" IMPORT_SUFFIX ".dll.a"
        RUNTIME_OUTPUT_DIRECTORY "${BDS_COMMON_DIR}/DLL/Win64x/${CMAKE_BUILD_TYPE}")
    target_compile_definitions(${target}_dll PUBLIC BOOST_IOSTREAMS_DYN_LINK)

    foreach(build_target IN ITEMS ${target} ${target}_dll)
        # TARGET_LINKER_FILE is the static archive or the DLL's import archive.
        add_custom_command(TARGET ${build_target} POST_BUILD
            COMMAND "${CMAKE_COMMAND}"
                "-DNM=${BOOST_FILTERS_NM}"
                "-DARCHIVE=$<TARGET_LINKER_FILE:${build_target}>"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/check_archive.cmake"
            VERBATIM COMMENT "Checking ${build_target} for unintended program entry points")
    endforeach()
endfunction()

include(bz2.cmake)
include(zlib.cmake)
message(STATUS "Compression archives: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
message(STATUS "Compression DLLs: ${BDS_COMMON_DIR}/DLL/Win64x/${CMAKE_BUILD_TYPE}")
message(STATUS "Library suffix: ${lib_name_suffix}")

option(BOOST_FILTERS_BUILD_TESTS "Build static and DLL compression round-trip checks for CTest" OFF)
if(BOOST_FILTERS_BUILD_TESTS)
    enable_testing()
    foreach(linkage IN ITEMS static dll)
        set(test_target compression_roundtrip_${linkage})
        add_executable(${test_target} tests/compression_roundtrip.cpp)
        set_embt_target(${test_target} DynamicRuntime)
        target_compile_definitions(${test_target} PRIVATE BOOST_ALL_NO_LIB)
        target_include_directories(${test_target} PRIVATE "${BOOST_ROOT}")
        if(linkage STREQUAL "dll")
            target_link_libraries(${test_target} PRIVATE libboost_bzip2_dll libboost_zlib_dll)
        else()
            target_link_libraries(${test_target} PRIVATE libboost_bzip2 libboost_zlib)
        endif()
        add_test(NAME ${test_target} COMMAND ${test_target})
        set_property(TEST ${test_target} PROPERTY ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:${compiler_dir}"
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:libboost_bzip2_dll>")
    endforeach()
endif()
