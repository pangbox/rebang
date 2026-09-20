#!/usr/bin/env python3
"""Build tool for Rebang"""

from __future__ import annotations

import argparse
import fcntl
import functools
import hashlib
import json
import os
import re
import shutil
import subprocess
import tarfile
import tempfile
import zipfile
from collections.abc import Iterable, Iterator, Mapping, Sequence
from pathlib import Path, PurePosixPath
from typing import BinaryIO, TypedDict, cast


class EntryOptions(TypedDict, total=False):
    path: str
    library: str
    members: list[Entry]
    member: str
    patch: str
    flags: list[str]
    includes: list[str]
    extra_flags: list[str]
    dependencies: list[str]
    pch: str


Entry = str | EntryOptions
PathArgument = str | Path
ArchiveData = bytes | bytearray


class OptionalBuildConfig(TypedDict, total=False):
    precompiled_headers: list[Entry]
    source_archives: dict[str, Entry]


class BuildConfig(OptionalBuildConfig):
    inputs: list[Entry]
    cflags: list[str]
    includes: list[str]
    link_flags: list[str]


ROOT = Path(__file__).resolve().parents[1]
PREFIX = Path(os.environ.get("PROJECTG_WINEPREFIX", ROOT / ".wine")).resolve()
CONFIG = ROOT / "build.json"
BIN = ROOT / "tools/VC7.1/bin"
IMAGE = Path("build/ProjectG_ReleaseQA.exe")
LINKED = Path("build/ProjectG_ReleaseQA.link.exe")
IMAGE_SIZE = 7618560
EXPECTED = "761252876446178ad5190e78656aa8790b9dbaf2e86a248f31bb4678347e44fc"
HEADER_SUFFIXES = ("", ".h", ".hpp", ".hxx", ".inl", ".inc")
CLANG_TARGET = "i386-pc-windows-msvc"
MSC_VERSION = "13.10"
IDENTITY = (
    (320, "e1e6a04e"),
    (6184036, "e1e6a04e"),
    (6594392, "b0c094b607038c45a37f50dcaaff0630"),
)


def path(entry: Entry) -> str:
    return entry if isinstance(entry, str) else entry["path"]


def product(entry: Entry) -> str:
    if isinstance(entry, dict) and "library" in entry:
        return "build/" + entry["library"]
    source = path(entry)
    if isinstance(entry, dict) and "member" in entry:
        return str(Path("build", source).with_suffix("") / entry["member"])
    suffix = Path(source).suffix.lower()
    if suffix == ".rc":
        return str(Path("build", source).with_suffix(".res"))
    elif suffix in (".c", ".cpp", ".cxx", ".h"):
        return str(Path("build", source).with_suffix(".obj"))
    else:
        return source


def settings() -> tuple[BuildConfig, dict[str, Entry]]:
    config: BuildConfig = json.loads(CONFIG.read_text())
    entries = list(config.get("precompiled_headers", []))
    for entry in config["inputs"]:
        if isinstance(entry, dict) and "library" in entry:
            defaults: EntryOptions = {}
            if "flags" in entry:
                defaults["flags"] = entry["flags"]
            if "includes" in entry:
                defaults["includes"] = entry["includes"]
            if "extra_flags" in entry:
                defaults["extra_flags"] = entry["extra_flags"]
            for member in entry["members"]:
                member_options: EntryOptions
                if isinstance(member, str):
                    member_options = {"path": member}
                else:
                    member_options = member
                merged_options: EntryOptions = {**defaults, **member_options}
                entries.append(merged_options)
        else:
            entries.append(entry)
    return config, {path(e): e for e in entries}


def pch_product(source: str) -> str:
    return str(Path("build", source).with_suffix(".pch"))


def unpack_stamp(directory: str) -> str:
    return str(Path("build", directory) / ".unpacked")


