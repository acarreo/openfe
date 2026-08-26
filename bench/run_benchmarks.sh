#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/../.build"
RESULTS_DIR="${SCRIPT_DIR}/results"

mkdir -p "${RESULTS_DIR}"

echo "=========================================="
echo " Running OpenFE Benchmarks"
echo "=========================================="

if [ -f "${BUILD_DIR}/bench/bench_execution" ] && [ -f "${BUILD_DIR}/bench/bench_serialization" ]; then
    echo "Running execution benchmark suite..."
    "${BUILD_DIR}/bench/bench_execution" \
        --benchmark_out="${RESULTS_DIR}/execution_results.json" \
        --benchmark_out_format=json \
        --benchmark_time_unit=ms

    echo "Running serialization benchmark suite..."
    "${BUILD_DIR}/bench/bench_serialization" \
        --benchmark_out="${RESULTS_DIR}/serialization_results.json" \
        --benchmark_out_format=json \
        --benchmark_time_unit=ms

    echo "Results saved in ${RESULTS_DIR}/"
else
    echo "Benchmark executables not found in ${BUILD_DIR}/bench/. Please build the project first."
    exit 1
fi
