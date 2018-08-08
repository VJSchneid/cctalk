set(SOURCE_FILES
    bus.cpp
)

link_libraries(${Boost_LIBRARIES})


add_library(cctalk
    ${SOURCE_FILES}
)
