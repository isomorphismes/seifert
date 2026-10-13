#!/usr/bin/env python3
"""Explicit ordinary-C adapter for a first-party ICKY C translation unit.

This is NOT an ICK compiler or a substitute for ICK qualification.
Only the documented U+2190 C assignment token is adapted. Strings,
characters and comments are byte-for-byte preserved; unsupported
non-ASCII tokens and ordinary-C assignment in first-party source fail.

Usage: python3 tools/normalize_icky_c.py icky/seifert.c build/ndk/seifert.c
"""
from __future__ import annotations

import hashlib
from pathlib import Path
import sys


def normalize(source: str) -> tuple[str, int]:
    output: list[str] = []
    state = "code"
    index = 0
    arrows = 0

    while index < len(source):
        current = source[index]
        following = source[index + 1] if index + 1 < len(source) else ""

        if state == "line_comment":
            output.append(current)
            index += 1
            if current == "\n":
                state = "code"
            continue

        if state == "block_comment":
            if current == "*" and following == "/":
                output.extend(("*/",))
                index += 2
                state = "code"
            else:
                output.append(current)
                index += 1
            continue

        if state in ("string", "character"):
            output.append(current)
            index += 1
            if current == "\\":
                if index == len(source):
                    raise ValueError("unterminated escape in literal")
                output.append(source[index])
                index += 1
            elif current == ('"' if state == "string" else "'"):
                state = "code"
            continue

        if current == "/" and following in ("/", "*"):
            output.extend((current, following))
            index += 2
            state = "line_comment" if following == "/" else "block_comment"
            continue

        if current in ('"', "'"):
            state = "string" if current == '"' else "character"
            output.append(current)
            index += 1
            continue

        if current == "←":
            output.append("=")
            arrows += 1
            index += 1
            continue

        if ord(current) > 127:
            raise ValueError(
                f"unsupported executable glyph U+{ord(current):04X} at offset {index}"
            )

        if current == "=":
            previous = source[index - 1] if index else ""
            # ==, !=, <=, >= are comparisons, not assignment.
            if previous not in ("=", "!", "<", ">") and following != "=":
                raise ValueError(
                    f"ordinary C assignment at offset {index}; use ← in ICKY C"
                )

        output.append(current)
        index += 1

    if state in ("block_comment", "string", "character"):
        raise ValueError(f"unterminated {state}")
    if arrows == 0:
        raise ValueError("no ICKY assignment tokens found")
    return "".join(output), arrows


def main() -> int:
    if len(sys.argv) != 3:
        print(__doc__, file=sys.stderr)
        return 2

    source_path = Path(sys.argv[1])
    output_path = Path(sys.argv[2])
    if source_path.resolve() == output_path.resolve():
        raise ValueError("source and generated ordinary-C output must differ")

    source = source_path.read_text(encoding="utf-8")
    ordinary, arrow_count = normalize(source)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(ordinary, encoding="utf-8")

    print("SEIFERT_C_ROLE\tICKY_C_SOURCE_OF_TRUTH")
    print("SEIFERT_C_COMPATIBILITY\tEXPLICIT_NDK_NORMALIZATION_NOT_ICK")
    print(f"ICKY_ASSIGNMENTS\t{arrow_count}")
    print(f"ICKY_SOURCE_SHA256\t{hashlib.sha256(source.encode()).hexdigest()}")
    print(f"NDK_NORMALIZED_SHA256\t{hashlib.sha256(ordinary.encode()).hexdigest()}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
