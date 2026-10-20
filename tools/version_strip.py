#!/usr/bin/env python3
"""Draw the version shown under PRESS START on the HnS title screen.

    version_strip.py VERSION press_start.4bpp

The version is pixel art, not text: the game has no font for it. It lives in
graphics/title_screen/hns/press_start.png as a strip of 20 tiles (160x8 px)
laid out as sheet row 1 x40-127 followed by sheet row 2 x0-71. Strip column x
is screen x 48+x. This script clears that strip in the .4bpp that gbagfx made
from the PNG, and composes VERSION from the glyph table below, centred on
screen x 116 like the logo (.claude/rules/title_screen.md). It edits the tiles rather
than the PNG so the build needs nothing beyond python3.

Glyphs are 7 rows: outline, five body rows coloured 5, 5, 4, 3, 3 (2 marks a
rounded corner), outline. Neighbouring glyphs share their outline column, so
each glyph overlaps the previous one by one column; a pixel keeps the
strongest of the two (colour > black outline > transparent).

v, 0, 1, 2, 6, '.', P, L, U, S are copied from the hand-made strip of
v2.0.6.PLUS.1; 3, 4, 5, 7, 8, 9 were drawn in the same scheme.
"""
import sys

GLYPHS = {
    # Lowercase prefix; the trailing blank column keeps it off the first digit.
    'v': ['......',
          '......',
          '......',
          '.1.1..',
          '14141.',
          '.131..',
          '..1...'],
    '.': ['...',
          '...',
          '...',
          '...',
          '111',
          '131',
          '111'],
    '0': ['.1111111.',
          '125555521',
          '155111551',
          '1441.1441',
          '133111331',
          '123333321',
          '111111111'],
    '1': ['111111',
          '155551',
          '111551',
          '..1441',
          '..1331',
          '..1331',
          '..1111'],
    '2': ['111111111',
          '155555521',
          '.11111551',
          '124444441',
          '133111111',
          '133333331',
          '111111111'],
    '3': ['111111111',
          '155555521',
          '.11111551',
          '..1444441',
          '.11111331',
          '133333321',
          '111111111'],
    '4': ['1111.1111',
          '1551.1551',
          '1551.1551',
          '144444441',
          '111111331',
          '.....1331',
          '.....1111'],
    '5': ['111111111',
          '155555551',
          '15511111.',
          '144444421',
          '111111331',
          '133333321',
          '111111111'],
    '6': ['.11111111',
          '155555551',
          '15511111.',
          '144444421',
          '132111331',
          '133333321',
          '111111111'],
    '7': ['111111111',
          '155555551',
          '111111551',
          '.....1441',
          '....1331.',
          '....1331.',
          '....1111.'],
    '8': ['.1111111.',
          '125555521',
          '155111551',
          '124444421',
          '133111331',
          '123333321',
          '111111111'],
    '9': ['.1111111.',
          '125555521',
          '155111551',
          '124444441',
          '.11111331',
          '133333321',
          '111111111'],
    'P': ['111111111',
          '155555551',
          '155111551',
          '144111441',
          '133333331',
          '133111111',
          '1111....1'],
    'L': ['1111....1',
          '1551....1',
          '1551....1',
          '1441....1',
          '133111111',
          '133333331',
          '111111111'],
    'U': ['1111.1111',
          '1551.1551',
          '1551.1551',
          '1441.1441',
          '133111331',
          '133333331',
          '111111111'],
    'S': ['111111111',
          '155555551',
          '155111111',
          '144444441',
          '111111331',
          '133333331',
          '111111111'],
}

STRIP_W = 160
STRIP_Y = 0          # glyph row 0 lands on strip row 0; row 7 stays empty
SCREEN_X0 = 48       # screen x of strip column 0
CENTRE_X = 116       # centre of the logo, measured in mGBA


def strength(v):
    return 0 if v == 0 else (1 if v == 1 else 2)


def compose(text):
    cols = []  # each column is a list of 7 palette indices
    for ch in text:
        g = GLYPHS[ch]
        gcols = [[0 if row[x] == '.' else int(row[x], 16) for row in g]
                 for x in range(len(g[0]))]
        if cols:
            last = cols[-1]
            first = gcols[0]
            cols[-1] = [a if strength(a) >= strength(b) else b
                        for a, b in zip(last, first)]
            gcols = gcols[1:]
        cols.extend(gcols)
    # Trim blank edge columns so centring measures the ink.
    while cols and not any(cols[-1]):
        cols.pop()
    while cols and not any(cols[0]):
        cols.pop(0)
    return cols


SHEET_W, SHEET_H = 128, 24


def strip_to_sheet(x, y):
    """Sheet coordinates of strip pixel (x, y)."""
    return (40 + x, 8 + y) if x < 88 else (x - 88, 16 + y)


def put(tiles, x, y, v):
    """Set pixel (x, y) of the sheet; tiles are row-major, 4bpp, low nibble left."""
    sx, sy = strip_to_sheet(x, y)
    i = ((sy // 8) * (SHEET_W // 8) + sx // 8) * 32 + (sy % 8) * 4 + (sx % 8) // 2
    if sx % 2:
        tiles[i] = (tiles[i] & 0x0F) | (v << 4)
    else:
        tiles[i] = (tiles[i] & 0xF0) | v


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    raw, path = sys.argv[1:]
    # Tags are v2.0.6.PLUS.2; the art is uppercase only.
    text = 'v' + raw[1:].upper() if raw[:1] in 'vV' else 'v' + raw.upper()
    bad = sorted({c for c in text if c not in GLYPHS})
    if bad:
        sys.exit(f'version_strip: no glyph for {"".join(bad)!r} in {raw!r} '
                 f'(have: {"".join(sorted(GLYPHS))})')

    cols = compose(text)
    w = len(cols)
    if w > STRIP_W:
        sys.exit(f'version_strip: {text} is {w} px, the strip holds {STRIP_W}')
    # Centred on the logo when it fits; otherwise pushed back inside the strip.
    x0 = CENTRE_X - SCREEN_X0 - w // 2
    x0 = max(0, min(x0, STRIP_W - w))

    with open(path, 'rb') as f:
        tiles = bytearray(f.read())
    if len(tiles) != SHEET_W * SHEET_H // 2:
        sys.exit(f'version_strip: {path} is {len(tiles)} bytes, '
                 f'expected a {SHEET_W}x{SHEET_H} 4bpp sheet')
    for y in range(8):
        for x in range(STRIP_W):
            put(tiles, x, y, 0)
    for i, col in enumerate(cols):
        for y, v in enumerate(col):
            put(tiles, x0 + i, STRIP_Y + y, v)
    with open(path, 'wb') as f:
        f.write(tiles)
    print(f'version_strip: {text}, {w} px, screen x {SCREEN_X0 + x0}-'
          f'{SCREEN_X0 + x0 + w - 1}')


if __name__ == '__main__':
    main()
