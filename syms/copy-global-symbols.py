from os import popen
from sys import argv

symbols = popen(f'readelf -s {argv[1]} | grep GLOBAL | rev | cut -d" " -f1 | rev').read().splitlines()
addrs = popen(f'readelf -s {argv[1]} | grep GLOBAL | cut -d" " -f6').read().splitlines()

with open(argv[2], 'w') as f:
    for (symbol, addr) in zip(symbols, addrs):
        f.write(f'.global {symbol}\n')
        f.write(f'.set {symbol}, 0x{addr}\n')
