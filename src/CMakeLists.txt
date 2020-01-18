set(SOURCE_FILES
    bus.cpp
    coin.cpp
    coinAcceptor.cpp
)

link_libraries(${Boost_LIBRARIES})

add_library(cctalk
    ${SOURCE_FILES}
)

if (MINGW)
    target_link_libraries(cctalk ws2_32)
endif()

if (WITH_PYTHON_LIB)
  add_subdirectory(py)
endif ()
