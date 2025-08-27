import pytest

import enum
import hashlib
import io
import lzma
import tarfile

from age.file import Encryptor
from age.keys.agekey import AgePublicKey
import minisign

import helpers.upload

# HACK
@enum.unique
class KDFAlgorithm(bytes, enum.Enum):
    SCRYPT = minisign.minisign.KDFAlgorithm.SCRYPT
    NOENC = bytes([0x00, 0x00])
minisign.minisign.KDFAlgorithm = KDFAlgorithm

from helpers.http import codes
from helpers.misc import gen_uuid


@pytest.mark.dev_only
async def test_firmware(client, service_data_file):
    def firmware_file_path(name):
        return service_data_file("control", "firmware", name)

    age_pub = "age1jgqvr5pv86k69pesvl2w2cmz79hurd9257kkqevyucfhplhg4sysue7hdn"
    age_key = AgePublicKey.from_public_string(age_pub)
    minisign_priv = """untrusted comment: minisign encrypted secret key
RWQAAEIyAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAyVs1czkgqlkeHrhcFmSb8RNMc1iAwF4JP1RYnSZfyZq18iTpR2sw1M2G7v9z3/1Y9t9nP7cgM/BJ1Pf1uvxadMZacf7KzpfGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA="""
    minisign_key = minisign.SecretKey.from_bytes(bytes(minisign_priv, 'utf-8'))

    file1 = bytes("data1", 'utf-8')
    file2 = bytes("data2", 'utf-8')

    sha = hashlib.sha256()
    sha.update(file1)
    sha.update(file2)
    signature = bytes(minisign_key.sign(sha.digest()))
    print("Files sha:", sha.hexdigest(), "signature:", signature.decode('utf-8'))

    arc = io.BytesIO()
    with tarfile.open(fileobj=arc, mode="w") as tar:
        header = tarfile.TarInfo("file1")
        header.size = len(file1)
        tar.addfile(header, fileobj=io.BytesIO(file1))
        header = tarfile.TarInfo("file2")
        header.size = len(file2)
        tar.addfile(header, fileobj=io.BytesIO(file2))
        header = tarfile.TarInfo("signature")
        header.size = len(signature)
        tar.addfile(header, fileobj=io.BytesIO(signature))

    compressed = lzma.compress(arc.getvalue())
    data = io.BytesIO()
    enc = Encryptor([age_key], data)
    enc.write(compressed)
    enc.close()

    session = gen_uuid()

    content = data.getvalue()
    # print(content)
    print(len(content))
    segments = helpers.upload.split_content(content, 3)
    print(segments)
    status = await client.set_firmware(session, "bytes %d-%d/%d" % (segments[0][0], segments[0][1], len(content)), segments[0][2])
    assert status == codes.CREATED

    status = await client.set_firmware(session, "bytes %d-%d/%d" % (segments[1][0], segments[1][1], len(content)), segments[1][2])
    assert status == codes.CREATED

    status = await client.set_firmware(session, "bytes %d-%d/%d" % (segments[2][0], segments[2][1], len(content)), segments[2][2])
    assert status == codes.OK

    with open(firmware_file_path("file1"), "r") as f:
        assert f.read() == file1.decode('utf-8')
    with open(firmware_file_path("file2"), "r") as f:
        assert f.read() == file2.decode('utf-8')
