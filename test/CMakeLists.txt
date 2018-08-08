set(SOURCE_FILES
    test.cpp
)

link_libraries(cctalk pthread)

add_executable(test ${SOURCE_FILES})
