cmake_minimum_required(VERSION 3.9)
set(lib_name "udt4")
project(${lib_name})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $ENV{LIBOutput})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{DLLOutput})
set(lib_name_suffix "$ENV{BDSRTLCODE}$ENV{PLATFORMCODE}")

add_library(${lib_name} SHARED)

set_embt_target(${lib_name} DynamicRuntime)

set_target_properties(${lib_name} PROPERTIES COMPILE_DEFINITIONS "UDT_EXPORTS;WIN32" SUFFIX "_${lib_name_suffix}.dll")

target_sources(${lib_name} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/buffer.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cache.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ccc.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/channel.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/epoll.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/list.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/md5.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/packet.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/queue.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/window.cpp>
)

# deploy the header file to common include folder...
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/udt.h" DESTINATION "$ENV{BDSCOMMONDIR}/include")
