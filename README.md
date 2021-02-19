# DSTU 4145

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Build Status](https://travis-ci.com/shamray/dstu4145.svg?branch=master)](https://travis-ci.com/shamray/dstu4145)

This library implements DSTU 4145-2002 - Ukrainian standard for
eleptic curve cryptography digital signature algorithm. It is
written in C++17. The library is designed to be compact, efficient
and easy to use.

Ця бібліотека реалізує ДСТУ 4145-2002 - стандарт цифрового підпису,
що ґрунтується на еліптичних кривих. Бібліотека написана на C++17.

## Compilation requirements
* Compiler with C++17 support
* CMake
* Conan

## How to build

The library uses Conan for installing its dependencies and builds
with CMake. 

```shell
mkdir build && cd build
cmake .. 
cmake -- build .
```

