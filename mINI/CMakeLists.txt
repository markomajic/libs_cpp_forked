cmake_minimum_required(VERSION 3.18)
project(mINI LANGUAGES CXX)

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()

find_path(MINI_BOOST_INCLUDE_DIR boost/locale/encoding_utf.hpp REQUIRED)

add_library(mINI STATIC
    src/mINI/ini_common.cpp
    src/mINI/ini_parser.cpp
    src/mINI/unicode_utils.cpp
)
target_compile_features(mINI PUBLIC cxx_std_17)
target_include_directories(mINI PUBLIC src "${MINI_BOOST_INCLUDE_DIR}")
target_compile_definitions(mINI PUBLIC MINI_NO_AUTOLINK BOOST_LOCALE_NO_LIB)

include(CTest)
if(BUILD_TESTING)
    foreach(test_name IN ITEMS testcasesens testcopy testgenerate testhuge testread testutf8 testwrite testregressions)
        add_executable(${test_name} tests/${test_name}.cpp)
        target_link_libraries(${test_name} PRIVATE mINI)
        # The original tests reuse filenames; isolate their working directories.
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tests/${test_name}")
        add_test(NAME ${test_name} COMMAND ${test_name})
        set_tests_properties(${test_name} PROPERTIES
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tests/${test_name}")
    endforeach()
endif()
