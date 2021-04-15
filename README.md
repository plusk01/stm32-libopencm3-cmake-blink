# stm32-libopencm3-cmake-blink

This simple project gives an example of how to use CMake + [`libopencm3`](https://github.com/libopencm3/libopencm3) for stm32 development with C++. A specific example is given for the stm32f765 cpu. The `blink` example comes from libopencm3's [miniblink](https://github.com/libopencm3/libopencm3-miniblink). The `openocd` tool is used for flashing with a stlink-v2.

### Prerequisites

- Install an arm-none-eabi toolchain and add to `PATH` (following [here](https://github.com/plusk01/airdamon_f3/wiki)) -- tested with gcc-arm-none-eabi-10-2020-q4-major
- install `openocd` and setup `udev` rules if needed -- tested with 0.10.0

### Comments

- Tested on [VOXL Flight Core](https://docs.modalai.com/flight-core-datasheet-functional-description/) using a knock-off stlink-v2 programmer
- had to use `reset_config none` when setting up openocd to program this specific board --- make sure RESET pin is connected.
- After getting stlink-v2 programmer connected to voxl flight core (with separate power), run `make blink-stlink-flash` to use openocd to program the board.