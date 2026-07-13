# DSTU 4145

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![CI](https://github.com/shamray/dstu4145/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/shamray/dstu4145/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/shamray/dstu4145/branch/master/graph/badge.svg?token=8iGWXl8Lfu)](https://codecov.io/gh/shamray/dstu4145)

This library implements DSTU 4145-2002 - Ukrainian standard for
eleptic curve cryptography digital signature algorithm. It is
written in C++17. The library is designed to be compact, efficient
and easy to use.

Ця бібліотека реалізує ДСТУ 4145-2002 - стандарт цифрового підпису,
що ґрунтується на еліптичних кривих. Бібліотека написана на C++17.

## Compilation requirements

- Compiler with C++17 support
- CMake >= 3.23
- OpenSSL >= 3 (the only runtime dependency)

## Integrating into your project

The library builds a static target `dstu4145::dstu4145`. Dependencies (just
OpenSSL) are resolved with plain `find_package()`.

### FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(dstu4145
    GIT_REPOSITORY https://github.com/shamray/dstu4145.git
    GIT_TAG        master)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(dstu4145)

target_link_libraries(your_target PRIVATE dstu4145::dstu4145)
```

### Conan

```
[requires]
dstu4145/0.1.0

[generators]
CMakeDeps
CMakeToolchain
```

```cmake
find_package(dstu4145 REQUIRED)
target_link_libraries(your_target PRIVATE dstu4145::dstu4145)
```

### System install

```shell
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
cmake --build build
cmake --install build --prefix /usr/local
```

```cmake
find_package(dstu4145 REQUIRED)
target_link_libraries(your_target PRIVATE dstu4145::dstu4145)
```

## Usage example

```cpp
#include <engine.h>
#include <random>

using namespace dstu4145;

// DSTU 4145-2002 Appendix B test curve (m=163)
auto curve = ecurve{gf2m{163, 7, 6, 3}, 1,
    integer{"5FF6108462A2DC8210AB403925E638A19C1455D21"}};
auto params = domain_params{curve,
    integer{"400000000000000000002BEC12BE2262D39BCF14D"},
    ecurve_point{curve,
        integer{"72D867F93A93AC27DF9FF01AFFE74885C8C540420"},
        integer{"0224A9C3947852B97C5599D5F4AB81122ADC3FD9B"}}};

auto rng = rng_t{[] {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return static_cast<std::byte>(gen());
}};

auto engine = dstu4145::engine{params};
auto [prv_key, pub_key] = engine.generate_key_pair(rng);

auto hash = buffer{/* ... */};
auto signature = engine.sign(rng, prv_key, hash);

assert(engine.verify(pub_key, hash, signature));
```

## Building this repository from source

```shell
pip install conan
conan profile detect --force
conan install . -of build --build=missing -s build_type=Debug
cmake --preset conan-debug
cmake --build --preset conan-debug
ctest --preset conan-debug
```

The acceptance test (`dstu-acceptance-test`, randomized sign/verify rounds)
is registered with CTest under the `acceptance` label:

```shell
ctest --preset conan-debug -L acceptance
```
