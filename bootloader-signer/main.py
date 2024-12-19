#!/usr/bin/env python3

from monocypher import generate_signing_key_pair, signature_sign

import argparse
import binascii
import struct

secret, public = generate_signing_key_pair()

root_keys = []
target_keys = []



def patch_binary_payload(bin_filename):
    """
    Patch crc & data_size fields of image_hdr_t in place in binary

    Raise exception if binary is not a supported type
    """
    IMAGE_HDR_SIZE_BYTES = 335
    IMAGE_HDR_MAGIC = 0xCAFE
    IMAGE_HDR_VERSION = 1

    with open(bin_filename, "rb") as f:
        image_hdr = f.read(IMAGE_HDR_SIZE_BYTES)
        data = f.read()

    image_magic = struct.unpack("<H", image_hdr[0:2])[0]
    image_hdr_version = struct.unpack("<H", image_hdr[322:324])[0]

    if image_magic != IMAGE_HDR_MAGIC:
        raise Exception(
            "Unsupported Binary Type."
        )

    if image_hdr_version != IMAGE_HDR_VERSION:
        raise Exception(
            "Unsupported Image Header Version."
        )

    data_size = len(data)
    sig = signature_sign(secret, data)

    print(sig.hex())

    image_hdr_crc_data_size = struct.pack("<64s", sig)
    #print(
    #    "Adding crc:0x{:08x} data_size:{} to '{}'".format(
    #        crc32, data_size, bin_filename
    #    )
    #)
    with open(bin_filename, "r+b") as f:
        # Seek to beginning of "uint32_t crc"
        f.seek(2)
        # Write correct values into crc & data_size
        f.write(image_hdr_crc_data_size)
    print("done")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("bin", action="store")
    args = parser.parse_args()

    patch_binary_payload(args.bin)
