#!/usr/bin/env python3
"""Debug tool for COFF symbols and COMDAT flags

Usage:

    # Print section information
    python tools/coffsym.py sections ./source/client/Fresh/source/frarea.obj

    # Find conflicting duplicates
    python tools/coffsym.py dupes

    # List symbols with filtering.
    python tools/coffsym.py list ./source/client/ProjectG/addfrienddlg.obj --filter FrAddFriendDlg --comdat

    # Perform simple diff of symbols
    python tools/coffsym.py diff ./source/client/Wangreal/source/wtextureview.obj ./build/source/client/Wangreal/source/wtextureview.obj

    # Diff with disassembly
    python tools/coffsym.py diff -d ./source/client/Wangreal/source/woverlay.obj ./build/source/client/Wangreal/source/woverlay.obj

    # Patch the COMDAT selection flag for a specific symbol
    python tools/coffsym.py set-selection ./source/client/Wangreal/source/wview.obj '??1WView@@UAE@XZ' any

    # Globally rename a symbol that has a placeholder name
    python tools/coffsym.py rename __pg_c_000b68 '?WCrossProduct@@YI?AVWVector@@ABV1@0@Z'
"""

from __future__ import annotations

import argparse
import difflib
import json
import shutil
import struct
import sys
from pathlib import Path

IMAGE_FILE_MACHINE_I386 = 0x014C
IMAGE_SCN_CNT_CODE = 0x0020
IMAGE_SCN_LNK_COMDAT = 0x1000
IMAGE_SYM_CLASS_EXTERNAL = 2
IMAGE_SYM_CLASS_STATIC = 3

RELOCATIONS = {
    0x0006: "dir32",
    0x0007: "dir32nb",
    0x000A: "section",
    0x000B: "secrel",
    0x000C: "token",
    0x000D: "secrel7",
    0x0014: "rel32",
}

SELECTIONS = {
    0: "plain",
    1: "nodup",
    2: "any",
    3: "same-size",
    4: "exact",
    5: "assoc",
    6: "largest",
}
BY_NAME = {v: k for k, v in SELECTIONS.items()}
ROOT = Path(__file__).resolve().parents[1]
CONFIG = ROOT / "build.json"


class Section:
    def __init__(self, index, name, size, characteristics, raw_ptr, rel_ptr, nrelocs):
        self.index = index
        self.name = name
        self.size = size
        self.characteristics = characteristics
        self.raw_ptr = raw_ptr
        self.rel_ptr = rel_ptr
        self.nrelocs = nrelocs
        self.selection = 0
        self.sel_offset = None
        self.owner = None

    @property
    def code(self):
        return bool(self.characteristics & IMAGE_SCN_CNT_CODE)

    @property
    def comdat(self):
        return bool(self.characteristics & IMAGE_SCN_LNK_COMDAT)


class Symbol:
    def __init__(self, name, value, section, storage_class, offset=0, index=0):
        self.index = index
        self.name = name
        self.value = value
        self.section = section
        self.storage_class = storage_class
        self.offset = offset

    @property
    def defined(self):
        return self.storage_class == IMAGE_SYM_CLASS_EXTERNAL and self.section > 0

    @property
    def undefined(self):
        return self.storage_class == IMAGE_SYM_CLASS_EXTERNAL and self.section == 0


