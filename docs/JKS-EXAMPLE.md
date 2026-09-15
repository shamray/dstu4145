# JKS signing example — research results & implementation plan

Status: **researched, not yet implemented** (2026-07-12).
Goal: a real-world example under `examples/jks/` that loads a PrivatBank-style
Java KeyStore (`.jks`) containing a DSTU 4145 private key, signs a file, and
verifies the signature against the certificate from the keystore.

Everything below was verified against a real PrivatBank-issued keystore
(`research/key_22501806_22501806.jks`, **local only — never commit**) and
cross-checked against the reference implementations listed at the end.

---

## 1. Research results

### 1.1 The real keystore, dissected

Parsed with a throwaway script (offsets are from the actual file):

```
magic    = 0xFEEDFEED   version = 2   entries = 1
entry 0  : tag=1 (private key), alias "key_22501806_22501806"
  encrypted key blob : 866 bytes
  certificate chain  : 4 × X.509 DER
    cert 0  leaf, KeyUsage = Key Agreement            (KEP certificate)
    cert 1  leaf, KeyUsage = Digital Signature,
                             Non-Repudiation          (signing certificate)
    cert 2  CA   "КНЕДП АЦСК АТ КБ «ПРИВАТБАНК»"
    cert 3  root "Засвідчувальний центр" (National Bank of Ukraine)
trailing 20 bytes = keystore integrity digest (SHA-1)
```

Notable: the JKS "certificate chain" slot is (ab)used to carry **both** leaf
certificates (signing + key agreement) plus the CA chain. The example must
select the correct certificate by KeyUsage and by matching its public key
against the one derived from the private key.

Signing certificate SPKI (from `openssl asn1parse`):

```
SEQUENCE                                  -- AlgorithmIdentifier
  OID 1.2.804.2.1.1.1.1.3.1.1             -- "DSTU 4145-2002 little endian"
  SEQUENCE                                -- DSTU4145Params
    OID 1.2.804.2.1.1.1.1.3.1.1.2.6       -- named curve: "DSTU curve 6" (m=257)
    OCTET STRING (64 bytes)               -- DKE: packed GOST 28147/34.311 S-box
BIT STRING                                -- wraps DER OCTET STRING (33 bytes)
                                          -- = compressed EC point, little-endian
```

### 1.2 JKS container format

Big-endian binary, no ASN.1 at the container level:

```
u4  magic 0xFEEDFEED
u4  version (2)
u4  entry count
per entry:
  u4  tag                  1 = private key, 2 = trusted cert
  UTF alias                (u2 length + modified-UTF8 bytes)
  u8  timestamp (ms)
  tag 1: u4 len + EncryptedPrivateKeyInfo DER
         u4 chain count, each: UTF cert type ("X.509"), u4 len + DER
  tag 2: UTF cert type, u4 len + DER
u20 keystore integrity digest
     = SHA1( UTF-16BE(password) || "Mighty Aphrodite" || all preceding bytes )
```

The integrity digest allows verifying the store password before attempting
key decryption (useful for good error messages).

### 1.3 Sun private-key protection (OID 1.3.6.1.4.1.42.2.17.1.1)

The encrypted blob is a DER `EncryptedPrivateKeyInfo`:

```
SEQUENCE {
  SEQUENCE { OID 1.3.6.1.4.1.42.2.17.1.1, NULL }
  OCTET STRING {  salt(20) || ciphertext || digest(20)  }
}
```

