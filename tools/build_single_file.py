from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "src"
OUT = ROOT / "main.c"

HEADERS = ["model.h", "data.h", "ui.h", "auth.h", "user.h", "manager.h"]
SOURCES = ["model.c", "data.c", "ui.c", "auth.c", "user.c", "manager.c", "main.c"]


def strip_local_includes(text: str) -> str:
    lines = []
    for line in text.splitlines(keepends=True):
        if re.match(r"\s*#\s*include\s*\"", line):
            continue
        lines.append(line)
    return "".join(lines)


def read_utf8(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig")


parts = ["#define _CRT_SECURE_NO_WARNINGS 1\n"]
for name in HEADERS:
    parts.append(f"\n/* ========== src/{name} ========== */\n")
    parts.append(strip_local_includes(read_utf8(SRC / name)))

for name in SOURCES:
    parts.append(f"\n/* ========== src/{name} ========== */\n")
    parts.append(strip_local_includes(read_utf8(SRC / name)))

OUT.write_text("".join(parts), encoding="utf-8-sig", newline="\n")
print(f"generated {OUT}")
