#!/usr/bin/env python3
"""Print the CHANGELOG.md section of one release, for the release notes.

usage: changelog_section.py VERSION [CHANGELOG]

VERSION may carry the tag's leading "v". The section runs from the
"## VERSION" heading to the next heading of the same or a higher level.
Exits 1 when there is no such section, so a release cannot go out without
its notes.
"""
import re
import sys


def section(text, version):
    lines = text.splitlines()
    head = re.compile(r'^## ' + re.escape(version) + r'(\s|$)')
    for i, line in enumerate(lines):
        if head.match(line):
            body = []
            for line in lines[i + 1:]:
                if re.match(r'^#{1,2} ', line):
                    break
                body.append(line)
            return '\n'.join(body).strip('\n')
    return None


def main():
    if len(sys.argv) not in (2, 3):
        sys.exit(__doc__.strip())
    version = sys.argv[1]
    if version.startswith('v'):
        version = version[1:]
    path = sys.argv[2] if len(sys.argv) == 3 else 'CHANGELOG.md'
    with open(path, encoding='utf-8') as f:
        body = section(f.read(), version)
    if not body:
        sys.exit(f'{path}: no "## {version}" section with content; add one before releasing')
    print(body)


if __name__ == '__main__':
    main()
