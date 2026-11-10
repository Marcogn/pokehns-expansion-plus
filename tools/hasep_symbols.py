#!/usr/bin/env python3
"""Read the symbol table a Hasep ROM carries for companion apps
(include/hasep_symbols.h) and, with --elf, check it against the build.

    tools/hasep_symbols.py pokehns.gba                 # print the table
    tools/hasep_symbols.py pokehns.gba --elf pokehns.elf

The check compares every address entry with the ELF's own symbol (nm), so a
wrong export or a table the parser can't walk fails loudly: an empty table, a
missing core name, a zero address, a duplicate. Constants (UPPER_CASE names)
are printed; only those the gTrainers row is checked with are used. Bad
entries also go to stderr (CI keeps stdout quiet). Exits 1 on any of them.
"""
import argparse
import struct
import subprocess
import sys

MAGIC = b"HASEPSYM"
ROM_BASE = 0x08000000
HEADER_SEARCH = 0x1000  # the table sits right after the GF and RHH headers
# Names the PokeDaisy fork can't work without: a table missing them is broken.
REQUIRED = ("gPlayerParty", "gPlayerPartyCount", "gSaveBlock1Ptr", "gSaveBlock2Ptr", "gMain", "gSpeciesInfo")
TRAINER_SIZE = 0x34  # sizeof(struct Trainer), asserted in src/hasep_symbols.c


def read_table(rom):
    at = rom.find(MAGIC, 0, HEADER_SEARCH)
    if at < 0:
        raise SystemExit(f"no {MAGIC.decode()} header in the first {HEADER_SEARCH:#x} bytes")
    fmt, layout, size, _pad, end = struct.unpack_from("<HHHHI", rom, at + 8)
    if fmt != 1:
        raise SystemExit(f"format version {fmt}, this reader knows 1")
    first = at + 20
    last = end - ROM_BASE
    if size < 8 or last <= first or (last - first) % size:
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
            # An LTO build (RELEASE=1) renames statics: sLockFieldControls.lto_priv.0.
            name = parts[2].split(".", 1)[0]
            syms.setdefault(name, set()).add(int(parts[0], 16))
    return syms


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("rom")
    ap.add_argument("--elf")
    args = ap.parse_args()
    rom = open(args.rom, "rb").read()
    header, fmt, layout, entries = read_table(rom)
    print(f"header at {header:#010x}: format {fmt}, layout {layout}, {len(entries)} entries")
    syms = elf_symbols(args.elf) if args.elf else None
    bad = []
    names = set()
    table = dict(entries)
    for name, value in entries:
        note = ""
        if name in names:
            note = "DUPLICATE"
        names.add(name)
        if not note and not name.isupper() and value == 0:
            note = "ZERO ADDRESS"
        if not note and syms is not None and not name.isupper():
            base = name.split("[", 1)[0]
            want = syms.get(base, set())
            if base == "gTrainers":  # gTrainers[DIFFICULTY_NORMAL]: that row's start, exactly
                row = table.get("DIFFICULTY_NORMAL", 0) * table.get("TRAINERS_COUNT", 0) * TRAINER_SIZE
                want = {a + row for a in want} if "TRAINERS_COUNT" in table else set()
            # A Thumb function's pointer carries bit 0, as the game's own function pointers do.
            if value not in want and (value & ~1) not in want:
                note = f"MISMATCH (elf: {', '.join(hex(a) for a in sorted(syms.get(base, []))) or 'none'})"
            else:
                note = "ok"
        line = f"  {name:44} {value:#010x} {note}"
        print(line)
        if note and note != "ok":
            bad.append(line)
    if not entries:
        bad.append("  the table has no entries")
    bad += [f"  {name}: MISSING" for name in REQUIRED if name not in table]
    if bad:
        print(f"{len(bad)} bad entries:", file=sys.stderr)
        for line in bad:
            print(line, file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
