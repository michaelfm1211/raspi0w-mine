# raspi0w-mine: Michael's Interactive Network Enabled bare metal system

A simple bare metal application for the Raspberry Pi Zero W (not RPI Zero 2).
Currently boots up and drops you into a kind of interactive terminal.

### Project structure

- `docs/` - Contains relevant documentation.
- `drivers/` - Contains interfaces for interacting with peripherals.
- `firmware/` - Contains necessary firmware for setting up peripherals.
- `kernel/` - Contains higher level abstractions and applications code.
- `lib/` - Contains functions that would be in the C standard library.
- `syms/` - Contains tools for extracting a symbol table from the kernel. May be \
may be used for a future program loader. Don't worry about this now.
- `boot.S` - Main entry point. Calls `kernel/kmain.c`

### config.txt

To run this on a real machine, you'll need to use the Raspberry Pi firmware
from [the official repo](https://github.com/raspberrypi/firmware/). This is not
included in the `firmware/` directory. You may also need to modify your
`config.txt` file. This is what your `config.txt` should look like to run
raspi0w-mine:
```
# this file is intentionally left blank
```
