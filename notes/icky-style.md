# Seifert source-language contract

This repository follows the user's **functorial, Icky C** convention.
The shape of the mathematics must be visible in the executable source,
not concealed in a single large Android/renderer procedure.

## The one authoritative mathematical source

`icky/seifert.c` is first-party ICKY C.
`native/seifert.h` is its stable, narrow C ABI.
`types/Seifert.idric` is the independent Idriç type sketch, **not** an
executable or a generator.

The actual call graph for sampling is:

```text
orthant_connections
  → attachment_pair
      → rotated_attachment_root
      → rotate_around_block_diagonal
  → centerline_at
      → cubic_hermite_weights
      → weighted_sum
  → ribbon_rotation_at
      → smooth_ribbon_parameter
  → sample_ribbon_cross_section
  → sample_one_ribbon
  → seifert_sample_ribbons
  → GPU upload (in Android renderer, not here)
```

The explicitly represented objects are:
`SeifertRibbonConnection`, `SeifertRibbonAttachments`,
`SeifertHermiteWeights`, `SeifertVec3`, `SeifertScene`, and
`SeifertRibbonMesh`. An attachment diagram determines the two bands
instead of scattering block-number tests throughout the sampler.

Naming is descriptive `snake_case`. Functions call named operations
and return mathematical objects or checked status. Renderer and Android
pointer/lifecycle code must not own formulas for ribbon geometry.

## Executable symbols — no invented compiler promises

The pinned ICK C language source
[`dilapidated-shed/ick`](https://github.com/dilapidated-shed/ick)
currently documents **U+2190 `←` assignment/initializer** as a
qualified C extension; both `x ← y` and
`point ← (SeifertVec3){...}` have real parser meaning.

C's `==`, `!=`, relational comparisons, pointer `*`, `->`,
numeric `*` and `-` retain their ordinary executable spellings.
The desired mathematical symbols `≟`, `×`, `−`, `√` and `λ`
are welcome in Idriç and human-facing mathematics, but **are not
invented as executable C tokens** until the appropriate ICK frontend
and acceptance tests prove them. In particular, this pass has not
silently rewritten them or claimed that `*` is pointer-only in the
qualified compiler.

## Compiler, provenance and build statuses

The maintained host and Android producers compile the unmodified
`icky/seifert.c`, including `←` and `÷`, directly through pinned ICK.
NDK r27c separately assembles and links Android output, then packages the APK.
The former Python lexical adapter was an earlier compatibility experiment;
it and its adapter-only tests are retired in Git history. The exact compiler
pins and evidence boundaries are in `docs/division-migration.md`.

## Behavioral boundary

No elastic force model, geometrical smoothing, new ribbon topology,
new block controls, or scene mutation belongs in this source-style pass.

The historical v0.2 C implementation is **test-only** and frozen in
`qualification/v02/seifert_reference.c`. It is not an editable fallback
or a second production geometry engine. The reference and ICKY source
are compiled in isolation and compared over 343 separate block-angle
configurations, multiple segment counts, and exact triangle indices;
every normal host test remains enabled.

A source-style pull request fails closed if the direct ICK reference
comparison, host geometry tests, or paired APK build fail.
