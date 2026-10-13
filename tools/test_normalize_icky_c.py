#!/usr/bin/env python3
"""Host tests for the explicit U+2190 compatibility adapter."""
from normalize_icky_c import normalize


def rejected(source: str) -> None:
    try:
        normalize(source)
    except ValueError:
        return
    raise AssertionError("unexpectedly accepted: " + repr(source))


def main() -> None:
    example = (
        'int x ← 2; // equality = and √ in a comment\n'
        'char *words ← "← and = are literal text";\n'
        "char symbol ← '=';\n"
        'int yes ← x == 2; /* x ← 9 is a comment */\n'
    )
    ordinary, count = normalize(example)
    assert count == 4
    assert 'int x = 2;' in ordinary
    assert 'char *words = "← and = are literal text";' in ordinary
    assert "char symbol = '=';" in ordinary
    assert "x == 2" in ordinary
    assert "// equality = and √ in a comment" in ordinary
    assert "/* x ← 9 is a comment */" in ordinary

    rejected("int x = 2;")
    rejected("int x → 2;")
    rejected("int x ← 2; int y × 3;")
    rejected("int x ← 2; /* unterminated")
    rejected('int x ← 2; "unterminated')
    rejected("int x ← 2; int y += 1;")
    print("Icky C compatibility adapter lexical tests: PASS")


if __name__ == "__main__":
    main()
