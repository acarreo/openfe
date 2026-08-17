cmake_minimum_required(VERSION 3.16)
project(openfe VERSION 0.1.0 LANGUAGES C CXX)

include(FetchContent)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect sources and public headers from subdirectories.
set(SOURCE_FILES "")
set(PUBLIC_HEADER "")

macro(add_sources)
    file(RELATIVE_PATH _relPath "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    foreach(_src ${ARGN})
        if(_relPath)
            list(APPEND SOURCE_FILES "${_relPath}/${_src}")
        else()
            list(APPEND SOURCE_FILES "${_src}")
        endif()
    endforeach()
    set(SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)
endmacro()

macro(add_header)
    file(RELATIVE_PATH _relPath "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    foreach(_src ${ARGN})
        if(_relPath)
            list(APPEND PUBLIC_HEADER "${_relPath}/${_src}")
        else()
            list(APPEND PUBLIC_HEADER "${_src}")
        endif()
    endforeach()
    set(PUBLIC_HEADER ${PUBLIC_HEADER} PARENT_SCOPE)
endmacro()

function(openfe_patch_bpdpvs_cmake source_dir)
    set(_bpdpvs_cmake "${source_dir}/CMakeLists.txt")
    if(EXISTS "${_bpdpvs_cmake}")
        file(READ "${_bpdpvs_cmake}" _bpdpvs_contents)
        string(REPLACE [=["-DSHLIB=ON -DSTBIN=OFF -DTESTS=OFF -DBENCH=OFF ${RELIC_SRC_DIR}"]=]
            [=["-DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DSHLIB=ON -DSTBIN=OFF -DTESTS=OFF -DBENCH=OFF ${RELIC_SRC_DIR}"]=]
            _bpdpvs_patched_contents
            "${_bpdpvs_contents}"
        )
        if(NOT _bpdpvs_patched_contents STREQUAL _bpdpvs_contents)
            file(WRITE "${_bpdpvs_cmake}" "${_bpdpvs_patched_contents}")
        endif()
    endif()
endfunction()

option(OPENFE_USE_LOCAL_BPDPVS "Build bpdpvs from local ../bp-dpvs checkout" ON)
option(OPENFE_BUILD_MAIN "Build openfe test executable" ON)
set(OPENFE_BPDPVS_GIT_REPOSITORY "https://github.com/acarreo/bp-dpvs.git" CACHE STRING "bp-dpvs git repository used when local checkout is unavailable")
set(OPENFE_BPDPVS_GIT_TAG "main" CACHE STRING "bp-dpvs git tag/branch/commit used when fetching" FORCE)

if(OPENFE_USE_LOCAL_BPDPVS)
    set(_BPDPVS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../bp-dpvs")
    if(EXISTS "${_BPDPVS_SOURCE_DIR}/CMakeLists.txt")
        set(TESTS OFF CACHE BOOL "" FORCE)
        set(BENCH OFF CACHE BOOL "" FORCE)
        set(BUILD_MAIN OFF CACHE BOOL "" FORCE)
        set(BPDPVS_INSTALL OFF CACHE BOOL "" FORCE)
        set(_BPDPVS_STAGED_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/_deps/bp-dpvs-src")
        file(REMOVE_RECURSE "${_BPDPVS_STAGED_SOURCE_DIR}")
        file(COPY "${_BPDPVS_SOURCE_DIR}/" DESTINATION "${_BPDPVS_STAGED_SOURCE_DIR}")
        openfe_patch_bpdpvs_cmake("${_BPDPVS_STAGED_SOURCE_DIR}")
        add_subdirectory("${_BPDPVS_STAGED_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/_deps/bp-dpvs-build")
    else()
        message(STATUS "Local bp-dpvs not found at ${_BPDPVS_SOURCE_DIR}; fetching from ${OPENFE_BPDPVS_GIT_REPOSITORY} (${OPENFE_BPDPVS_GIT_TAG})")
        set(TESTS OFF CACHE BOOL "" FORCE)
        set(BENCH OFF CACHE BOOL "" FORCE)
        set(BUILD_MAIN OFF CACHE BOOL "" FORCE)
        set(BPDPVS_INSTALL OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(
            bpdpvs
            GIT_REPOSITORY ${OPENFE_BPDPVS_GIT_REPOSITORY}
            GIT_TAG ${OPENFE_BPDPVS_GIT_TAG}
        )
        FetchContent_Populate(bpdpvs)
        openfe_patch_bpdpvs_cmake("${bpdpvs_SOURCE_DIR}")
        add_subdirectory("${bpdpvs_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/_deps/bp-dpvs-build")
    endif()
else()
    find_package(bpdpvs CONFIG REQUIRED)
endif()

add_subdirectory(include)
add_subdirectory(src)

add_library(openfe ${SOURCE_FILES})
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

enable_testing()
add_subdirectory(tests)
