cmake_minimum_required(VERSION 3.9)
project(udt4_tests)

message(STATUS "${CMAKE_IMPORT_LIBRARY_SUFFIX}")
message(STATUS "$ENV{LibraryPath}")

set(CMAKE_FIND_DEBUG_MODE TRUE)

# find_file (udt4_lib "udt4${CMAKE_IMPORT_LIBRARY_SUFFIX}" PATHS $ENV{LIBRARYPATH} REQUIRED)
find_file (udt4_lib "udt4.lib" PATHS $ENV{LibraryPath} REQUIRED NO_DEFAULT_PATH)
find_file (udt_h "udt.h" PATHS $ENV{IncludePath} REQUIRED NO_DEFAULT_PATH)

set(test_src_files test.cpp;sendfile.cpp;recvfile.cpp;appserver.cpp;appclient.cpp)

foreach(test_src ${test_src_files})
	string(REPLACE ".c" "" exe_name ${test_src})
	add_executable(${exe_name} "${CMAKE_CURRENT_SOURCE_DIR}/${test_src}" "${udt_h}")
	set_target_properties(${exe_name} PROPERTIES
		# COMPILE_DEFINITIONS "SODIUM_EXPORT_EXTERNAL;SODIUM_EXPORT=__declspec(dllimport);SODIUM_EXPORT_WEAK=SODIUM_EXPORT"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
	)
	target_include_directories(${exe_name} PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${lib_src_dir}>
		$<BUILD_INTERFACE:$ENV{INCLUDEPATH}>
	)
	target_link_directories(${exe_name} PUBLIC
		$<BUILD_INTERFACE:$ENV{LIBRARYPATH}>
	)
endforeach()
