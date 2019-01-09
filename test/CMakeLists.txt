set(SOURCE_FILES
    test.cpp
)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.62.0 REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(coin)

link_libraries(cctalk pthread)

add_executable(test ${SOURCE_FILES})
