find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(openfe_tests
    relic_env.cpp
    test_abdp15.cpp
    test_als16.cpp
    test_klmmrw16.cpp
)

target_link_libraries(openfe_tests PRIVATE OPENFE::openfe GTest::gtest GTest::gtest_main)

gtest_discover_tests(openfe_tests)
