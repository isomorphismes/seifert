# Android orthant playground

The three cubes form an **L**, with one at the corner and outer cubes
above and to the right. The corner cube is attached to both ribbons.
Touch any block and drag horizontally to rotate it about its spatial
diagonal, then reverse direction to undo. Release keeps the pose.

The invisible grab disk for each cube now has a 19%-of-short-screen-side
radius (previously 14%), with a minimum 48 px radius. Nearest-center
selection resolves any overlap. Crucially, the exact same 4×4 projection
matrix is used to draw the blocks and locate their touch centers.

**Sticky capture:** selection happens once, at pointer-down. The original
finger retains its block through all subsequent moves, including over the
other blocks and outside the original grab disk, until that finger lifts,
is canceled, or the Activity is paused/resized. Other fingers do not
steal an existing grab. Multi-touch does not change the selected block.

This is still the architecture inherited from Spinor:

```text
types/Seifert.idric       design-only semantic sketch
native/seifert.[ch]       host-tested C geometry and per-block angles
android/seifert_view.c   camera / touch targets / grab capture
android/seifert_android.c  NativeActivity input, EGL and saved state
android/seifert_renderer.c GLES2 drawing
android-NDK packager     canonical signed APK
```

No DEX, Java or Kotlin. The renderer does not invent ribbon geometry.
The three unwrapped angles survive Activity state restoration and
ordinary surface re-creation.

## Build

Run host acceptance:

```sh
ICK=/absolute/qualified/ick sh native/test-host.sh
```

For MIRO A1, compile and package against the same canonical NDK and test
signer as the previous working A1 APK, with a bumped versionCode 2 so it
installs over version 1 without erasing saved app data:

```sh
ICK_CC=/absolute/qualified/arm-linux-gnueabi-gcc \
ANDROID_ABI=armeabi-v7a ANDROID_NDK_HOME=/absolute/android-ndk-r27c \
  bash android/build-native.sh
```

Then set `ANDROID_NDK_CHECKOUT` and the
`SEIFERT_KEYSTORE`, `SEIFERT_KEYSTORE_TYPE`, `SEIFERT_KEY_ALIAS`,
`SEIFERT_STORE_PASSWORD`, `SEIFERT_KEY_PASSWORD`, and
`SEIFERT_EXPECTED_CERT_SHA256` signer variables; run

```sh
ANDROID_ABI=armeabi-v7a bash android/build.sh
```

CI builds and uploads both MIRO A1 (`armeabi-v7a`) and
MIRO C67 (`arm64-v8a`) signed APKs using that packager. A1 remains
the priority target if only one can be produced. Physical acceptance
is separate from host and CI build success.

Camera orbit, cloth physics, arbitrary attachment graphs, Seifert
surface invariant checks and topological untangling remain deferred.