class Module:
    def __init__(self, label, data, base=0, from_archive=False):
        self.label = label
        self.data = data
        self.base = base
        self.from_archive = from_archive
        self.sections = []
        self.symbols = []
        self.symtab = {}
        self.strings = None
        self._parse()

    def _parse(self):
        d = self.data
        if len(d) < 20:
            raise ValueError("truncated COFF header")
        nsections = struct.unpack_from("<H", d, 2)[0]
        symptr, nsymbols = struct.unpack_from("<II", d, 8)
        opt = struct.unpack_from("<H", d, 16)[0]

        base = 20 + opt
        for i in range(nsections):
            o = base + i * 40
            name = d[o : o + 8].rstrip(b"\0").decode("latin1")
            size, raw_ptr, rel_ptr = struct.unpack_from("<III", d, o + 16)
            nrelocs = struct.unpack_from("<H", d, o + 32)[0]
            characteristics = struct.unpack_from("<I", d, o + 36)[0]
            self.sections.append(
                Section(i + 1, name, size, characteristics, raw_ptr, rel_ptr, nrelocs)
            )

        if not symptr:
            return
        strings = symptr + nsymbols * 18
        self.strings = self.base + strings
        i = 0
        while i < nsymbols:
            o = symptr + i * 18
            raw = d[o : o + 18]
            if raw[0:4] == b"\0\0\0\0":
                off = struct.unpack_from("<I", raw, 4)[0]
                end = d.index(b"\0", strings + off)
                name = d[strings + off : end].decode("latin1")
            else:
                name = raw[:8].rstrip(b"\0").decode("latin1")
            value, section, _type, storage, naux = struct.unpack_from("<IhHBB", raw, 8)
            sym = Symbol(name, value, section, storage, self.base + o, i)
            self.symbols.append(sym)
            self.symtab[i] = sym

            if naux and storage == IMAGE_SYM_CLASS_STATIC and 0 < section <= nsections:
                sec = self.sections[section - 1]
                sec.selection = d[o + 18 + 14]
                sec.sel_offset = self.base + o + 18 + 14
            elif sym.defined and not name.startswith("."):
                sec = self.sections[section - 1]
                if sec.owner is None:
                    sec.owner = name
            i += 1 + naux

    def section_data(self, sec):
        if not sec.raw_ptr:
            return b""
        return self.data[sec.raw_ptr : sec.raw_ptr + sec.size]

    def relocations(self, sec):
        out = {}
        for i in range(sec.nrelocs):
            o = sec.rel_ptr + i * 10
            addr, index, kind = struct.unpack_from("<IIH", self.data, o)
            sym = self.symtab.get(index)
            out[addr] = (
                sym.name if sym else f"#{index}",
                RELOCATIONS.get(kind, hex(kind)),
            )
        return out

    def extent(self, sym):
        sec = self.sections[sym.section - 1]
        end = sec.size
        for other in self.symbols:
            if (
                other.section == sym.section
                and sym.value < other.value < end
                and other.storage_class
                in (IMAGE_SYM_CLASS_EXTERNAL, IMAGE_SYM_CLASS_STATIC)
                and not other.name.startswith(".")
            ):
                end = other.value
        return sym.value, end


def archive_members(data):
    """Yield members out of an `ar` archive."""
    pos, names = 8, b""
    while pos + 60 <= len(data):
        name = data[pos : pos + 16].decode("latin1").rstrip()
        try:
            size = int(data[pos + 48 : pos + 58])
        except ValueError:
            break
        if name == "//":
            names = data[pos + 60 : pos + 60 + size]
        elif name != "/":
            if name.startswith("/") and name[1:].isdigit():
                start = int(name[1:])
                name = (
                    names[start:].split(b"\0", 1)[0].split(b"\n", 1)[0].decode("latin1")
                )
            name = name.replace("\\", "/").rstrip("/").rsplit("/", 1)[-1]
            yield name, pos + 60, size
        pos += 60 + size + (size & 1)


def load(path):
    """Load an object or archive."""
    data = Path(path).read_bytes()
    if data[:8] == b"!<arch>\n":
        out = []
        for name, off, size in archive_members(data):
            chunk = data[off : off + size]
            if (
                len(chunk) >= 20
                and struct.unpack_from("<H", chunk, 0)[0] == IMAGE_FILE_MACHINE_I386
            ):
                out.append(Module(f"{path}({name})", chunk, off, from_archive=True))
        return out
    return [Module(str(path), data)]


def load_many(paths):
    """Load many objects or archives."""
    modules = []
    for p in paths:
        try:
            modules.extend(load(p))
        except (OSError, ValueError, IndexError, struct.error) as exc:
            print(f"warning: {p}: {exc}", file=sys.stderr)
    return modules


def delinked_objects():
    root = str(CONFIG.resolve().parent / "build")
    return [
        p
        for p in build_json_inputs()
        if p.endswith(".obj") and not p.startswith(root) and Path(p).is_file()
    ]


