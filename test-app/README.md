# Test App

This is a hello world application that can be used to test the bootloaders ability to update an app.

## Buiding

To build, use make:

```bash
make
```

You will need the [GNU Arm Embedded Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) installed. You can use the GCC_PATH variable if the toolchain is not in your path. The resulting binary that should be written to the SD card is `build/test1.bin`. It should be renamed to `firmware.bin`, however.