# Roadmap

Goal: make dstu4145 a **usable, easy-to-integrate utility library** for other C++
projects, with proper packaging, a real-world example (JKS-based signing), and a
hardened crypto core.

Priority order (agreed 2026-07-12):

1. **Phase 1 — Build & packaging modernization**
2. **Phase 2 — JKS real-world example** (design: [JKS-EXAMPLE.md](JKS-EXAMPLE.md))
3. Phase 3 — Correctness & security hardening
4. Phase 4 — API & usability
5. Phase 5 — Release & distribution

---

## Phase 1 — Build & packaging modernization

The current build only works with Conan 1.x (`conan.cmake` v0.16.1 /
`conan_cmake_run` are incompatible with Conan 2), install rules are broken, and
CI (Travis) is dead. A consumer cannot integrate the library today.

**Status (2026-07-13): implemented and locally verified.** All checklist items
below are done except the OpenSSL-pimpl item (deliberately deferred). All
three consumption modes in the Definition of Done were built and exercised
locally on Windows/MSVC. GitHub Actions workflow ran green on 2026-07-14
(run "Fix CI", commit b0b196d), covering ubuntu (gcc, clang), windows
(MSVC), and macos (apple-clang).

- [x] **Restructure CMake** (`CMakeLists.txt`, `src/CMakeLists.txt`, `test/CMakeLists.txt`)
  - `cmake_minimum_required(VERSION 3.23)`, project version, `CMAKE_CXX_STANDARD 17`.
  - Remove the embedded `conan.cmake` download and `conan_cmake_run` from the
    consumer-facing build. Dependencies are found with plain `find_package()`;
    how they get there (Conan, vcpkg, system) is the consumer's choice.
  - Public headers via `target_sources(... FILE_SET HEADERS ...)` so
    `install(TARGETS)` installs them (today `install(DIRECTORY include/ ...)`
    references a directory that does not exist — headers are never installed).
  - Fix the export: namespace `dstu4145::dstu4145`, config +
    `write_basic_package_version_file`, install to
    `${CMAKE_INSTALL_LIBDIR}/cmake/dstu4145` (currently exports to
    `share/MyLibrary/cmake` — copy-paste leftover).
  - Gate tests behind `BUILD_TESTING` (CTest convention) so
    `add_subdirectory`/FetchContent consumers don't build gtest.
  - Register `dstu-acceptance-test` with CTest (label it `acceptance`).
- [x] **Drop Boost** — it is a heavyweight *public* dependency used only for:
  - the inactive `bmp::integer` backend (already bit-rotted: uses
    `boost::mpl` internals removed from modern Boost.Multiprecision);
  - the inactive `in::polynomial` backend;
  - `boost::equality_comparable` (trivially replaced by hand-written `!=`).
  Keep only OpenSSL. Delete or archive the inactive backends (git history
  preserves them). This is the single biggest packaging win.
- [ ] **Stop leaking `<openssl/bn.h>` from public headers** — move the BIGNUM
  representation behind an opaque struct (pimpl) in `integer.h` so consumers
  need OpenSSL only at link time. (Can slip to Phase 4 if it blocks Phase 2.)
- [x] **Conan 2 recipe** — `conanfile.py` (not `.txt`) with `CMakeToolchain` /
  `CMakeDeps`, `openssl/[>=3 <4]` requirement, package_info with
  `cpp_info.set_property("cmake_target_name", "dstu4145::dstu4145")`.
  Verify `conan create .` works on Linux + Windows. (Verified on Windows/MSVC
  locally. CI builds on Linux/macOS via `conan install` + CMake presets, but
  does not run `conan create .` — still unverified there.)
- [x] **GitHub Actions CI** (replace dead Travis; delete `.travis.yml`)
  - Matrix: ubuntu (gcc, clang), windows (MSVC), macos (apple-clang).
  - One job with `-fsanitize=address,undefined`.
  - Coverage job uploading to codecov (keep existing token/badge).
  - A "consumer smoke test" job: FetchContent a fresh checkout and compile a
    5-line program calling `engine::verify` — proves integration works.
  (Verified on GitHub Actions, commit b0b196d.)
- [x] **README rewrite** — fix broken build instructions (`cmake -- build .`
  typo), replace Travis badge with Actions badge, add an integration section
  (FetchContent, Conan), minimal usage example. Closes GitHub issue #2.
- [x] `.gitignore`: add `research/` (contains a real bank-issued key that must
  never be committed — done as part of this planning commit).

**Definition of done:** a third-party project can consume the library three
ways without touching its internals: `add_subdirectory`/FetchContent,
`conan create` + `find_package`, plain `cmake --install` + `find_package`.

## Phase 2 — JKS real-world example

Full research + design in [JKS-EXAMPLE.md](JKS-EXAMPLE.md). Summary of
milestones (details there):

- [ ] **M1** Standard curves table (Appendix G of the standard; at minimum
  curves 0/163 and 6/257) + little-endian import/export helpers — prerequisites
  living in the library proper, not the example.
  (Curves done: `standard_curve(curve_id::uacurve0..9)` in
  `src/standard_curves.h`, all 10 curves, table cross-checked against
  dstu-engine and jkurwa; each curve validated by `domain_params`, covered by
  `appendix_g/standard_curves.*`. Little-endian / fixed-length helpers
  deferred to M3, where the parser defines their shape.)
