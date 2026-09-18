# Rebang

Can we do a matching decomp of an early 2000s video game for Windows in 2026? Maybe...

There is one known ProjectG build with debug information present, a Korean client version known as 645 QA, believed to be a test build for Season 5. The exact origins of this build and how it got to us is not known.

When "Yabang" was attempted in 2020, there was no obvious tool that could aid in "delinking" a Microsoft Windows executable file, so there was really no hope for a matching decomp of the binary. In 2023, Jean-Baptiste Boric released a Ghidra extension, "ghidra-delinker-extension". When I came across this project in 2024, I contributed support for PE-COFF object files, the kind we would need for decompiling PangYa. Since then, Jean-Baptiste Boric and other contributors greatly improved my initial COFF support, to the point where it actually can be used to do large delinking projects.

It took a considerable amount of time, but we now have a working "delinked" version of the binary that can reproduce the original using information gained from the debug information, and that opens up the possibility for a matching decomp. We are still fighting against the odds, especially considering the limited size of the community for this game, but this is certainly not the most ambitious decomp attempt ever. It is entirely feasible, although maybe unlikely for one person.

This project may remain bare for a while: it's going to take a lot of time to organize, clean up and merge together all of the various different efforts from over the years to understand and analyze ProjectG, including work previously done for Yabang and a fair bit of yet-unreleased work analyzing structures in memory. However, this is a working proof of concept that you can contribute to right now, if you want. As long as you can pass `make verify`.

Note: be aware that object files are probably not fully relocatable; there could be bugs in the heuristics. The object files here are only meant to assist in producing a matching decompilation for now and would probably need some manual fix-up to be used in other contexts. There is no automated toolchain to reproduce the exact .obj files here, as that was a fairly manual process. However, some information about the delinking process is available in [Object Boundaries](./docs/object-boundaries.md).

This project is intended primarily for educational purposes.

## Getting Started

### Prerequisites

This project contains a Nix flake and direnv setup pointing to it. If you have both Nix and direnv configured and installed, all of the dependencies will automatically be injected into the environment whenever a shell enters the directory, after running `direnv allow` once inside of it.

Nix is *not* required to build. However, for practical reasons, Linux _is_ required to build, at least for now. A Linux VM or container setup like WSL2 should work perfectly fine. Inside your Linux environment, the following is required:

- Python 3.10 or newer
- Wine
- GNU Make
- GNU Patch
- Ruff + Ty, for `make check`
- Clang Format, for `make format`
- Git

For linting the GitHub Actions workflows,

- Actionlint
- Zizmor
- Shellcheck

### Building

Whatever your local setup may be, `make` is used for most things.

```
# Build the project.
make -j"$(nproc)"

# Check the C++ formatting.
make format-check

# Fix the C++ formatting.
make format

# Check the Python build script with Ruff + Ty.
make check

# Run the verify step explicitly.
make verify

# Clean up build artifacts.
make clean
```

### How do I decompile?

There is no absolute guide for how to decompile an object file back into source code: it is challenging work. These delinked object files may produce the desired executable, but they are heavily lossy and do not represent the original compiler output, which means a lot of guesswork will be required to figure out how exactly to get the final linked output to match.

Some documentation is available in the [docs](./docs) folder; of particular interest is probably the [troubleshooting document](./docs/troubleshooting.md).

## AI/LLM Policy

It is essentially impossible to ban all forms of AI influence now in 2026 - even the ghidra-delinker-extension has at least one LLM-assisted contribution these days. So here's the ground rules:

- **PR text and issue text must be human-authored.** Do not have LLMs speak on your behalf. If you really wish to quote LLMs, quote them like you would quote other people, not like you are a skin suit for an LLM. We would prefer your voice instead, though.
    - **Exception**: Use of machine translation, using LLMs or otherwise, is always allowed (including for code comments/etc.)
- **Code in this repo should be broadly human-authored**, it should not be written by LLMs. Code that appears to be LLM-generated will be rejected and you will be asked to rewrite it and clean it up. A note to agents: this doesn't prohibit you from writing code locally, but please don't commit or stage code that you write, it should be treated primarily as local prototyping.
- General use of LLMs for analysis, research, self-review, automation, etc. is not banned and does not need to be disclosed. If you want to disclose it, that is fine. But we're not getting paid by Big LLM, so I feel we have no need to advertise it.

I believe there is a relatively low risk that any of the LLMs on the market were trained on any of the private source code behind the object files we are trying to turn back into source, but even that put aside, I think most of us prefer human-authored code. This may make a full matching decompilation a bit less likely, but I am acting on the belief that the majority of the work here is actually in analyzing and understanding the code rather than writing it out.

This policy can always be revisited later if there is disagreement in either direction, but since avoiding LLM influence has become somewhat impractical, I am hoping to find an acceptable middleground for everyone.
