# Native three-block ribbon model

The executable model is deliberately small and Android-independent.
`../icky/seifert.c` is the authoritative, compositional ICKY C translation
unit. `seifert.h` is the stable C ABI: 3 cube centers, 3 signed unwrapped
angles, 2 ribbon meshes, and edge geometry attached to rotating cube faces.
There is **no maintained ordinary-C copy** of the mathematical source.

The source expresses:
`orthant_connections → attachment_pair → centerline_at →
sample_ribbon_cross_section → sample_one_ribbon → seifert_sample_ribbons`.
The mathematical edges of the two cubes, not the GLES renderer, determine
the ribbon topology and geometry.

Host and Android builds compile the maintained source directly with ICK
c61e4482. NDK r27c assembles and links Android output. The historical Python
adapter is no longer a build producer. Exact pins and qualification are in
[division-migration.md](../docs/division-migration.md).

## Positive two-dimensional orthant

The cube centers lie in the positive (x, y) quadrant of the z=0 plane:
upper (1.90, 4.40), corner (1.90, 1.90), right (4.40, 1.90).
One strip runs north (+y) from the corner to the upper block; the other
runs east (+x) from the corner to the right block.

Each cube spins about a spatial diagonal through its own center,
`(1, 1, 1)/sqrt(3)`. This single rotation axis per cube is intentionally
usable at the corner despite its two **perpendicular ribbon faces**. The
angles are separate state values, so turning the corner alters both
ribbons while turning either outer cube affects only its own.

## Exact attached edges

A band root on block `i` has a local face normal `n`, transverse width
direction `w`, half cube size `h`, band half width `b`, cube center
`C_i`, and rotation `R_i`. Its actual two vertices are

`C_i + R_i(h*n - b*w)` and `C_i + R_i(h*n + b*w)`.

Thus both edge vertices lie on the current rigid face, not on a
renderer-invented fixed center. The centerline between root centers is a
cubic Hermite curve with the two rotated face normals as end tangents.
The band transverse vector interpolates using the full unwrapped angles:

```
theta(u) = (1 - smoothstep(u)) * angle_start
             + smoothstep(u) * angle_end
smoothstep(u) = u*u*(3 - 2*u)
width(u) = R(theta(u)) * width_direction
```

This is a deterministic ruled band, **not** a collision-free ambient
isotopy, a cloth simulation, or a checked Seifert spanning surface.
Large relative turns can still cause folds or surface intersections.

## Host acceptance

Run `ICK=/absolute/qualified/ick sh native/test-host.sh`. Tests check a non-collinear, perpendicular
orthant, exact ribbon end-face attachment before and after unrelated
cube turns, independent block effects, 2π cube return with interior twist,
reversibility, buffer checks, picking at the same positions drawn by GLES,
a 36%-larger hit radius on MIRO A1, and persistence of captured touch
well beyond the original block. Android pointer details remain below the
platform adapter, with a small platform-independent gesture state for tests.