(In the real file the DER header before the payload is exactly 24 bytes,
matching jksreader's `slice(0x18)`.)

Decryption — a SHA-1 keystream XOR ("encrypt-and-MAC", both prefix-keyed):

```
pw   = UTF-16BE bytes of the password
k(0) = salt                       -- first 20 bytes of payload
k(i) = SHA1( pw || k(i-1) )
keystream  = k(1) || k(2) || ...
plaintext  = ciphertext XOR keystream
valid iff SHA1( pw || plaintext ) == digest   -- last 20 bytes of payload
```

~60 lines of code with OpenSSL's `SHA1()`. Constant-time comparison for the
digest check; wrong password ⇒ digest mismatch.

### 1.4 Decrypted plaintext: PKCS#8 with DSTU OIDs

```
PrivateKeyInfo ::= SEQUENCE {
  version    INTEGER (0),
  algorithm  SEQUENCE {
    OID  1.2.804.2.1.1.1.1.3.1.1,       -- dstu4145le
    DSTU4145Params ::= SEQUENCE {
      curve  CHOICE {
        namedCurve  OID,                 -- 1.2.804.2.1.1.1.1.3.1.1.2.N
        explicit    ECBinary             -- SEQ{ field SEQ{m,k|k1,k2,k3}, a, b, n, base point }
      },
      dke   OCTET STRING OPTIONAL,       -- 64-byte packed S-box
      dke2  OCTET STRING OPTIONAL
    }
  },
  privateKey OCTET STRING,               -- d, LITTLE-ENDIAN
  attributes [0] IMPLICIT ... OPTIONAL   -- may carry the second (KEP) key
}
```

Our blob's plaintext is ~802 bytes — much larger than one named-curve key —
so expect **explicit curve parameters and/or the KEP key in attributes**. The
parser must support both the named-curve and explicit-params branches, and
must pick the *signing* key when two keys are present (match derived public
key against the Digital Signature certificate).

### 1.5 Named curves (OID arc 1.2.804.2.1.1.1.1.3.1.1.2.N)

| N | m   | basis            | N | m   | basis            |
|---|-----|------------------|---|-----|------------------|
| 0 | 163 | x⁷+x⁶+x³ penta   | 5 | 233 | trinomial        |
| 1 | 167 |                  | 6 | 257 | x¹²  trinomial   |
| 2 | 173 |                  | 7 | 307 |                  |
| 3 | 179 |                  | 8 | 367 |                  |
| 4 | 191 |                  | 9 | 431 |                  |

Constants (a, b, n, base point) come from DSTU 4145-2002 **Appendix G**;
cross-check transcription against `jkurwa`'s `lib/curves.js` and
`dstu-engine`. Curve 0 (m=163) is already exercised by our unit tests; the
real keystore needs **curve 6 (m=257, trinomial x²⁵⁷+x¹²+1)**. Ship the whole
table in the library (`dstu4145::standard_curve(curve_id)`), not the example —
this is Phase 2 / M1 and generally useful.

### 1.6 Endianness and wire formats (the classic trap)

Ukrainian PKI uses the **little-endian** flavor of DSTU 4145
(the `…3.1.1` OID; big-endian has its own arc `…3.1.1.1.1`):

- private key `d` in PKCS#8: little-endian octets;
- public key in SPKI `BIT STRING`: DER `OCTET STRING` containing the
  **compressed point, little-endian**, fixed length ⌈m/8⌉;
- signature value: DER `OCTET STRING` containing
  `r_LE || s_LE`, each zero-padded to the same fixed length
  (verified against jkurwa `short_sign`: `0x04, len, reverse(r), reverse(s)`);
- our library's `integer`/`to_buffer` is big-endian ⇒ every boundary needs an
  explicit byte reversal. Add `to_buffer_le`/`from_buffer_le` helpers to the
  library rather than sprinkling `std::reverse` through the example.

### 1.7 Hash: GOST 34.311-95

Real Ukrainian signatures (and the certificates themselves) hash with
**GOST 34.311-95** (OID `1.2.804.2.1.1.1.1.2.1`), *not* SHA-x. OpenSSL does
not ship it. The 64-byte `dke` OCTET STRING in `DSTU4145Params` is the packed
GOST 28147-89 S-box parameterizing the hash; when absent, the standard DSTU
S-box (СБ №1) applies.

Plan: implement GOST 34.311-95 (+ the underlying GOST 28147-89 block cipher,
ECB-encrypt only) in the example tree, with the S-box as a parameter.
`dstu-engine` (Apache-2.0, C) is a clean reference; port, don't link — it's
an OpenSSL *engine* (deprecated machinery in OpenSSL 3) and a heavyweight
dependency. Scope: ~300 lines + test vectors from the GOST spec and from
`dstu-engine`'s test suite.

### 1.8 Reference implementations

| project | language | license | useful for |
|---|---|---|---|
| [dstucrypt/jkurwa](https://github.com/dstucrypt/jkurwa) | JS | — | ASN.1 schemas, curves table, sign format |
| [dstucrypt/jksreader](https://github.com/dstucrypt/jksreader) | JS | — | JKS + Sun decryption, PrivatBank quirks |
| [dstucrypt/dstu-engine](https://github.com/dstucrypt/dstu-engine) | C | Apache-2.0 | GOST 34.311/28147, keylib (JKS, Key-6.dat), test vectors |
| [pyjks](https://pyjks.readthedocs.io/en/latest/jks.html) | Python | MIT | JKS format details; **test fixture generation** |
| [Neil Madden — Java KeyStores: the gory details](https://neilmadden.blog/2017/11/17/java-keystores-the-gory-details/) | — | — | protection algorithm write-up |
| BouncyCastle | Java | MIT-like | has full DSTU 4145 support — alternative fixture generator |

---

## 2. Implementation plan

Layout (example code stays out of the installed library; M1 lands in the
library proper):

```
examples/jks/
  jks/            jks_reader.{h,cpp}     container + Sun key protection
  asn1/           dstu_asn1.{h,cpp}      PKCS#8, DSTU4145Params, SPKI, cert (via OpenSSL d2i/ASN1)
  gost/           gost34311.{h,cpp}      hash + gost28147.{h,cpp} (ECB only)
  cli/            main.cpp               dstu-jks-sign
  tests/          fixtures + gtest
```

### M1 — library prerequisites (src/)
Standard curves table (`standard_curve(curve_id::uacurve6)` etc.),
little-endian buffer helpers, fixed-length serialization. Unit tests: each
curve's base point validates and `n·P = O`.

### M2 — JKS reader
Container parser (§1.2) + Sun decryption (§1.3) + store-integrity check.
No dependency on the crypto core; testable standalone. Errors: bad magic,
truncated file, wrong password (distinguish store password vs key password —
PrivatBank uses the same for both).

### M3 — ASN.1 layer
Parse `EncryptedPrivateKeyInfo`, PKCS#8 (§1.4), named + explicit curve
params, SPKI, and the certificate enough to extract SPKI + KeyUsage
(OpenSSL `X509` API handles the outer certificate; the DSTU-specific SPKI
payload is parsed manually — OpenSSL errors on it, see §1.1). Expand the
compressed public key point with `ecurve::expand_point` (must be hardened
first — see ROADMAP Phase 3 item; minimal fix can land with this milestone).
Select signing key/cert by KeyUsage + public-key match.

### M4 — GOST 34.311-95
As §1.7. Test vectors first (from the spec + dstu-engine), then implementation.

### M5 — CLI + end-to-end
```
dstu-jks-sign info   <store.jks> [-p pass]        # aliases, certs, curve
dstu-jks-sign sign   <store.jks> -p pass <file>   # -> file.sig (OCTET STRING r||s LE)
dstu-jks-sign verify <store.jks> <file> <file.sig>
```
Acceptance: round-trip sign/verify in CI; `verify` interops with a signature
produced by jkurwa over the same fixture key (cross-implementation check).

### Test fixtures — do NOT use the real key
Generate a committed fixture keystore with a known password (`testpass`):
either **pyjks** (write a JKS entry wrapping a PKCS#8 blob we build ourselves
around a curve-0 test key) or **BouncyCastle + keytool** (Java, full DSTU
support). Keep the real bank keystore strictly local (`research/` is
git-ignored) — optionally a manual smoke-test script that takes store path +
password from the environment.

Effort estimate: M1 ≈ 1 day, M2 ≈ 1 day, M3 ≈ 2–3 days (the fiddly part),
M4 ≈ 1–2 days, M5 ≈ 1 day, plus fixture tooling ≈ 1 day.

## 3. Security notes

- `research/key_22501806_22501806.jks` is a **live personal bank key**
  (cert valid until 2026-08-08). Git-ignored; never commit, never upload.
- Zeroize password bytes, keystream, decrypted PKCS#8, and `d` after use
  (`OPENSSL_cleanse`).
- The Sun protection scheme is weak by modern standards (unsalted-chain SHA-1
  keystream); the example should print a note recommending migration, not
  pretend this is good cryptography.
- Wrong-password and corrupted-store paths must fail cleanly (no throws
  escaping `main`, no partial writes of `.sig` files).
