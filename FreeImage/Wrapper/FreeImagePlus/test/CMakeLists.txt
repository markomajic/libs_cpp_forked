cmake_minimum_required(VERSION 3.9)
project(FreeImagePlus)
set(exe_name "fipTest")
set(bds_common $ENV{BDSCOMMONDIR})

# if(BCC64)
# set(platform_code "$ENV{BDSRTLCODE}x64")
# elseif(${BCC32C})
# set(platform_code "$ENV{BDSRTLCODE}x86")
# else()
# 	message(FATAL_ERROR "Unknown/unsupported compiler platform!")
# 	return()
# endif()

include_directories("$ENV{IncludePath}")
link_directories("$ENV{LibraryPath}")

get_filename_component(SRC_DIR_PARENT ../ ABSOLUTE)
# message(FATAL_ERROR ${SRC_DIR_PARENT})

add_executable(${exe_name})
set_embt_target(${exe_name} DynamicRuntime)

# set_target_properties(${lib_name} PROPERTIES 
# 					COMPILE_DEFINITIONS "FIP_EXPORTS"
# 					OUTPUT_NAME "${lib_name}"
# 					SUFFIX "${platform_code}.dll")

# target_include_directories(${lib_name} PUBLIC
# 	$<BUILD_INTERFACE:${SRC_DIR_PARENT}>
# )

target_sources(${exe_name} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/fipTest.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/fipTestMemIO.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/fipTestMPage.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/fipTestMPageMemory.cpp>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/fipTestMPageStream.cpp>
)

target_link_libraries(${exe_name} "FreeImage${CMAKE_IMPORT_LIBRARY_SUFFIX}")
target_link_libraries(${exe_name} "fip${CMAKE_IMPORT_LIBRARY_SUFFIX}")

file(GLOB TEST_DATA_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.jpg"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.tif"
)
file(COPY ${TEST_DATA_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
