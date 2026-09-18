#!/usr/bin/env python3
"""Decomp progress tool"""

from __future__ import annotations

import argparse
import csv
import json
import subprocess
import sys
from collections.abc import Iterable, Iterator
from datetime import datetime
from pathlib import Path
from typing import NamedTuple

ROOT = Path(__file__).resolve().parents[1]
CONFIG = ROOT / "build.json"
SIZES_CSV = ROOT / "docs/module-sizes.csv"
HISTORY_CSV = ROOT / "docs/progress-history.csv"
CHECKLIST_MD = ROOT / "docs/progress.md"
GRAPH_SVG = ROOT / "docs/progress.svg"
THIRD_PARTY = ("/lib/", "/third_party/", "/LuaSystem/Lua/", "tools/")
SOURCE_SUFFIXES = (".c", ".cpp", ".cxx")

GROUPS = (
    ("Fresh", "source/client/Fresh/"),
    ("ProjectG", "source/client/ProjectG/"),
    ("Wangreal", "source/client/Wangreal/"),
    ("shared", "source/shared/"),
)

HISTORY_FIELDS = (
    "commit",
    "date",
    "ordinal",
    "units",
    "total_units",
    "size",
    "total_size",
) + tuple(name.lower() for name, _ in GROUPS)

WIDTH, HEIGHT = 720, 260
MARGIN = (48, 16, 36, 56)
COLORS = (
    ("ProjectG", "#48d"),
    ("Wangreal", "#5b6"),
    ("Fresh", "#da4"),
    ("shared", "#a7d"),
)


class Unit(NamedTuple):
    path: str
    module: int
    original: str
    size: int

    @property
    def group(self) -> str:
        for name, prefix in GROUPS:
            if self.path.startswith(prefix):
                return name
        raise ValueError(f"{self.path}: not in a known group")

    @property
    def done(self) -> bool:
        return self.path.endswith(SOURCE_SUFFIXES)


def walk_paths(entries: Iterable) -> Iterator[str]:
    for entry in entries:
        if isinstance(entry, dict) and "library" in entry:
            yield from walk_paths(entry["members"])
        elif isinstance(entry, dict):
            yield entry["path"]
        else:
            yield entry


def entry_paths(config: dict) -> Iterator[str]:
    walk = walk_paths
    yield from walk(config.get("precompiled_headers", []))
    yield from walk(config["inputs"])


def candidates(config: dict) -> Iterator[tuple[str, str]]:
    for path in entry_paths(config):
        if any(part in path for part in THIRD_PARTY):
            continue
        name = path.rsplit("/", 1)[-1]
        stem, _, suffix = name.rpartition(".")
        if suffix.lower() not in ("obj",) + tuple(s[1:] for s in SOURCE_SUFFIXES):
            continue
        yield stem.lower(), path


def load_sizes() -> dict[int, tuple[str, str, int]]:
    out = {}
    with SIZES_CSV.open(newline="") as handle:
        for row in csv.DictReader(handle):
            out[int(row["module"])] = (
                row["path"],
                row["original_object"],
                int(row["size"]),
            )
    return out


def units(config: dict) -> list[Unit]:
    table = load_sizes()
    by_stem = {
        path.rsplit("/", 1)[-1].rpartition(".")[0].lower(): (module, original, size)
        for module, (path, original, size) in table.items()
    }
    seen, out = set(), []
    for stem, path in candidates(config):
        if stem not in by_stem:
            continue
        module, original, size = by_stem[stem]
        seen.add(module)
        out.append(Unit(path, module, original, size))
    for module in sorted(set(table) - seen):
        path, original, size = table[module]
        print(
            f"warning: module {module} ({original}) is no longer built", file=sys.stderr
        )
        out.append(Unit(path, module, original, size))
    out.sort(key=lambda unit: unit.path)
    return out


class Totals(NamedTuple):
    count: int
    total_count: int
    size: int
    total_size: int

    @property
    def percent(self) -> float:
        return 100.0 * self.size / self.total_size if self.total_size else 0.0


def totals(items: Iterable[Unit]) -> Totals:
    items = list(items)
    done = [unit for unit in items if unit.done]
    return Totals(
        len(done),
        len(items),
        sum(unit.size for unit in done),
        sum(unit.size for unit in items),
    )


def config_at(rev: str | None) -> dict:
    if rev is None:
        return json.loads(CONFIG.read_text())
    text = subprocess.run(
        ["git", "-C", str(ROOT), "show", f"{rev}:build.json"],
        check=True,
        capture_output=True,
        text=True,
    ).stdout
    return json.loads(text)


def cmd_summary(args: argparse.Namespace) -> int:
    items = units(config_at(args.rev))
    overall = totals(items)
    width = max(len(name) for name, _ in GROUPS)
    for name, _ in GROUPS:
        group = totals(unit for unit in items if unit.group == name)
        print(
            f"{name:<{width}}  {group.count:>3}/{group.total_count:<3} units  "
            f"{group.size:>9,}/{group.total_size:<9,} bytes  {group.percent:6.3f}%"
        )
    print(
        f"{'total':<{width}}  {overall.count:>3}/{overall.total_count:<3} units  "
        f"{overall.size:>9,}/{overall.total_size:<9,} bytes  {overall.percent:6.3f}%"
    )
    return 0


