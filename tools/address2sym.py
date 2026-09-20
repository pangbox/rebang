#!/usr/bin/env python3
"""Finds what translation unit an address belongs to."""
import argparse
import bisect
import csv
import os
import sys

DOCS = os.path.join(os.path.dirname(os.path.abspath(__file__)), os.pardir, "docs")
IMAGE_BASE = 0x00400000


def read_csv(name):
    with open(os.path.join(DOCS, name), newline="") as f:
        return list(csv.DictReader(f))


def load_sections():
    sections = []
    for row in read_csv("module-sections.csv"):
        rva = int(row["rva"], 16)
        sections.append(
            {
                "id": int(row["section_id"]),
                "name": row["name"],
                "rva": rva,
                "size": int(row["virtual_size"], 16),
            }
        )
    return sections


def load_modules():
    modules = {}
    for row in read_csv("module-list.csv"):
        modules[int(row["module"])] = {
            "original_object": row["original_object"],
            "original_module": row["original_module"],
            "path": None,
        }
    for row in read_csv("module-sizes.csv"):
        modules.setdefault(int(row["module"]), {}).setdefault("original_object", row["original_object"])
        modules[int(row["module"])]["path"] = row["path"]
    return modules


def load_contributions():
    """Return {section_id: (sorted offsets, rows)}."""
    by_section = {}
    for row in read_csv("object-boundaries.csv"):
        by_section.setdefault(int(row["section"]), []).append(
            {
                "contribution": int(row["contribution"]),
                "module": int(row["module"]),
                "offset": int(row["offset"], 16),
                "size": int(row["size"], 16),
            }
        )
    out = {}
    for sid, rows in by_section.items():
        rows.sort(key=lambda r: r["offset"])
        out[sid] = ([r["offset"] for r in rows], rows)
    return out


def lookup(rva, sections, contributions, modules):
    section = next(
        (s for s in sections if s["rva"] <= rva < s["rva"] + s["size"]), None
    )
    if section is None:
        return None, None, None
    offsets, rows = contributions.get(section["id"], ([], []))
    i = bisect.bisect_right(offsets, rva - section["rva"]) - 1
    if i < 0:
        return section, None, None
    contrib = rows[i]
    if not contrib["offset"] <= rva - section["rva"] < contrib["offset"] + contrib["size"]:
        # In a section but inside padding between contributions.
        return section, None, contrib
    return section, contrib, contrib


def main():
    ap = argparse.ArgumentParser(description="Find what module an address belongs to")
    ap.add_argument("addresses", nargs="+", help="addresses, e.g. 0x00702440")
    ap.add_argument("--base", default=hex(IMAGE_BASE), help="image base (default 0x00400000)")
    ap.add_argument("--rva", action="store_true", help="treat addresses as RVAs")
    args = ap.parse_args()

    base = 0 if args.rva else int(args.base, 16)
    sections = load_sections()
    modules = load_modules()
    contributions = load_contributions()

    status = 0
    for text in args.addresses:
        va = int(text, 16)
        rva = va - base
        print(f"{va:#010x}  (rva {rva:#08x})")
        if rva < 0:
            print("  below image base")
            status = 1
            continue
        section, contrib, nearest = lookup(rva, sections, contributions, modules)
        if section is None:
            print("  not in any section")
            status = 1
            continue
        print(f"  section {section['id']} {section['name']} "
              f"+{rva - section['rva']:#x}")
        if contrib is None:
            print("  no contribution covers this address")
            if nearest is not None:
                m = modules.get(nearest["module"], {})
                print(f"  previous contribution: {m.get('path') or m.get('original_object')}")
            status = 1
            continue
        m = modules.get(contrib["module"], {})
        start = section["rva"] + contrib["offset"] + base
        print(f"  module {contrib['module']}: {m.get('original_object', '?')}"
              f" (in {m.get('original_module', '?')})")
        print(f"  path: {m.get('path') or '<not yet decompiled>'}")
        print(f"  contribution {contrib['contribution']}: "
              f"{start:#010x}..{start + contrib['size']:#010x} "
              f"(size {contrib['size']:#x}, +{rva - section['rva'] - contrib['offset']:#x})")
    return status


if __name__ == "__main__":
    sys.exit(main())
