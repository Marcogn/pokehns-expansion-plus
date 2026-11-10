#!/usr/bin/env python3
"""Read the symbol table a Hasep ROM carries for companion apps
(include/hasep_symbols.h) and, with --elf, check it against the build.

    tools/hasep_symbols.py pokehns.gba                 # print the table
    tools/hasep_symbols.py pokehns.gba --elf pokehns.elf

The check compares every address entry with the ELF's own symbol (nm), so a
wrong export or a table the parser can't walk fails loudly. Constants
(UPPER_CASE names, or anything that isn't a ROM / RAM address) are printed,
not checked. Exits 1 on any mismatch.
"""
import argparse
import struct
import subprocess
import sys

MAGIC = b"HASEPSYM"
ROM_BASE = 0x08000000
HEADER_SEARCH = 0x1000  # the table sits right after the GF and RHH headers


def read_table(rom):
    at = rom.find(MAGIC, 0, HEADER_SEARCH)
    if at < 0:
        raise SystemExit(f"no {MAGIC.decode()} header in the first {HEADER_SEARCH:#x} bytes")
    fmt, layout, size, _pad, end = struct.unpack_from("<HHHHI", rom, at + 8)
    if fmt != 1:
        raise SystemExit(f"format version {fmt}, this reader knows 1")
    first = at + 20
    last = end - ROM_BASE
    if size < 8 or (last - first) % size:
        raise SystemExit(f"entries from {first:#x} to {last:#x} don't divide into {size}-byte records")
    entries = []
    for off in range(first, last, size):
        raw = rom[off:off + size - 4]
        name = raw.split(b"\0", 1)[0].decode("ascii")
        (value,) = struct.unpack_from("<I", rom, off + size - 4)
        entries.append((name, value))
    return at + ROM_BASE, fmt, layout, entries


def elf_symbols(elf):
    out = subprocess.run(["arm-none-eabi-nm", elf], check=True, capture_output=True, text=True).stdout
    syms = {}
    for line in out.splitlines():
        parts = line.split()
        if len(parts) == 3:
            syms.setdefault(parts[2], set()).add(int(parts[0], 16))
    return syms


def is_address(value):
    return 0x02000000 <= value < 0x02040000 or 0x03000000 <= value < 0x03008000 or ROM_BASE <= value < 0x0A000000


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("rom")
    ap.add_argument("--elf")
    args = ap.parse_args()
    rom = open(args.rom, "rb").read()
    header, fmt, layout, entries = read_table(rom)
    print(f"header at {header:#010x}: format {fmt}, layout {layout}, {len(entries)} entries")
    syms = elf_symbols(args.elf) if args.elf else None
    bad = 0
    names = set()
    for name, value in entries:
        note = ""
        if name in names:
            note, bad = "DUPLICATE", bad + 1
        names.add(name)
        if syms is not None and is_address(value) and not name.isupper():
            base = name.split("[", 1)[0]
            want = syms.get(base, set())
            if base == "gTrainers":  # a row of gTrainers[DIFFICULTY_COUNT][TRAINERS_COUNT]: past its start
                want = {v for a in want for v in range(a, a + 0x40000, 4)}
            # A Thumb function's pointer carries bit 0, as the game's own function pointers do.
            if value not in want and (value & ~1) not in want:
                note, bad = f"MISMATCH (elf: {', '.join(hex(a) for a in sorted(syms.get(base, []))) or 'none'})", bad + 1
            elif note == "":
                note = "ok"
        print(f"  {name:44} {value:#010x} {note}")
    if bad:
        print(f"{bad} bad entries", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
