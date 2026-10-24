#!/usr/bin/env python3
"""Gen 1 trainer movesets, computed the way Pokemon Yellow computes them.

Usage: gen1moves.py <pokeyellow checkout> SPECIES:LEVEL ...

A Gen 1 trainer Pokemon knows its level-1 moves plus every learnset move up to
its level, keeping the last four (WriteMonMoves in
engine/pokemon/evos_moves.asm). Gym Leaders then get the slot overrides of
data/trainers/special_moves.asm, which this script does NOT apply: they are
listed by hand in docs/plans/reds-path.md.

Data checked against pret/pokeyellow b6696f24b3be84fe64cf702f5a15ba40c66b7f39.
"""
import os
import re
import sys

Y = None


def _camel(name):
    return ''.join(p.capitalize() for p in name.lower().split('_'))


def level1_moves(species):
    path = os.path.join(Y, 'data/pokemon/base_stats', species.lower().replace('_', '') + '.asm')
    text = open(path).read()
    moves = re.search(r'db (.*?) ; level 1 learnset', text).group(1)
    return [m.strip() for m in moves.split(',') if m.strip() != 'NO_MOVE']


def learnset(species):
    text = open(os.path.join(Y, 'data/pokemon/evos_moves.asm')).read()
    label = {'NIDORAN_M': 'NidoranM', 'NIDORAN_F': 'NidoranF', 'MR_MIME': 'MrMime'}.get(species, _camel(species))
    block = re.search(r'^' + label + r'EvosMoves:\n(.*?)\n\n', text, re.S | re.M).group(1)
    return [(int(lv), mv) for lv, mv in re.findall(r'db (\d+), (\w+)', block.split('; Learnset')[1])]


def learn(moves, move):
    if move in moves:
        return
    if len(moves) == 4:
        moves.pop(0)
    moves.append(move)


def trainer_moves(species, level):
    moves = list(level1_moves(species))
    for lv, mv in learnset(species):
        if lv <= level:
            learn(moves, mv)
    return moves


def raised(chain, start, level):
    """A Pokemon obtained at `start` and levelled up one level at a time to
    `level`, always accepting the new move. chain = [(species, from_level)].
    Only a baseline: a real player keeps better moves and teaches TMs/HMs."""
    stage = lambda lv: [s for s, f in chain if f <= lv][-1]
    moves = trainer_moves(stage(start), start)
    for lv in range(start + 1, level + 1):
        for move_lv, mv in learnset(stage(lv)):
            if move_lv == lv:
                learn(moves, mv)
    return stage(level), moves


if __name__ == '__main__':
    Y = sys.argv[1]
    for arg in sys.argv[2:]:
        species, level = arg.split(':')
        print(species, level, ', '.join(trainer_moves(species, int(level))))
