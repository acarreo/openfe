find_package(GTest QUIET)
if(NOT GTest_FOUND)
    message(STATUS "GoogleTest not found; fetching v1.14.0")
    if(POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
    endif()
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        URL_HASH SHA256=1f357c27ca988c3f7c6b4bf68a9395005ac6761f034046e9dde0896e3aba00e4
    )
    if(WIN32)
        set(gtest_force_shared_crt ON)
    endif()
    FetchContent_MakeAvailable(googletest)
endif()
include(GoogleTest)

if(TARGET GTest::gtest_main)
    set(OPENFE_GTEST_MAIN_TARGET GTest::gtest_main)
else()
    set(OPENFE_GTEST_MAIN_TARGET GTest::Main)
endif()

add_executable(openfe_tests
    relic_env.cpp
    test_abdp15.cpp
    test_als16.cpp
    test_klmmrw16.cpp
)

target_link_libraries(openfe_tests PRIVATE OPENFE::openfe ${OPENFE_GTEST_MAIN_TARGET})

gtest_discover_tests(openfe_tests)