def source_files(archive: PathArgument) -> Iterator[tuple[Path, bytes]]:
    def relative(name: str) -> Path:
        p = PurePosixPath(name)
        if p.is_absolute() or ".." in p.parts or len(p.parts) < 2:
            raise ValueError(f"invalid source archive path: {name}")
        return Path(*p.parts[1:])

    if zipfile.is_zipfile(archive):
        with zipfile.ZipFile(archive) as z:
            for member in z.infolist():
                if not member.is_dir():
                    yield relative(member.filename), z.read(member)
    else:
        with tarfile.open(archive) as t:
            for tar_member in t:
                if tar_member.isfile():
                    stream = cast(BinaryIO, t.extractfile(tar_member))
                    yield relative(tar_member.name), stream.read()


def unpack(directory: str) -> None:
    entry: Entry = json.loads(CONFIG.read_text())["source_archives"][directory]
    dest = ROOT / directory
    dest.parent.mkdir(parents=True, exist_ok=True)
    print("UNPACK", path(entry), flush=True)
    with tempfile.TemporaryDirectory(dir=dest.parent) as temporary:
        tree = Path(temporary) / "source"
        for relative, data in source_files(ROOT / path(entry)):
            output = tree / relative
            output.parent.mkdir(parents=True, exist_ok=True)
            output.write_bytes(data)
        if isinstance(entry, dict) and "patch" in entry:
            subprocess.run(
                [
                    "patch",
                    "--binary",
                    "--batch",
                    "--fuzz=0",
                    "-p1",
                    "-i",
                    str(ROOT / entry["patch"]),
                ],
                cwd=tree,
                check=True,
            )
        if dest.exists():
            shutil.rmtree(dest)
        tree.rename(dest)
    stamp = Path(unpack_stamp(directory))
    stamp.parent.mkdir(parents=True, exist_ok=True)
    stamp.touch()


def clean() -> None:
    for directory in json.loads(CONFIG.read_text()).get("source_archives", {}):
        dest = ROOT / directory
        if dest.exists():
            shutil.rmtree(dest)
    shutil.rmtree(ROOT / "build", ignore_errors=True)
    (ROOT / "compile_commands.json").unlink(missing_ok=True)


def windows(p: PathArgument) -> str:
    return "Z:" + str((ROOT / p).resolve()).replace("/", "\\")


def environment() -> dict[str, str]:
    return dict(
        os.environ,
        WINEPREFIX=str(PREFIX),
        WINEDEBUG="-all",
        WINEDLLOVERRIDES="mscoree,mshtml=",
        WINEPATH=windows(BIN),
    )


def prepare_wine() -> None:
    (ROOT / "build").mkdir(exist_ok=True)
    with (ROOT / "build/wine.lock").open("w") as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        marker = PREFIX / ".projectg-ready"
        if not marker.exists():
            with (ROOT / "build/wineboot.log").open("w") as log:
                subprocess.run(
                    ["wineboot", "-u"],
                    env=environment(),
                    stdout=log,
                    stderr=subprocess.STDOUT,
                    check=True,
                )
            marker.touch()
        drive = ROOT / "build/pdb-drive"
        (drive / "Build/Custom/temp/bin").mkdir(parents=True, exist_ok=True)
        link = PREFIX / "dosdevices/d:"
        if link.is_symlink() or link.exists():
            if link.resolve() != drive:
                raise ValueError(f"Wine D: points elsewhere: {link}")
        else:
            link.symlink_to(drive)


def response(output: PathArgument, args: Iterable[PathArgument]) -> Path:
    output = Path(output)
    output.parent.mkdir(parents=True, exist_ok=True)
    rsp = output.with_suffix(output.suffix + ".rsp")
    rsp.write_bytes(
        ("\r\n".join('"' + str(a) + '"' for a in args) + "\r\n").encode("ascii")
    )
    return rsp


