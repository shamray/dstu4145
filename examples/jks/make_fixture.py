# Generates testdata/store.jks, the JKS keystore used by the JKS example tests:
# a private key entry "fixture" (DSTU 4145 curve-6 test key, two placeholder
# certificates in its chain) and a trusted certificate entry "ca", both
# protected with the password "testpass".
#
#   pip install pyjks==20.0.0
#   python make_fixture.py
#
# The key protection salt and the entry timestamps are pinned, so the output
# is reproducible byte-for-byte.

import os
import jks
import jks.sun_crypto

PASSWORD = 'testpass'
TIMESTAMP = 1700000000000
PKCS8 = bytes.fromhex(
    '3045020100301E060B2A86240201010101030101300F060D2A86240201010101030101'
    '020604203E5A9B1C0C7973D0F89371D647FF5179DF0FF683010000000000000000000000')
CHAIN = [bytes.fromhex('3003020101'), bytes.fromhex('3003020102')]
CA = bytes.fromhex('3003020103')
SALT = bytes(range(20))


class _FixedSalt:
    @staticmethod
    def urandom(n):
        assert n == len(SALT)
        return SALT


# jks_pkey_encrypt() draws its 20-byte salt from os.urandom
jks.sun_crypto.os = _FixedSalt


def main():
    key = jks.PrivateKeyEntry.new('fixture', CHAIN, PKCS8, key_format='pkcs8')
    key.timestamp = TIMESTAMP
    ca = jks.TrustedCertEntry.new('ca', CA)
    ca.timestamp = TIMESTAMP

    path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'testdata', 'store.jks')
    os.makedirs(os.path.dirname(path), exist_ok=True)
    data = jks.KeyStore.new('jks', [key, ca]).saves(PASSWORD)
    with open(path, 'wb') as f:
        f.write(data)

    # load() checks the trailing store integrity digest
    store = jks.KeyStore.load(path, PASSWORD)
    assert list(store.entries) == ['fixture', 'ca']
    loaded = store.private_keys['fixture']
    loaded.decrypt(PASSWORD)
    assert loaded.pkey_pkcs8 == PKCS8
    assert loaded.timestamp == TIMESTAMP
    assert loaded.cert_chain == [('X.509', c) for c in CHAIN]
    loaded_ca = store.certs['ca']
    assert (loaded_ca.type, loaded_ca.cert, loaded_ca.timestamp) == ('X.509', CA, TIMESTAMP)

    print('%s (%d bytes)' % (path, len(data)))
    print(data.hex())

    # known answer for UTF-16 password encoding of the store digest
    print('empty store, password "\\u043f\\u0430\\u0440\\u043e\\u043b\\u044c\\U0001f511":')
    print(jks.KeyStore.new('jks', []).saves('пароль\U0001f511').hex())


if __name__ == '__main__':
    main()
