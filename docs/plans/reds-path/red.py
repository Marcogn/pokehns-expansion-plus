#!/usr/bin/env python3
"""Baseline movesets for Red's team at each Red's Path challenge.

Usage: red.py <pokeyellow checkout>

The levels are the plan's proposal (docs/plans/reds-path.md), not game data:
no Gen 1 game stores a team for Red. Starting levels are: Pikachu Lv5
(Yellow starter), the three gift starters Lv10, Eevee Lv25, Snorlax Lv30.
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gen1moves

CHAINS = {
    'P': ([('PIKACHU', 1)], 5),
    'B': ([('BULBASAUR', 1), ('IVYSAUR', 16), ('VENUSAUR', 32)], 10),
    'C': ([('CHARMANDER', 1), ('CHARMELEON', 16), ('CHARIZARD', 36)], 10),
    'S': ([('SQUIRTLE', 1), ('WARTORTLE', 16), ('BLASTOISE', 36)], 10),
    'E': ([('EEVEE', 1)], 25),
    'Z': ([('SNORLAX', 1)], 30),
}

PLAN = [
    ('Brock', dict(P=14)),
    ('Misty', dict(P=22, B=19, C=18)),
    ('Surge', dict(P=28, B=25, C=25)),
    ('Erika', dict(P=33, B=30, C=30, S=26, E=26)),
    ('Koga', dict(P=50, B=46, C=46, S=43, E=40, Z=38)),
    ('Sabrina', dict(P=52, B=48, C=48, S=46, E=43, Z=44)),
    ('Blaine', dict(P=55, B=51, C=51, S=50, E=46, Z=48)),
    ('Giovanni', dict(P=58, B=54, C=54, S=53, E=50, Z=51)),
]

if __name__ == '__main__':
    gen1moves.Y = sys.argv[1]
    for leader, team in PLAN:
        print('==', leader)
        for key, level in team.items():
            chain, start = CHAINS[key]
            species, moves = gen1moves.raised(chain, start, level)
            print(f'  {species} {level}: {", ".join(moves)}')