def invoke(
    tool: str, output: PathArgument, args: Sequence[str], cwd: PathArgument = ROOT
) -> str:
    prepare_wine()
    output = Path(output)
    output.parent.mkdir(parents=True, exist_ok=True)
    command = args if tool == "rc.exe" else ["@" + windows(response(output, args))]
    log = output.with_suffix(output.suffix + ".log")
    with log.open("w") as stream:
        result = subprocess.run(
            ["wine", str(BIN / tool), *command],
            cwd=cwd,
            env=environment(),
            stdout=stream,
            stderr=subprocess.STDOUT,
            check=False,
        )
    log_text = log.read_text(errors="replace")
    if result.returncode:
        raise RuntimeError(f"{tool} failed; see {log}\n{log_text}")
    return log_text


def make_escape(p: PathArgument) -> str:
    return (
        str(p)
        .replace("\\", "/")
        .replace("$", "$$")
        .replace("#", "\\#")
        .replace(" ", "\\ ")
    )


@functools.cache
def include_mirror(directory: str) -> str | None:
    """Build lowercase mirror of headers for clangd."""
    source = ROOT / directory
    mirror = ROOT / "build/include-mirror" / directory
    if not source.is_dir():
        return None
    links = {
        str(p.relative_to(source)).lower(): p
        for p in sorted(source.rglob("*"))
        if p.is_file() and p.suffix.lower() in HEADER_SUFFIXES
    }
    links = {n: p for n, p in links.items() if str(p.relative_to(source)) != n}
    shutil.rmtree(mirror, ignore_errors=True)
    if not links:
        return None
    for name, target in links.items():
        link = mirror / name
        link.parent.mkdir(parents=True, exist_ok=True)
        link.symlink_to(target)
    return str(mirror)


def clang_arguments(
    source: str, options: EntryOptions, config: BuildConfig, output: str
) -> list[str]:
    """Rough translation from MSVC command to clangd driver command."""
    flags = [*options.get("flags", config["cflags"]), *options.get("extra_flags", [])]
    includes = [*config["includes"], *options.get("includes", [])]
    c = Path(source).suffix.lower() == ".c" and "/TP" not in flags
    args = [
        "clang",
        f"--target={CLANG_TARGET}",
        f"-fms-compatibility-version={MSC_VERSION}",
        "-fms-extensions",
        "-fms-compatibility",
        "-Wno-switch",
        "-x",
        "c" if c else "c++",
        "-std=" + (("c89" if "/Za" in flags else "gnu89") if c else "c++03"),
        "-D_CLANGD=1",
    ]
    for flag in flags:
        if flag[:1] in "-/" and flag[1:2] in ("D", "U"):
            args.append("-" + flag[1:])
        elif flag[:1] in "-/" and flag[1:3] == "FI":
            args += ["-include", flag[3:]]
    args += ["-I" + str(ROOT / i) for i in includes]
    args += ["-I" + m for i in includes if (m := include_mirror(i)) is not None]
    args += ["-c", "-o", str(ROOT / output), str(ROOT / source)]
    return args


def compile_commands() -> None:
    config, entries = settings()
    pch_sources = {path(e) for e in config.get("precompiled_headers", [])}
    database: list[dict[str, object]] = []
    for source, entry in entries.items():
        if Path(source).suffix.lower() not in (".c", ".cpp", ".cxx", ".h"):
            continue
        options: EntryOptions = entry if isinstance(entry, dict) else {}
        if "member" in options:
            continue
        output = pch_product(source) if source in pch_sources else product(entry)
        database.append(
            {
                "directory": str(ROOT),
                "file": str(ROOT / source),
                "output": str(ROOT / output),
                "arguments": clang_arguments(source, options, config, output),
            }
        )
    (ROOT / "compile_commands.json").write_text(json.dumps(database, indent=2) + "\n")


