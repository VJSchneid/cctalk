set(SOURCE_FILES
    bus.cpp
)

find_package(PythonInterp)

if (PythonInterp_FOUND)
    if (PYTHON_VERSION_MAJOR EQUAL 3)
        find_package(Boost COMPONENTS python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR})
        find_package(PythonInterp 3)
        find_package(PythonLibs 3 REQUIRED)
    else()
        find_package(Boost COMPONENTS python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR})
        find_package(PythonInterp)
        find_package(PythonLibs REQUIRED)
    endif()
else()
    message(FATAL_ERROR "Python not found")
endif()

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${PYTHON_INCLUDE_DIRS}
)

link_libraries(
    ${Boost_LIBRARIES}
    ${PYTHON_LIBRARIES}
    cctalk
)

python_add_module(cctalk_py ${SOURCE_FILES})
