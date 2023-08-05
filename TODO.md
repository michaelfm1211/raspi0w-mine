# Todo list for raspi0w-mine

- Create a wireless terminal over Bluetooth.
- Create a program loader.
- Get a real clock working, not just waiting clock cycles.
- Make acronym for MINE.

# Documentation

### Memory Map
Half open interval: includes starting address, not ending address

Physical Address | Size | Purpose
---|---|---
`0x20_000_000` and up | N/A (not in RAM) | BCM 2835 Peripherals
? (`0x14000`) - `0x20_000_000` | Depends (511 MB, 944 KB as of 8-4-23) | Unallocated
`0x8000` - ? (`0x14000`) | Depends (32 KB as of 8-4-23) | Kernel
`0x7C00` - `0x8000` | 1 KB | Interrupt stack
`0x40` - `0x7C00` | 30 KB and 960 B | Kernel stack
`0x0` - `0x40` | 64 B | Exception vector table

