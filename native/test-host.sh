#!/bin/sh
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT HUP INT TERM
: "${ICK:?Set ICK to the qualified native compiler}"
"$ICK" ${ICK_FLAGS:--fno-link-libatomic} -std=c11 -Wall -Wextra -Werror -pedantic \
    "$root/native/seifert.c" "$root/android/seifert_view.c" \
    "$root/native/test_seifert.c" \
    -lm -o "$work/test_seifert"
"$work/test_seifert"
