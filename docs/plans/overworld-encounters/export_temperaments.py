#!/usr/bin/env python3
"""Merge the review page's saved choices into temperaments.json.

Input: data.json (proposals) and a directory of the artifact's "families"
documents as saved by `ArtifactData list ... out_dir=<dir>` (one
<FAMILY>.json per family the user touched). A family with no document keeps
the proposal; a family with one takes its profile and its full species map
(the page saves the complete set of exceptions, proposed ones included).
Profile weights come from an optional config/profiles.json in the same dump,
else from the page defaults below.

    python3 export_temperaments.py <dump>/families [<dump>/config/profiles.json]
"""
import glob, json, os, sys

HERE = os.path.dirname(os.path.abspath(__file__))
BEHAVIOURS = ['guarda', 'gira', 'avvicina', 'insegue', 'scappa', 'sparisce']
DEFAULT_PROFILES = {  # same as PROF in page_script.js
    'NEUTRO':     [30, 40, 0, 15, 15, 0],
    'TIMIDO':     [15, 25, 0, 0, 60, 0],
    'AGGRESSIVO': [15, 25, 0, 55, 5, 0],
    'CURIOSO':    [20, 20, 45, 10, 5, 0],
    'PLACIDO':    [65, 25, 0, 5, 5, 0],
    'SFUGGENTE':  [15, 15, 0, 0, 30, 40],
}

data = json.load(open(os.path.join(HERE, 'data.json')))
saved = {}
for p in glob.glob(os.path.join(sys.argv[1], '*.json')):
    doc = json.load(open(p))
    saved[os.path.basename(p)[:-5]] = doc.get('data', doc)

profiles = {k: dict(zip(BEHAVIOURS, v)) for k, v in DEFAULT_PROFILES.items()}
if len(sys.argv) > 2 and os.path.exists(sys.argv[2]):
    doc = json.load(open(sys.argv[2]))
    for k, w in doc.get('data', doc).items():
        profiles[k].update(w)

families = {}
for f in data:
    s = saved.get(f['id'])
    if s:
        entry = {'profile': s['profile'], 'species': dict(sorted(s.get('species', {}).items())), 'source': 'user'}
        if s.get('note'):
            entry['note'] = s['note']
    else:
        entry = {'profile': f['prop'], 'species': {x['id']: x['prop'] for x in f['species'] if x['prop']}, 'source': 'proposal'}
    families[f['id']] = entry

unknown = set(saved) - set(families)
if unknown:
    sys.exit(f'saved documents for unknown families: {sorted(unknown)}')
out = {'behaviours': BEHAVIOURS, 'profiles': profiles, 'families': families}
json.dump(out, open(os.path.join(HERE, 'temperaments.json'), 'w'), indent=1, ensure_ascii=False)
print(f'{len(families)} families, {sum(e["source"] == "user" for e in families.values())} from the user')