def build_json_inputs():
    root = CONFIG.resolve().parent
    cfg = json.loads(CONFIG.read_text())
    out = []

    def walk(entries):
        for e in entries:
            if isinstance(e, dict) and "library" in e:
                walk(e["members"])
                continue
            p = e if isinstance(e, str) else e.get("path", "")
            suffix = Path(p).suffix.lower()
            if suffix in (".obj", ".lib"):
                out.append(str(root / p))
            elif suffix in (".c", ".cpp", ".cxx", ".h"):
                product = root / "build" / Path(p).with_suffix(".obj")
                if product.exists():
                    out.append(str(product))
                else:
                    print(f"warning: not built yet: {p}", file=sys.stderr)

    walk(cfg["inputs"])
    return out


_CS = None


def disassembler():
    global _CS
    if _CS is None:
        try:
            import capstone
        except ImportError:
            sys.exit(
                "capstone not available - please use `uv run` or `nix develop` shell to run this script"
            )
        _CS = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        _CS.skipdata = True
    return _CS


def disassemble(mod, sym):
    """Quick'n'dirty disassembly output for a symbol"""
    sec = mod.sections[sym.section - 1]
    data = mod.section_data(sec)
    if not data or not sec.code:
        return []
    start, end = mod.extent(sym)
    relocs = mod.relocations(sec)
    out = []
    for insn in disassembler().disasm(data[start:end], start):
        text = f"{insn.mnemonic} {insn.op_str}".strip()
        targets = [
            f"{relocs[a][0]} ({relocs[a][1]})"
            for a in range(insn.address, insn.address + insn.size)
            if a in relocs
        ]
        if targets:
            text += "  ; " + ", ".join(targets)
        out.append((insn.address - start, insn.bytes, text))
    return out


