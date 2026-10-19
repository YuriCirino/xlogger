#!/usr/bin/env python3
"""List the marker strings inside an ELF and the section that holds each one.

Only sections with the ALLOC flag end up in the flashed firmware; markers that
appear only in non-allocated sections (debug info) do not cost flash.

  python3 find_markers.py firmware.elf          -> prints "MARKER<TAB>section<TAB>flash|debug"
"""
import re
import sys

from elftools.elf.constants import SH_FLAGS
from elftools.elf.elffile import ELFFile

PATTERN = re.compile(rb"(?:STRIP|KEEP)_MARKER_[A-Z_]+")

with open(sys.argv[1], "rb") as f:
    elf = ELFFile(f)
    seen = set()
    for sec in elf.iter_sections():
        if sec.header.sh_type == "SHT_NOBITS":
            continue
        data = sec.data()
        kind = "flash" if sec.header.sh_flags & SH_FLAGS.SHF_ALLOC else "debug"
        for m in PATTERN.finditer(data):
            key = (m.group().decode(), sec.name, kind)
            if key not in seen:
                seen.add(key)
                print("\t".join(key))
