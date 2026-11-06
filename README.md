# openfe

This project implements **Functional Encryption** building blocks in C/C++.

The code relies on the [bp-dpvs](https://github.com/acarreo/bp-dpvs.git) library for the required cryptographic primitives, and exposes an `openfe` library plus a `openfe_main` test executable.

## Compilation

### Dependencies

- CMake >= 3.16
- C/C++ compiler with C++20 support (GCC or Clang)
- Git and a build tool such as Make
- GMP development libraries: `libgmp-dev`
- Network access to GitHub when dependencies are not already installed

On Ubuntu/Debian (if needed):

```bash
sudo apt update
sudo apt install -y cmake g++ git libgmp-dev
```

### Standard build

From the project root:

```bash
cmake -S . -B .build
cmake --build .build -j4
ctest --test-dir .build --output-on-failure
```

Tests are enabled by default. GoogleTest is used if installed (for example
with `sudo apt install libgtest-dev`); otherwise version 1.14.0 is downloaded
and built in the build directory. To build without tests or GoogleTest:

```bash
cmake -S . -B .build -DBUILD_TESTING=OFF
cmake --build .build -j4
```

### Benchmarks

Benchmarks are disabled by default. To enable them, configure with
`-DOPENFE_BUILD_BENCH=ON`; Google Benchmark is used if installed or fetched
automatically otherwise.

```bash
cmake -S . -B .build -DOPENFE_BUILD_BENCH=ON
cmake --build .build -j4
cmake --build .build --target run_benchmarks
```

### bp-dpvs handling

CMake first searches for an installed `bpdpvs` CMake package in standard
prefixes such as `/usr` and `/usr/local`. Its dependencies, RELIC and GMP,
must also be available. For other installation prefixes, set `CMAKE_PREFIX_PATH`
or `bpdpvs_DIR`.

If no package is found, `bp-dpvs` is downloaded and built in the build
directory, without installing it system-wide. When RELIC is not installed,
`bp-dpvs` also downloads and builds it. Its default RELIC preset targets x64.

The fallback uses a fixed `bp-dpvs` commit by default. These variables can
override it when fetching:

- `OPENFE_BPDPVS_GIT_REPOSITORY`
- `OPENFE_BPDPVS_GIT_TAG` (a tag, branch, or commit)

Example:

```bash
cmake -S . -B .build -DOPENFE_BPDPVS_GIT_TAG=main
cmake --build .build -j4
```

CMake caches dependency locations and options. When changing how dependencies
are resolved, configure a fresh build directory or clear the relevant cache
entries.
