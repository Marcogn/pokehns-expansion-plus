#!/usr/bin/env python3
"""Assemble indole_selvatici.html from the page parts and data.json.

The published page is https://claude.ai/artifact/5wJUsUmXQhwuyvNksf4uBc
(db capability: collection "families", doc "config/profiles"). Republishing
keeps the user's saved choices: they live in the artifact database, not in
the HTML.
"""
import os
HERE = os.path.dirname(os.path.abspath(__file__))
rd = lambda n: open(os.path.join(HERE, n), encoding='utf-8').read()
html = rd('page_head.html') + rd('page_body.html') + '<script>\nconst DATA=' + rd('data.json') + ';\n' + rd('page_script.js') + '</script>\n'
open(os.path.join(HERE, 'indole_selvatici.html'), 'w', encoding='utf-8').write(html)
print(len(html), 'bytes')
