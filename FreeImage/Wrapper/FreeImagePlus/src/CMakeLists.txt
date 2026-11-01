cmake_minimum_required(VERSION 3.9)
project(FreeImagePlus)
set(lib_name "fip")
set(bds_common $ENV{BDSCOMMONDIR})

include_directories("$ENV{IncludePath}")
link_directories("$ENV{LibraryPath}")

get_filename_component(SRC_DIR_PARENT ../ ABSOLUTE)
# message(FATAL_ERROR ${SRC_DIR_PARENT})

add_library(${lib_name} SHARED)
set_embt_target(${lib_name} DynamicRuntime)

set_target_properties(${lib_name} PROPERTIES 
					COMPILE_DEFINITIONS "FIP_EXPORTS"
					OUTPUT_NAME "${lib_name}"
					SUFFIX "_$ENV{BDSRTLCODE}$ENV{PLATFORMCODE}.dll")

target_include_directories(${lib_name} PUBLIC
	$<BUILD_INTERFACE:${SRC_DIR_PARENT}>
)

target_sources(${lib_name} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/FreeImagePlus.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/fipImage.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/fipMemoryIO.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/fipMetadataFind.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/fipMultiPage.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/fipTag.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/fipWinIMage.cpp>
	$<BUILD_INTERFACE:${SRC_DIR_PARENT}/FreeImagePlus.rc>
)

target_link_libraries(${lib_name} "FreeImage${CMAKE_IMPORT_LIBRARY_SUFFIX}")
