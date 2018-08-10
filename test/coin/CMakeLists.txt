set(SOURCE_FILES
    coin.cpp
    currency.cpp
    revision.cpp
    value.cpp
)

link_libraries(
    ${Boost_LIBRARIES}
    cctalk
)

add_executable(test_coin ${SOURCE_FILES})
