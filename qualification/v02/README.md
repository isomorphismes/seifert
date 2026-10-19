# Installed v0.2 reference equivalence

The user's MIRO C67 physical evidence applies to the signed v0.2 APK from
`isomorphisms/seifert` PR #1, head commit
`eb1824cfb7acfd8344a03a0e399a6241e03f8724`.

`seifert_reference.c` is an **immutable historical snapshot** of
`native/seifert.c` from that commit (Git blob
`7a041a2783f6340070644dd405479a04c7c08849`).
It is not a maintained ICKY source or a second mathematical implementation.

For the source-style migration, the host test compiles this old C under
renamed public symbol names and compares its cube and ribbon vertices with
the new `icky/seifert.c` authoritative implementation, after explicitly
normalizing `←` to ordinary C assignment for the host compiler.

The regression grid spans 343 independent three-angle states and
varying sampling resolutions (including 1 and 512 segments). All ribbon
triangle indices must match exactly, while 3D float coordinates must match
to 3×10⁻⁵. The test establishes **semantic equivalence for the sampled
states**. It does not prove all floating-point paths equivalent, claim
elasticity, or qualify direct Android compilation by ICK.

The reference must not be silently updated to make a failing test pass.
