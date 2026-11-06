cmake_minimum_required(VERSION 3.16)
project(openfe_bench LANGUAGES CXX)

find_package(benchmark QUIET)

if(NOT benchmark_FOUND)
    message(STATUS "Google Benchmark not found via find_package; using FetchContent...")
    include(FetchContent)
    FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.8.3
    )
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_EXCEPTIONS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googlebenchmark)
endif()

macro(add_openfe_bench BENCH_NAME BENCH_SRC)
    add_executable(${BENCH_NAME} ${BENCH_SRC})
    target_link_libraries(${BENCH_NAME} PRIVATE OPENFE::openfe benchmark::benchmark)
    target_include_directories(${BENCH_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endmacro()

add_openfe_bench(bench_execution bench-execution.cpp)
add_openfe_bench(bench_serialization bench-serialization.cpp)

add_custom_target(run_benchmarks
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/bench_execution --benchmark_time_unit=ms --benchmark_counters_tabular=true
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/bench_serialization --benchmark_time_unit=us --benchmark_counters_tabular=true
    DEPENDS bench_execution bench_serialization
    COMMENT "Running all OpenFE benchmarks (execution and serialization)"
)
