#!/usr/bin/env python3

from monocypher import generate_signing_key_pair

import struct

root_keys = []
target_keys = []

ROOT_THRESHHOLD = 3
TARGET_THRESHHOLD = 1

EXPIRE_MAJOR = 2
EXPIRE_MINOR = 0
EXPIRE_PATCH = 0

for i in range(5):
    root_keys.append(generate_signing_key_pair())
    with open('root' + str(i) + '.key', 'wb') as f:
        f.write(root_keys[i][0])

for i in range(3):
    target_keys.append(generate_signing_key_pair())
    with open('target' + str(i) + '.key', 'wb') as f:
        f.write(target_keys[i][0])

root_metadata = struct.pack("<32s32s32s32s32sB32s32s32sBBBB", root_keys[0][1], root_keys[1][1], root_keys[2][1], root_keys[3][1], root_keys[4][1], ROOT_THRESHHOLD, target_keys[0][1], target_keys[1][1], target_keys[2][1], TARGET_THRESHHOLD, EXPIRE_MAJOR, EXPIRE_MINOR, EXPIRE_PATCH)

with open('root.bin', 'wb') as f:
    f.write(root_metadata)
