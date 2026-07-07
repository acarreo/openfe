cmake_minimum_required(VERSION 3.16)
project(openfe VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(OPENFE_USE_LOCAL_BPDPVS "Build bpdpvs from local ../bp-dpvs checkout" ON)
option(OPENFE_BUILD_MAIN "Build openfe test executable" ON)

if(OPENFE_USE_LOCAL_BPDPVS)
    set(_BPDPVS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../bp-dpvs")
    if(NOT EXISTS "${_BPDPVS_SOURCE_DIR}/CMakeLists.txt")
        message(FATAL_ERROR "Local bp-dpvs not found at ${_BPDPVS_SOURCE_DIR}")
    endif()

    # Build bp-dpvs locally as a dependency of openfe.
    set(TESTS OFF CACHE BOOL "" FORCE)
    set(BENCH OFF CACHE BOOL "" FORCE)
    set(BUILD_MAIN OFF CACHE BOOL "" FORCE)
    set(BPDPVS_INSTALL OFF CACHE BOOL "" FORCE)
    add_subdirectory("${_BPDPVS_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/_deps/bp-dpvs-build")
else()
    find_package(bpdpvs CONFIG REQUIRED)
endif()

add_library(openfe src/openfe.cpp)
add_library(OPENFE::openfe ALIAS openfe)

target_include_directories(openfe
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(openfe PUBLIC BPDPVS::bpdpvs)

if(OPENFE_BUILD_MAIN)
    add_executable(openfe_main main.cpp)
    target_link_libraries(openfe_main PRIVATE OPENFE::openfe)
endif()
