# openfe

This project implements **Functional Encryption** building blocks in C/C++.

The code relies on the [bp-dpvs](https://github.com/acarreo/bp-dpvs.git) library for the required cryptographic primitives, and exposes an `openfe` library plus a `openfe_main` test executable.

## Compilation

### Dependencies

- CMake >= 3.16
- C/C++ compiler (GCC or Clang)
- Git
- GMP development libraries: `libgmp-dev`

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
```

### bp-dpvs handling

The top-level CMake handles `bp-dpvs` automatically:

- If `../bp-dpvs` exists, it is built locally.
- Otherwise, it is fetched automatically from GitHub through `FetchContent`.

Useful CMake variables:

- `OPENFE_USE_LOCAL_BPDPVS=ON|OFF`
- `OPENFE_BPDPVS_GIT_REPOSITORY`
- `OPENFE_BPDPVS_GIT_TAG`

Example to force a specific version/tag:

```bash
cmake -S . -B .build -DOPENFE_BPDPVS_GIT_TAG=v1.2.3
cmake --build .build -j4
```
