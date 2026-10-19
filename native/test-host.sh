#!/bin/sh
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT HUP INT TERM
: "${ICK:?Set ICK to the qualified native compiler}"
"$ICK" ${ICK_FLAGS:--fno-link-libatomic} -std=c11 -Wall -Wextra -Werror -pedantic \
    -I "$root/native" \
    "$root/icky/seifert.c" "$root/android/seifert_view.c" \
    "$root/native/test_seifert.c" \
    -lm -o "$work/test_seifert"

# Historical C is test-only provenance. Rename its public functions at the
# compiler boundary to link it alongside the one authoritative ICKY source.
"$ICK" ${ICK_FLAGS:--fno-link-libatomic} -std=c11 -Wall -Wextra -Werror -pedantic \
    -I "$root/native" \
    -Dseifert_scene_init=seifert_v02_scene_init \
    -Dseifert_scene_turn=seifert_v02_scene_turn \
    -Dseifert_scene_set_angles=seifert_v02_scene_set_angles \
    -Dseifert_scene_valid=seifert_v02_scene_valid \
    -Dseifert_ribbon_vertex_count=seifert_v02_ribbon_vertex_count \
    -Dseifert_ribbon_index_count=seifert_v02_ribbon_index_count \
    -Dseifert_sample_ribbons=seifert_v02_sample_ribbons \
    -Dseifert_sample_blocks=seifert_v02_sample_blocks \
    -c "$root/qualification/v02/seifert_reference.c" \
    -o "$work/seifert_reference.o"

"$ICK" ${ICK_FLAGS:--fno-link-libatomic} -std=c11 -Wall -Wextra -Werror -pedantic \
    -I "$root/native" \
    "$root/icky/seifert.c" \
    "$work/seifert_reference.o" \
    "$root/qualification/v02/test_equivalence.c" \
    -lm -o "$work/test_equivalence"
"$work/test_equivalence"
"$work/test_seifert"