def rules() -> None:
    config, entries = settings()
    lines = ["# Generated from build.json.", ""]
    source_targets: list[str] = []
    for directory, entry in config.get("source_archives", {}).items():
        stamp = unpack_stamp(directory)
        files = [
            str(Path(directory) / name) for name, _ in source_files(ROOT / path(entry))
        ]
        source_targets += [stamp, *files]
        dependencies = [path(entry), "build.json", "tools/build.py"]
        if isinstance(entry, dict) and "patch" in entry:
            dependencies.append(entry["patch"])
        lines += [
            f"build/rules.mk: {make_escape(path(entry))}",
            " ".join(map(make_escape, [stamp, *files]))
            + " &: "
            + " ".join(map(make_escape, dependencies)),
            f"\t@python3 tools/build.py unpack {directory}",
            "",
        ]
    lines += [
        "sources: " + " ".join(map(make_escape, source_targets)),
        "all: sources",
        "",
    ]
    tool_deps = " ".join(
        str(p.relative_to(ROOT)) for p in sorted(BIN.iterdir()) if p.is_file()
    )
    common = "build.json tools/build.py " + tool_deps
    outputs: set[str] = set()
    depfiles: list[str] = []
    pch_sources = {path(e) for e in config.get("precompiled_headers", [])}
    for source, entry in entries.items():
        precompile = source in pch_sources
        dest = pch_product(source) if precompile else product(entry)
        if dest == source:
            continue
        if dest in outputs:
            raise ValueError(f"conflicting sources for {dest}")
        outputs.add(dest)
        options: EntryOptions = entry if isinstance(entry, dict) else {}
        dependencies = list(options.get("dependencies", []))
        if "pch" in options:
            dependencies.append(pch_product(options["pch"]))
        resource = Path(source).suffix.lower() == ".rc"
        extract = "member" in options
        if resource:
            dependencies += [
                str(p)
                for p in Path(source).parent.iterdir()
                if p.suffix.lower() in (".ico", ".bmp", ".manifest", ".h", ".rc2")
            ]
        if extract:
            action = "extract"
        elif precompile:
            action = "pch"
        elif resource:
            action = "resource"
        else:
            action = "compile"
        lines += [
            f"{dest}: {source} "
            + " ".join(map(make_escape, dependencies))
            + " "
            + common
            + " | sources",
            f"\t@python3 tools/build.py {action} {source}",
            "",
        ]
        if not resource and not extract:
            depfiles.append(dest + ".d")
    for entry in config["inputs"]:
        if not isinstance(entry, dict) or "library" not in entry:
            continue
        dest = product(entry)
        members = [product(m) for m in entry["members"]]
        lines += [
            f"{dest}: " + " ".join(members) + " " + common,
            f"\t@python3 tools/build.py library {entry['library']}",
            "",
        ]
    lines += [
        f"{LINKED}: " + " ".join(product(e) for e in config["inputs"]) + " " + common,
        "\t@python3 tools/build.py link",
        "",
        f"{IMAGE}: {LINKED} tools/build.py",
        "\t@python3 tools/build.py normalize",
        "",
    ]
    if depfiles:
        lines.append("-include " + " ".join(depfiles))
    Path("build/rules.mk").write_text("\n".join(lines) + "\n")
    compile_commands()


