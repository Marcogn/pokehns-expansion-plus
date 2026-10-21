#!/usr/bin/env python3
"""Build data.json for the "Indole dei selvatici" review page.

Reads src/data/wild_encounters.json (only the *_HNS maps the hns build uses),
groups every species met on land or water into its family (the #if P_FAMILY_X
blocks of src/data/pokemon/species_info/gen_N_families.h) and attaches the
proposed temperament from proposals.py. Fishing-only and Rock Smash-only
species are left out: they never become overworld encounters.

Run from the repo root:
    python3 docs/plans/overworld-encounters/extract_families.py
then rebuild the page with build_page.py.
"""
import collections, glob, json, os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from proposals import P_, EXTRA  # noqa: E402

wild = collections.defaultdict(lambda: collections.defaultdict(set))
for group in json.load(open('src/data/wild_encounters.json'))['wild_encounter_groups']:
    if not group.get('for_maps'):
        continue
    for enc in group['encounters']:
        if not enc.get('map', '').endswith('_HNS'):
            continue
        for kind, mons in enc.items():
            if kind.endswith('_mons'):
                for s in re.findall(r'SPECIES_\w+', json.dumps(mons)):
                    if s != 'SPECIES_NONE':
                        wild[s][kind].add(enc['map'])

fam, order = {}, []
for g in range(1, 10):
    path = f'src/data/pokemon/species_info/gen_{g}_families.h'
    if not os.path.exists(path):
        continue
    cur = None
    for line in open(path):
        m = re.match(r'#if P_FAMILY_(\w+)', line)
        if m:
            cur = m.group(1)
            order.append((g, cur))
        m = re.match(r'\s*\[(SPECIES_\w+)\]\s*=', line)
        if m and cur:
            fam[m.group(1)] = (g, cur)

def pretty_map(m):
    m = re.sub(r'ROUTE(\d+)', r'ROUTE \1', m[4:-4])
    return ' '.join(w.capitalize() for w in m.split('_'))

def pretty(s):
    return ' '.join(w.capitalize() for w in s.split('_'))

OWE_KINDS = {'land_mons': 'terra', 'water_mons': 'acqua'}
out = []
for g, f in order:
    members = [s for s, v in fam.items() if v == (g, f) and set(wild.get(s, {})) & set(OWE_KINDS)]
    if not members:
        continue
    prof, over, why, mod = P_[f]
    over = dict(over, **EXTRA.get(f, {}))
    species = []
    for s in members:
        maps = set().union(*(wild[s][k] for k in OWE_KINDS if k in wild[s]))
        species.append({'id': s[8:], 'name': pretty(s[8:]), 'maps': sorted(map(pretty_map, maps)),
                        'kinds': sorted(OWE_KINDS[k] for k in OWE_KINDS if k in wild[s]),
                        'prop': over.get(s[8:])})
    name = {'VOLBEAT_ILLUMISE': 'Volbeat / Illumise', 'HITMONS': 'Tyrogue'}.get(f, pretty(f))
    out.append({'id': f, 'gen': g, 'name': name, 'prop': prof, 'why': why, 'mod': bool(mod), 'species': species})

missing = [s for s in wild if s not in fam and set(wild[s]) & set(OWE_KINDS)]
if missing:
    print('species with no family block (not in the table):', missing, file=sys.stderr)
json.dump(out, open(os.path.join(HERE, 'data.json'), 'w'), separators=(',', ':'), ensure_ascii=False)
print(f'{len(out)} families, {sum(len(f["species"]) for f in out)} species')
