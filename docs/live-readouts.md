# Live endpoint rotations

The existing three-block experiment now draws two coloured numeric readouts:
upper minus corner, and right minus corner, in signed turns. Dragging the
corner changes both; dragging an outer block changes its attached diagnostic.
One positive full turn of the corner gives −1 and −1. Unwrapped history is
retained, so two turns do not collapse to zero because the final cube
orientation repeats.

These are commanded relative rotations about the shared block axis. They do
not measure rotation about the ribbon tangent. No geometric twist, writhe,
linking number or genus is inferred from these open bands.

The implementation preserves the compositional ICK geometry source, narrow
C ABI, source-to-assembly ICK frontend, NDK assembly/link boundary, package ID
and registered development signer. The readout consumer explicitly uses
binary32, matching the application's angle ABI. Idriç's existing binary64
host specification is not packaged as an Android executable.

`_/check-live-readouts.grease` requires the qualified ICK compiler and already
provisioned EGL/GLES headers. It executes signed fixtures, common-rotation
invariance, retained turns, invalid input and formatting checks, then draws
the actual application renderer in a real host EGL context. The pixel test
checks visible text, changed values, unchanged diagnostics under a common
turn, rejected invalid scene replacement, renderer recreation, and resize.
The existing 343-state geometry/reference comparison remains enabled.

Local host checks use the pinned ICK frontend and Mesa's GLES implementation.
The verified Grease owner runtime is an ASan build; LeakSanitizer cannot run
under this execution environment's process tracing. The initial script exit
therefore failed after its semantic/render checks passed. The repeat scopes
`ASAN_OPTIONS=detect_leaks=0` to that shell runtime and exits successfully.
This is not leak-check acceptance. CI retains the unmodified runtime settings.

Version code 3 / version name 0.3 preserves the package update identity.
Signed ARMv7/AArch64 packaging and physical installation/gesture/lifecycle
acceptance are separate boundaries; no phone result is inferred from the
host image or pixel checks.