def compile_source(source: str, precompile: bool = False) -> None:
    config, entries = settings()
    entry = entries[source]
    options: EntryOptions = entry if isinstance(entry, dict) else {}
    output = pch_product(source) if precompile else product(entry)
    flags = [*options.get("flags", config["cflags"]), *options.get("extra_flags", [])]
    includes = [*config["includes"], *options.get("includes", [])]
    pdb = Path(output).with_suffix(".pdb")
    if precompile or "pch" in options:
        pch_source = source if precompile else options["pch"]
        flags += [
            ("/Yc" if precompile else "/Yu") + Path(pch_source).with_suffix(".h").name,
            "/Fp" + windows(pch_product(pch_source)),
        ]
        pdb = Path(pch_product(pch_source)).with_suffix(".pdb")
    obj = output + ".obj" if precompile else output
    args = [
        "/nologo",
        "/c",
        "/showIncludes",
        *flags,
        *["/I" + windows(p) for p in includes],
        "/Fo" + windows(obj),
        "/Fd" + windows(pdb),
        windows(source),
    ]
    print("PCH" if precompile else "CL", source, flush=True)
    # Workaround for C1033 errors.
    pdb.parent.mkdir(parents=True, exist_ok=True)
    with pdb.with_suffix(".pdb.lock").open("w") as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        log = invoke("cl.exe", output, args)
    headers: list[str] = []
    actual_paths = {
        str(p).lower(): p
        for tree in (ROOT / "source", ROOT / "tools")
        for p in tree.rglob("*")
        if p.is_file()
    }
    for match in re.finditer(r"Note: including file:\s*(.+)", log):
        name = match[1].strip().replace("\\", "/")
        if name[:2].lower() == "z:":
            p = Path(name[2:])
            p = actual_paths.get(str(p.resolve()).lower(), p)
            if not p.exists():
                raise ValueError(f"cannot resolve included header: {name}")
            try:
                headers.append(str(p.relative_to(ROOT)))
            except ValueError:
                headers.append(str(p))
    headers = sorted(set(headers))
    Path(output + ".d").write_text(
        make_escape(output)
        + ": "
        + " ".join(map(make_escape, headers))
        + "\n"
        + "".join(make_escape(h) + ":\n" for h in headers)
    )


def resource(source: str) -> None:
    _, entries = settings()
    output = product(entries[source])
    print("RC", source, flush=True)
    invoke(
        "rc.exe",
        output,
        ["/c", "65001", "/l", "0x412", "/fo", windows(output), windows(source)],
        cwd=(ROOT / source).parent,
    )


def archive_members(data: ArchiveData) -> Iterator[tuple[int, str, ArchiveData]]:
    if data[:8] != b"!<arch>\n":
        raise ValueError("not a COFF archive")
    pos, names = 8, b""
    while pos < len(data):
        name = bytes(data[pos : pos + 16]).decode("ascii").rstrip()
        size = int(data[pos + 48 : pos + 58])
        if name == "//":
            names = bytes(data[pos + 60 : pos + 60 + size])
        elif name != "/":
            if name.startswith("/") and name[1:].isdigit():
                name = (
                    names[int(name[1:]) :]
                    .split(b"\0", 1)[0]
                    .split(b"\n", 1)[0]
                    .decode("ascii")
                )
            yield (
                pos,
                name.replace("\\", "/").rstrip("/").rsplit("/", 1)[-1],
                data[pos + 60 : pos + 60 + size],
            )
        pos += 60 + size + (size & 1)


def extract_member(source: str) -> None:
    _, entries = settings()
    entry = cast(EntryOptions, entries[source])
    matches = [
        body
        for _, name, body in archive_members(Path(source).read_bytes())
        if name == entry["member"]
    ]
    if len(matches) != 1:
        raise ValueError(f"expected one {entry['member']} in {source}")
    output = Path(product(entry))
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(matches[0])
    print("EXTRACT", source, entry["member"], flush=True)


def member_names(data: bytearray, overrides: Mapping[str, str]) -> None:
    """Retain short import-member names; LINK uses them when ordering .idata."""
    found: set[str] = set()
    for pos, name, _ in archive_members(data):
        if name in overrides:
            replacement = (overrides[name] + "/").encode("ascii")
            if len(replacement) > 16:
                raise ValueError(
                    "import member name must fit the short archive name field"
                )
            data[pos : pos + 16] = replacement.ljust(16, b" ")
            found.add(name)
    if found != set(overrides):
        raise ValueError("archive import name did not match an input")