- [ ] **M2** `examples/jks/jks_reader` — JKS container parser + Sun key
  protection decryption (SHA-1 keystream XOR + integrity digest).
- [ ] **M3** ASN.1 layer (OpenSSL `d2i_*`-based): PKCS#8 with DSTU OIDs,
  `DSTU4145Params` (named curve / explicit params), certificate SPKI,
  compressed point expansion, LE quirks.
- [ ] **M4** GOST 34.311-95 hash with configurable S-box (required for
  interop with Ukrainian PKI; certificates carry the S-box as a 64-byte DKE).
- [ ] **M5** `dstu-jks-sign` CLI: `sign` / `verify` / `info` commands; test
  fixture keystore with a known password; CI-tested end-to-end.

## Phase 3 — Correctness & security hardening

Found during the 2026-07-12 code review. Ordered by severity.

- [x] `engine::verify` must never throw on malformed input — return `false`.
  (Fixed in commit 319b89d; verify now rejects empty/odd-length signatures
  and zero r/s before multiply.)
- [x] `engine::verify`: enforce `0 < r < n`, `0 < s < n`.
  (Fixed in commit 319b89d.)
- [x] Replace standard-mandated `assert`s with real logic (they vanish in
  release builds), per DSTU 4145: if hash reduces to 0 use 1; if `r == 0` or
  `s == 0` during signing, regenerate the nonce and retry. (Fixed in
  commit 319b89d: hash reducing to 0 now uses 1 via hash_element; r == 0
  or s == 0 makes try_sign return nullopt and sign(rng, ...) retries with
  a fresh nonce. The ci.yml now runs the `acceptance` label in a separate
  Release job.)
- [x] `ecurve::expand_point` returns `infinity_point()` on three invalid-input
  paths instead of failing (`src/ecurve.cpp:71-81`); public key
  deserialization must reject: not-on-curve, infinity, wrong order.
  (Fixed: `expand_point` returns `std::optional`, `nullopt` when no point
  exists; `x³ + ax² + b == 0` was a valid point, not an error, and now
  decodes to `(x, 0)` / `(x, x)`. `public_key(params, buffer)` throws on
  `nullopt`, infinity, and `Q·n != O`; covered by the `dstu257.public_key_*`
  tests.)
- [x] `gf2m::operator==` compares only `m`, ignoring the reduction polynomial
  (`src/gf2m.cpp:30`) — fields GF(2^m) with different bases compare equal.
  (Fixed: now compares `basis()` too; covered by
  `gf2m_test.comparison_in_fields_with_different_basis`.)
- [x] `domain_params` constructor validates with `assert` only
  (`src/domain_params.h:20`). (Fixed: throws `std::runtime_error` when the
  base point is off the curve, is the point at infinity, or `p·n != O`;
  covered by the `dstu257.domain_params_*` tests.)
- [ ] Non-canonical zero: `vec::polynomial::operator+` can produce a non-empty
  all-zero vector; `operator integer()` / `operator<<` then throw via `msb()`
  (`src/polynomial.cpp:343`). Canonicalize in `operator+` or make `msb()`
  well-defined for zero.
- [x] `1 << 31` on `int` in `bit_set` / `bit_unset` / `bit_test` /
  `left_shift` (`src/polynomial.cpp:120,195,208,230`) — use `1u`. (Fixed.
  Not actually UB in C++17 — well-defined since C++14, CWG 1457 — but `1u`
  matches the `unsigned` chunk type.)
- [ ] Zeroize secrets on destruction (`private_key`, nonce `e`, decrypted
  buffers) with `OPENSSL_cleanse`.
- [ ] Meaningful exception types (`dstu4145::error` hierarchy) instead of a
  dozen `std::runtime_error("error")` in `src/integer.cpp`.
- [ ] Add negative tests: malformed signatures, tampered keys, zero scalars,
  cross-field element mixing.

## Phase 4 — API & usability

- [ ] Built-in secure RNG (`dstu4145::secure_rng()` wrapping `RAND_bytes`) so
  users are not pushed toward `std::mt19937` (which the acceptance test
  currently models).
- [ ] Deterministic, fixed-length serialization for keys and signatures
  (length derived from field/order size, not from the value's leading zeros —
  today `private_key::to_buffer` and `public_key::to_buffer` are
  variable-length).
- [ ] Batch RNG interface (`rng_t` is one virtual call per *byte*).
- [ ] Performance pass (matches existing TODO.md): cache `BN_CTX`, fast
  `msb`/`lsb` (`BN_num_bits`), stop copying `gf2m`/`ecurve` into every
  element/point, Karatsuba / windowed multiplication.
- [ ] Doxygen or header comments for the public API; usage documentation.

## Phase 5 — Release & distribution

- [ ] **License decision**: GPL-3.0 blocks adoption in closed-source projects.
  If wide adoption is the goal and all code is original, relicense
  (MIT / Apache-2.0), or dual-license.
- [ ] Tag `v0.1.0`, GitHub release with notes; semantic versioning from then on.
- [ ] Submit recipe to ConanCenter; add vcpkg port.
- [ ] Announce / document (wiki entry from TODO.md).