def side_by_side(left, right, width=None):
    width = width or shutil.get_terminal_size((160, 24)).columns
    col = max(24, (width - 5) // 2)

    def render(row):
        if row is None:
            return ""
        off, raw, text = row
        hexed = raw.hex()
        if len(hexed) > 12:
            hexed = hexed[:11] + "+"
        return f"{off:04x} {hexed:<12} {text}"

    a = [t for _, _, t in left]
    b = [t for _, _, t in right]
    lines = []
    for tag, i1, i2, j1, j2 in difflib.SequenceMatcher(None, a, b).get_opcodes():
        if tag == "equal":
            pairs = [(left[i], right[j]) for i, j in zip(range(i1, i2), range(j1, j2))]
            marks = [" "] * len(pairs)
        else:
            n = max(i2 - i1, j2 - j1)
            ls = list(left[i1:i2]) + [None] * (n - (i2 - i1))
            rs = list(right[j1:j2]) + [None] * (n - (j2 - j1))
            pairs = list(zip(ls, rs))
            marks = [
                "~" if lr[0] and lr[1] else ("<" if lr[0] else ">") for lr in pairs
            ]
        for (lrow, rrow), mark in zip(pairs, marks):
            lines.append(
                f"  {render(lrow):<{col}.{col}} {mark} {render(rrow):.{col}}".rstrip()
            )
    return lines


def cmd_sections(args):
    for mod in load_many(args.paths):
        print(f"== {mod.label}")
        for sec in mod.sections:
            if not args.debug and (
                sec.name.startswith(".debug") or sec.name == ".drectve"
            ):
                continue
            sel = SELECTIONS.get(sec.selection, "?")
            print(f"  {sec.name:<10} {sec.size:<#8x} {sel:<9} {sec.owner or ''}")


def cmd_list(args):
    for mod in load_many(args.paths):
        rows = []
        for sym in mod.symbols:
            if args.undefined and not sym.undefined:
                continue
            if not args.undefined and not sym.defined:
                continue
            if args.filter and args.filter not in sym.name:
                continue
            sec = mod.sections[sym.section - 1] if sym.section > 0 else None
            if args.comdat and not (sec and sec.comdat):
                continue
            rows.append((sym, sec))
        if not rows:
            continue
        print(f"== {mod.label}")
        for sym, sec in rows:
            if sec is None:
                print(f"  {'(undef)':<10} {'':<8} {'':<9} {sym.name}")
            else:
                sel = SELECTIONS.get(sec.selection, "?")
                print(f"  {sec.name:<10} {sec.size:<#8x} {sel:<9} {sym.name}")


def cmd_dupes(args):
    paths = args.paths if len(args.paths) > 0 else build_json_inputs()
    table = {}
    for mod in load_many(paths):
        for sym in mod.symbols:
            if not sym.defined:
                continue
            sec = mod.sections[sym.section - 1]
            table.setdefault(sym.name, []).append(
                (mod.label, sec.selection, mod.from_archive)
            )
    conflicts = 0
    for name, defs in sorted(table.items()):
        if len(defs) < 2:
            continue
        bad = [d for d in defs if d[1] in (0, 1)]
        if bad and not args.all and all(d[2] for d in defs):
            bad = []
        if not bad and not args.all:
            continue
        conflicts += bool(bad)
        flag = "LNK2005" if bad else "ok"
        print(f"{flag:<8} {name}")
        for label, sel, _ in defs:
            print(f"           {SELECTIONS.get(sel, '?'):<9} {label}")
    print(
        f"\n{conflicts} conflicting symbol(s) out of {len(table)} defined",
        file=sys.stderr,
    )
    return 1 if conflicts else 0


def cmd_diff(args):
    def index(path):
        out = {}
        for mod in load_many([path]):
            for sym in mod.symbols:
                if sym.defined:
                    out[sym.name] = (mod, sym, mod.sections[sym.section - 1])
        return out

    a, b = index(args.a), index(args.b)
    for name in sorted(set(a) - set(b)):
        print(f"- {a[name][2].name:<12} {a[name][2].size:<#8x} {name}")
    for name in sorted(set(b) - set(a)):
        print(f"+ {b[name][2].name:<12} {b[name][2].size:<#8x} {name}")
    for name in sorted(set(a) & set(b)):
        amod, asym, asec = a[name]
        bmod, bsym, bsec = b[name]
        changed = asec.size != bsec.size
        if not args.disasm:
            if changed:
                print(f"~ {asec.name:<12} {asec.size:<#8x} -> {bsec.size:<#8x} {name}")
            continue
        left, right = disassemble(amod, asym), disassemble(bmod, bsym)
        if not left and not right:
            continue
        same = [t for _, _, t in left] == [t for _, _, t in right]
        if same and not changed:
            continue
        sizes = f"{asec.size:#x} -> {bsec.size:#x}" if changed else f"{asec.size:#x}"
        print(f"~ {asec.name:<12} {sizes:<18} {name}")
        for line in side_by_side(left, right, args.width):
            print(line)
        print()


def cmd_set_selection(args):
    want = BY_NAME.get(args.selection)
    if want is None:
        want = int(args.selection, 0)
    modules = load(args.path)
    hits = []
    for mod in modules:
        for sym in mod.symbols:
            if sym.defined and sym.name == args.symbol:
                hits.append((mod, mod.sections[sym.section - 1]))
    if not hits:
        sys.exit(f"{args.symbol}: not defined in {args.path}")
    if len(hits) > 1:
        sys.exit(f"{args.symbol}: defined {len(hits)} times in {args.path}")
    mod, sec = hits[0]
    if not sec.comdat or sec.sel_offset is None:
        sys.exit(f"{args.symbol}: section {sec.name} is not a COMDAT")
    was = SELECTIONS.get(sec.selection, sec.selection)
    now = SELECTIONS.get(want, want)
    print(
        f"{args.symbol}: {sec.name} selection {was} -> {now} (file offset {sec.sel_offset})"
    )
    if args.dry_run:
        return
    data = bytearray(Path(args.path).read_bytes())
    data[sec.sel_offset] = want
    Path(args.path).write_bytes(bytes(data))


def patch_name(data, sym, strings, new):
    """Rewrite a symbol name, expanding the string table if necessary."""
    raw = new.encode("latin1")
    if len(raw) <= 8:
        data[sym.offset : sym.offset + 8] = raw.ljust(8, b"\0")
        return
    if len(raw) <= len(sym.name):
        at = strings + struct.unpack_from("<I", data, sym.offset + 4)[0]
        data[at : at + len(raw) + 1] = raw + b"\0"
        return
    size = struct.unpack_from("<I", data, strings)[0]
    if strings + size != len(data):
        raise ValueError("string table is not at the end of the file")
    struct.pack_into("<I", data, sym.offset, 0)
    struct.pack_into("<I", data, sym.offset + 4, size)
    data += raw + b"\0"
    struct.pack_into("<I", data, strings, len(data) - strings)


def cmd_rename(args):
    """Globally rename a symbol across all objects."""
    paths = args.paths if args.paths else delinked_objects()
    needle = args.old.encode("latin1")
    plan = []
    for path in paths:
        if needle not in Path(path).read_bytes():
            continue
        try:
            modules = load(path)
        except (OSError, ValueError, IndexError, struct.error) as exc:
            print(f"warning: {path}: {exc}", file=sys.stderr)
            continue
        hits = []
        for mod in modules:
            if mod.from_archive:
                sys.exit(f"{path}: cannot rename inside an archive ({mod.label})")
            if mod.strings is None:
                continue
            if any(sym.name == args.new for sym in mod.symbols):
                sys.exit(f"{args.new}: already present in {path}")
            hits += [(mod.strings, sym) for sym in mod.symbols if sym.name == args.old]
        if hits:
            plan.append((path, hits))
    if not plan:
        sys.exit(f"{args.old}: not found in {len(paths)} object(s)")
    records = 0
    for path, hits in plan:
        defs = sum(1 for _, sym in hits if sym.defined)
        print(f"{path}: {len(hits)} record(s), {defs} definition(s)")
        records += len(hits)
        if args.dry_run:
            continue
        data = bytearray(Path(path).read_bytes())
        for strings, sym in hits:
            patch_name(data, sym, strings, args.new)
        Path(path).write_bytes(bytes(data))
    print(
        f"\n{args.old} -> {args.new}: {records} record(s) in {len(plan)} object(s)",
        file=sys.stderr,
    )


def main(argv=None):
    parser = argparse.ArgumentParser(
        description="Debug tool for COFF symbols and COMDAT flags"
    )
    sub = parser.add_subparsers(dest="cmd", required=True)

    p = sub.add_parser("sections", help="list sections")
    p.add_argument("paths", nargs="+")
    p.add_argument("--debug", action="store_true")
    p.set_defaults(func=cmd_sections)

    p = sub.add_parser("list", help="list symbols")
    p.add_argument("paths", nargs="+")
    p.add_argument("--filter")
    p.add_argument("--comdat", action="store_true")
    p.add_argument("--undefined", action="store_true")
    p.set_defaults(func=cmd_list)

    p = sub.add_parser("dupes", help="find duplicate symbols")
    p.add_argument("paths", nargs="*")
    p.add_argument("--all", action="store_true", help="show non-conflicting duplicates")
    p.set_defaults(func=cmd_dupes)

    p = sub.add_parser("diff", help="compare object file symbol-by-symbol")
    p.add_argument("a")
    p.add_argument("b")
    p.add_argument(
        "-d", "--disasm", action="store_true", help="side-by-side disassembly"
    )
    p.add_argument("--width", type=int, help="total output width (default: terminal)")
    p.set_defaults(func=cmd_diff)

    p = sub.add_parser("rename", help="rename a symbol across objects")
    p.add_argument("old")
    p.add_argument("new")
    p.add_argument(
        "paths", nargs="*", help="objects to patch (default: every delinked object)"
    )
    p.add_argument("-n", "--dry-run", action="store_true")
    p.set_defaults(func=cmd_rename)

    p = sub.add_parser("set-selection", help="patch a COMDAT section's selection flags")
    p.add_argument("path")
    p.add_argument("symbol")
    p.add_argument("selection", help="name ({}) or number".format("|".join(BY_NAME)))
    p.add_argument("-n", "--dry-run", action="store_true")
    p.set_defaults(func=cmd_set_selection)

    args = parser.parse_args(argv)
    sys.exit(args.func(args) or 0)


if __name__ == "__main__":
    main()
