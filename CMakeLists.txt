cmake_minimum_required(VERSION 3.8.2)
project(cctalk)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CCTALK_PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.62.0 REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(include)
add_subdirectory(src)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR ENABLE_TESTING)
        include(cmake/test.cmake)

        add_subdirectory(test)

        add_all_test_targets()
    endif()
endif()
