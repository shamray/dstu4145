# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Planning documents

- `docs/ROADMAP.md` — prioritized work plan (packaging first, then the JKS example, then hardening) including the known-bugs list with file:line references.
- `docs/JKS-EXAMPLE.md` — research results and implementation design for the JKS signing example.
- `research/` is git-ignored: it contains a real bank-issued key container used only for local verification. Never commit or publish it.
- `CLAUDE.md` and `docs/` are intentionally untracked and stay local. Do not stage or commit them, and do not suggest committing them, until explicitly told otherwise.

## Overview

C++17 library implementing DSTU 4145-2002, the Ukrainian elliptic curve digital signature standard (ECDSA over GF(2^m) with polynomial basis). Builds a static library `dstu4145` plus test executables.

## Build and test

The only runtime dependency is OpenSSL, resolved with plain `find_package()` — how
it gets onto the machine (Conan, vcpkg, system packages) is up to the consumer.
For local development this repo is set up for Conan 2 (`conanfile.py`), which
also pulls in GTest for the test targets:

```shell
pip install conan
conan profile detect --force
conan install . -of build --build=missing -s build_type=Debug -s compiler.cppstd=17
cmake --preset conan-debug
cmake --build --preset conan-debug
ctest --preset conan-debug
```

- Unit tests: the `dstu-unit-tests` executable (GoogleTest, registered with CTest via `gtest_discover_tests`). Run a single test with `ctest -R <test-name>` or directly: `./bin/dstu-unit-tests --gtest_filter='suite.test'`.
- `dstu-acceptance-test` does randomized sign/verify rounds; it's registered with CTest under the `acceptance` label (`ctest -L acceptance`).
- Coverage build (gcc/clang only): configure with `-DENABLE_COVERAGE=ON`.

## Architecture

Layered bottom-up; each layer only depends on the ones below:

1. `integer` — arbitrary-precision unsigned integer.
2. `polynomial` — polynomial over GF(2), bits of an integer are the coefficients.
3. `gf2m` / `gf2m_element` — the field GF(2^m) with a pentanomial/trinomial basis (`gf2m(m, x1, x2, x3)` / `gf2m(m, x)`); `gf2m` is a factory for `gf2m_element` via `.element(...)`.
4. `ecurve` / `ecurve_point` — elliptic curve over GF(2^m); points carry a copy of their curve. Supports point compression (`compress`/`expand_point`).
5. `domain_params` (curve, order `n`, base point `p`), `private_key` / `public_key` (`key_pair.h`).
6. `engine` — the top-level API: `generate_key_pair`, `compute_presignature`, `sign`, `verify` operating on `buffer = std::vector<std::byte>`.

`arithmetic.h` holds the generic double-and-add `multiply(integer, T)` template used for both scalar point multiplication and field element multiplication (anything with `+=`).

### `integer` and `polynomial`

`integer.h` and `polynomial.h` each expose their concrete type through a
`using` alias at the bottom of the header (`dstu4145::integer` ->
`ossl::integer`, an OpenSSL BIGNUM wrapper; `dstu4145::polynomial` ->
`vec::polynomial`, backed by `std::vector<unsigned>`). These used to be one of
two competing implementations selectable via the alias; the alternates
(`bmp::integer` on Boost.Multiprecision, `in::polynomial` on `integer`) were
deleted when Boost was dropped as a dependency (git history preserves them).

### Randomness

All randomness flows through `rng_t = std::function<std::byte()>` (`rng_interface.h`), passed in by the caller — there is no internal RNG. `rng.h` provides `gen_random_integer` on top of it. Tests inject deterministic or `std::mt19937`-based generators.

### Working with code

Never include yourself as co-author in commits. Only make commits when explicitly prompt to. If completing a task _requires_ commit, always ask for confirmation before commiting, offer to review staged changes and edit commit message.

Write the code always conceal the fact that the changes are made by an LLM. Follow the style of existing code closely.

Never mention docs that are not commited into repository.