def library(name: str) -> None:
    config, _ = settings()
    entry = next(
        e for e in config["inputs"] if isinstance(e, dict) and e.get("library") == name
    )
    output = product(entry)
    print("LIB", name, flush=True)
    members: list[str] = []
    overrides: dict[str, str] = {}
    for member in entry["members"]:
        source = product(member)
        if Path(source).suffix.lower() == ".lib":
            directory = Path(output + ".members") / Path(source).stem
            directory.mkdir(parents=True, exist_ok=True)
            for i, (_, original, body) in enumerate(
                archive_members(Path(source).read_bytes())
            ):
                obj = directory / f"{i:03d}_{Path(original).stem}.obj"
                obj.write_bytes(body)
                members.append(str(obj))
                if original.endswith(".dll"):
                    overrides[obj.name] = original
        else:
            members.append(source)
    Path(output).unlink(missing_ok=True)
    invoke(
        "lib.exe",
        output,
        [
            "/nologo",
            "/OUT:" + windows(output),
            *[windows(m) for m in reversed(members)],
        ],
    )
    if overrides:
        data = bytearray(Path(output).read_bytes())
        member_names(data, overrides)
        Path(output).write_bytes(data)


def link() -> None:
    config, _ = settings()
    prepare_wine()
    pdb = ROOT / "build/pdb-drive/Build/Custom/temp/bin/ProjectG_ReleaseQA.pdb"
    pdb.unlink(missing_ok=True)
    args = [
        *config["link_flags"],
        "/OUT:" + windows(LINKED),
        "/MAP:" + windows("build/ProjectG_ReleaseQA.map"),
        "/PDB:d:\\Build\\Custom\\temp\\bin\\ProjectG_ReleaseQA.pdb",
        *[windows(product(e)) for e in config["inputs"]],
    ]
    print("LINK", LINKED, flush=True)
    invoke("link.exe", LINKED, args)
    shutil.copyfile(pdb, ROOT / "build/ProjectG_ReleaseQA.pdb")


def normalize() -> None:
    data = bytearray(LINKED.read_bytes())
    if len(data) != IMAGE_SIZE:
        raise ValueError(
            f"unexpected image size: {len(data)}, expected {IMAGE_SIZE} (diff: {len(data) - IMAGE_SIZE})"
        )
    for offset, value in IDENTITY:
        replacement = bytes.fromhex(value)
        data[offset : offset + len(replacement)] = replacement
    actual = hashlib.sha256(data).hexdigest()
    if actual != EXPECTED:
        raise ValueError(f"image hash mismatch: {actual}")
    temporary = IMAGE.with_suffix(".tmp")
    temporary.write_bytes(data)
    temporary.replace(IMAGE)
    print(actual, IMAGE)


def verify() -> None:
    actual = hashlib.sha256(IMAGE.read_bytes()).hexdigest()
    if actual != EXPECTED:
        raise ValueError(f"image hash mismatch: {actual}")
    print(actual, IMAGE)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "action",
        choices=[
            "rules",
            "compile",
            "pch",
            "unpack",
            "extract",
            "resource",
            "library",
            "link",
            "normalize",
            "verify",
            "clean",
        ],
    )
    parser.add_argument("source", nargs="?")
    args = parser.parse_args()
    if (
        args.action in ("compile", "pch", "unpack", "extract", "resource", "library")
        and not args.source
    ):
        parser.error("this action requires a source path")

    if args.action == "rules":
        rules()
    elif args.action == "compile":
        compile_source(args.source)
    elif args.action == "pch":
        compile_source(args.source, True)
    elif args.action == "unpack":
        unpack(args.source)
    elif args.action == "extract":
        extract_member(args.source)
    elif args.action == "resource":
        resource(args.source)
    elif args.action == "library":
        library(args.source)
    elif args.action == "link":
        link()
    elif args.action == "normalize":
        normalize()
    elif args.action == "verify":
        verify()
    elif args.action == "clean":
        clean()
