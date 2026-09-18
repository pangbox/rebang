.DEFAULT_GOAL := all
.DELETE_ON_ERROR:

.PHONY: all clean verify sources check format format-check progress
all: build/ProjectG_ReleaseQA.exe docs/progress.md

progress: docs/progress.md

docs/progress.md: build.json docs/module-sizes.csv tools/progress.py
	@python3 tools/progress.py checklist

ifneq ($(filter-out clean check format format-check progress,$(or $(MAKECMDGOALS),all)),)
include build/rules.mk
endif

build/rules.mk compile_commands.json &: build.json tools/build.py Makefile
	@mkdir -p build
	@python3 tools/build.py rules

verify: all
	@python3 tools/build.py verify

clean:
	@python3 tools/build.py clean

check:
	ruff check
	ruff format --check
	ty check

FORMAT_SOURCES = $(wildcard $(shell git ls-files --cached --others \
	--exclude-standard -- 'source/*.c' 'source/*.cpp' 'source/*.h' \
	'source/*.inl'))

format:
	clang-format -i $(FORMAT_SOURCES)

format-check:
	clang-format --dry-run -Werror $(FORMAT_SOURCES)
