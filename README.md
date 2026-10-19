# Seifert

An interactive three-block ribbon experiment in a genuine **two-dimensional
positive orthant**, rather than three cubes laid out on one line:

```text
  upper
    □
    ║ ribbon 0
    ║
    □══════□
  corner    right
       ribbon 1
```

The corner block joins the upper and right blocks through two perpendicular
ribbons. All three cubes rotate independently about a shared 3D spatial
diagonal (through each cube's own center), while the ends of each ribbon
follow the rotating faces to which they attach.

Grab a cube and drag horizontally to turn it. Its hit area is larger than
the visible cube and remains captured even as your finger moves across
another block or well away from the starting target. Reversing the gesture
untwists the scene; releasing keeps all three angles.

This is a first interactive **ribbon-twist** model, not a Seifert-surface
solver or a guaranteed nonsingular Dirac belt-trick contraction. A complete
2π turn of one cube returns its visible orientation but can leave an
interior ribbon twist.

## Implementation

The design follows
[isomorphismes/spinor](https://github.com/isomorphismes/spinor):

- `types/Seifert.idric`: Idriç semantic/type sketch, not compiled;
- `native/seifert.[ch]`: checked host-tested independent 3D rotations and
  connected ribbon samples;
- `android/seifert_view.[ch]`: shared camera projection, large nearest-center
  grab targets, and persistent pointer capture, independently host-tested;
- `android/seifert_android.c`: NativeActivity/Android pointer lifecycle;
- `android/seifert_renderer.c`: OpenGL ES 2 renderer;
- `android/build-*.sh`: canonical android-NDK signing and packaging route.

`ICK=/absolute/qualified/ick sh native/test-host.sh` verifies the orthant, all six attachment pairs,
independent turning, 2π/undo, camera/picking consistency, and off-block
drag persistence. CI builds signed MIRO A1 and C67 APK artifacts.

Jason Hise's belt-trick/antitwister visualizations inspire the visual
experiment. No unreleased Hise source code is included.

See `native/README.md` and `android/README.md`.

## Mathematical reading list

- [Books: arithmetic topology and supporting topology](books/README.md) —
  Morishita's *Knots and Primes* (including its 2024 revision) and
  Milnor's *Singular Points of Complex Hypersurfaces*.
- [Papers: Alexander, Seifert, and Ghys](papers/README.md) —
  original knot and covering invariants, Seifert surfaces, and
  connections with dynamical systems.

These are references for future Idriç mathematical definitions, not a
claim that the current ribbon viewer computes those invariants. In particular,
a geometric twist is not automatically a change of knot type.
