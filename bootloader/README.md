# Bootloader

This is the actual bootloader, it has to be flashed using a JLINK or similar and the SWD port.

## Buiding

To build, use make:

```bash
make
```

You will need the [GNU Arm Embedded Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) installed. You can use the GCC_PATH variable if the toolchain is not in your path. The resulting binary that should be written to the board is `build/test1.bin`. It should then be written to the start of flash with a JLINK or similar through the SWD port.