def cmd_checklist(args: argparse.Namespace) -> int:
    items = units(config_at(args.rev))
    overall = totals(items)
    out = [
        "# Progress",
        "",
        "> [!IMPORTANT]",
        "> This file is automatically generated by `tools/progress.py`. Do not edit!",
        "",
        (
            f"**{overall.count} of {overall.total_count} translation units,"
            f" {overall.size:,} of {overall.total_size:,} bytes"
            f" ({overall.percent:.3f}%).**"
        ),
        "",
        "| Group | Units | Bytes | Progress |",
        "| --- | ---: | ---: | ---: |",
    ]
    for name, _ in GROUPS:
        group = totals(unit for unit in items if unit.group == name)
        out.append(
            f"| {name} | {group.count} / {group.total_count} |"
            f" {group.size:,} / {group.total_size:,} | {group.percent:.3f}% |"
        )
    out.append(
        f"| **total** | **{overall.count} / {overall.total_count}** |"
        f" **{overall.size:,} / {overall.total_size:,}** |"
        f" **{overall.percent:.3f}%** |"
    )
    for name, _ in GROUPS:
        out += ["", f"## {name}", ""]
        for unit in items:
            if unit.group != name:
                continue
            mark = "x" if unit.done else " "
            out.append(f"- [{mark}] `{unit.path}` ({unit.size:,} bytes)")
    out.append("")
    CHECKLIST_MD.write_text("\n".join(out))
    print(f"wrote {CHECKLIST_MD.relative_to(ROOT)} ({overall.percent:.3f}%)")
    return 0


def git(*args: str) -> str:
    return subprocess.run(
        ["git", "-C", str(ROOT), *args], check=True, capture_output=True, text=True
    ).stdout.strip()


def git_describe_revision(rev: str) -> tuple[str, str, int]:
    commit = git("rev-parse", rev)
    date = git("show", "-s", "--format=%cI", commit)
    ordinal = int(git("rev-list", "--count", commit))
    return commit, date, ordinal


def measure_progress(rev: str | None) -> dict[str, str]:
    items = units(config_at(rev))
    overall = totals(items)
    described = rev or "HEAD"
    commit, date, ordinal = git_describe_revision(described)
    row = {
        "commit": commit,
        "date": date,
        "ordinal": str(ordinal),
        "units": str(overall.count),
        "total_units": str(overall.total_count),
        "size": str(overall.size),
        "total_size": str(overall.total_size),
    }
    for name, _ in GROUPS:
        group = totals(unit for unit in items if unit.group == name)
        row[name.rsplit("/", 1)[-1].lower()] = str(group.size)
    return row


def read_history() -> list[dict[str, str]]:
    with HISTORY_CSV.open(newline="") as handle:
        return list(csv.DictReader(handle))


def write_history(rows: list[dict[str, str]]) -> None:
    rows.sort(key=lambda row: int(row["ordinal"]))
    with HISTORY_CSV.open("w", newline="") as handle:
        fields: list[str] = list(HISTORY_FIELDS)
        writer = csv.DictWriter(handle, fields, lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)


def values_changed(row: dict[str, str], previous: dict[str, str] | None) -> bool:
    if previous is None:
        return True
    return any(row[field] != previous[field] for field in HISTORY_FIELDS[3:])


def cmd_record(args: argparse.Namespace) -> int:
    rev = git("rev-parse", args.rev or "HEAD")
    rows = [row for row in read_history() if row["commit"] != rev]
    row = measure_progress(args.rev)
    previous = max(
        (r for r in rows if int(r["ordinal"]) < int(row["ordinal"])),
        key=lambda r: int(r["ordinal"]),
        default=None,
    )
    if not values_changed(row, previous):
        print(f"no change at {row['commit'][:8]}; nothing recorded")
        return 0
    rows.append(row)
    write_history(rows)
    print(f"recorded {row['commit'][:8]} at {int(row['size']):,} bytes")
    return 0


def escape(text: str) -> str:
    return text.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")


def nice_ceiling(value: float) -> float:
    for step in (1, 2.5, 5, 10, 25, 50, 100):
        if value <= step:
            return float(step)
    return 100.0


def render_graph(rows: list[dict[str, str]]) -> str:
    top, right, bottom, left = MARGIN
    plot_w, plot_h = WIDTH - left - right, HEIGHT - top - bottom
    times = [datetime.fromisoformat(row["date"]).timestamp() for row in rows]
    total = int(rows[-1]["total_size"])
    data_span = max(times[-1] - times[0], 1.0)
    now = datetime.now(tz=datetime.fromisoformat(rows[-1]["date"]).tzinfo).timestamp()
    pad = min(max(now - times[-1], 0.08 * data_span), 0.25 * data_span)
    end = times[-1] + pad
    span = max(end - times[0], 1.0)
    ceiling = nice_ceiling(100.0 * int(rows[-1]["size"]) / total)

    def x_of(time: float) -> float:
        return left + plot_w * (time - times[0]) / span

    def y_of(percent: float) -> float:
        return top + plot_h * (1.0 - percent / ceiling)

    out = [
        (
            f'<svg xmlns="http://www.w3.org/2000/svg"'
            f' viewBox="0 0 {WIDTH} {HEIGHT}" role="img"'
            ' aria-label="Decompilation progress over time">'
        ),
        (
            "<style>"
            "text{font:11px system-ui,sans-serif;fill:#666}"
            ".title{font-size:13px;font-weight:600;fill:#222}"
            ".axis{stroke:#ddd;stroke-width:1}"
            "@media (prefers-color-scheme:dark){"
            "svg{background-color:#0d1117}"
            "text{fill:#999}"
            ".title{fill:#fff}"
            ".axis{stroke:#344}"
            "}"
            "</style>"
        ),
    ]

    for index in range(5):
        percent = ceiling * index / 4
        y = y_of(percent)
        out.append(
            f'<line class="axis" x1="{left}" y1="{y:.1f}"'
            f' x2="{left + plot_w}" y2="{y:.1f}" opacity="0.6"/>'
        )
        out.append(
            f'<text x="{left - 8}" y="{y + 4:.1f}" text-anchor="end">'
            f"{percent:.2f}%</text>"
        )

    keys = [(name, color, name.lower()) for name, color in COLORS]
    floors = [0.0] * len(rows)
    for name, color, field in keys:
        tops = [
            floor + 100.0 * int(row[field]) / total for floor, row in zip(floors, rows)
        ]
        upper, lower = [], []
        for index, time in enumerate(times):
            if index:
                upper.append(f"{x_of(time):.1f},{y_of(tops[index - 1]):.1f}")
                lower.append(f"{x_of(time):.1f},{y_of(floors[index - 1]):.1f}")
            upper.append(f"{x_of(time):.1f},{y_of(tops[index]):.1f}")
            lower.append(f"{x_of(time):.1f},{y_of(floors[index]):.1f}")
        upper.append(f"{x_of(end):.1f},{y_of(tops[-1]):.1f}")
        lower.append(f"{x_of(end):.1f},{y_of(floors[-1]):.1f}")
        points = " ".join(upper + lower[::-1])
        out.append(f'<polygon points="{points}" fill="{color}" />')
        floors = tops

    out.append(
        f'<line class="axis" x1="{left}" y1="{top + plot_h}"'
        f' x2="{left + plot_w}" y2="{top + plot_h}"/>'
    )
    for index, time in enumerate(times):
        if index and index != len(times) - 1:
            continue
        label = datetime.fromisoformat(rows[index]["date"]).strftime("%Y-%m-%d")
        anchor = "start" if index == 0 else "middle"
        out.append(
            f'<text x="{x_of(time):.1f}" y="{top + plot_h + 18}"'
            f' text-anchor="{anchor}">{label}</text>'
        )

    percent = 100.0 * int(rows[-1]["size"]) / total
    out.append(
        f'<text class="title" x="{left}" y="20">Matched decompilation:'
        f" {percent:.3f}% ({int(rows[-1]['size']):,} / {total:,} bytes,"
        f" {rows[-1]['units']} / {rows[-1]['total_units']} units)</text>"
    )
    for index, (name, color, _) in enumerate(keys):
        x = left + index * 128
        out.append(f'<rect x="{x}" y="28" width="9" height="9" fill="{color}"/>')
        out.append(f'<text x="{x + 14}" y="36">{escape(name)}</text>')
    out.append("</svg>")
    return "\n".join(out) + "\n"


def cmd_graph(args: argparse.Namespace) -> int:
    rows = read_history()
    GRAPH_SVG.write_text(render_graph(rows))
    print(f"wrote {GRAPH_SVG.relative_to(ROOT)} ({len(rows)} points)")
    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Decomp progress tool")
    sub = parser.add_subparsers(dest="cmd", required=True)

    p = sub.add_parser("summary", help="generate summary")
    p.add_argument("--rev", help="measure this revision instead of HEAD")
    p.set_defaults(handler=cmd_summary, rev=None)

    p = sub.add_parser("checklist", help="generate checklist")
    p.add_argument("--rev", help="measure this revision instead of HEAD")
    p.set_defaults(handler=cmd_checklist, rev=None)

    p = sub.add_parser("record", help="record progress to CSV")
    p.add_argument("--rev", help="measure this revision instead of HEAD")
    p.set_defaults(handler=cmd_record, rev=None)

    p = sub.add_parser("graph", help="generate graph SVG")
    p.set_defaults(handler=cmd_graph)

    args = parser.parse_args(argv)
    return args.handler(args)


if __name__ == "__main__":
    sys.exit(main())
