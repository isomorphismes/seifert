# Division source and producers

The owned mathematical, view, renderer and acceptance C uses binary `÷`.
ICK c61e448251744a2f40ad743ebef1a027bdcd2f9d preserves ordinary `/`
semantics. Host and Android producers use immutable ai-ci
015cc7901ae0b3ad262b476f24e129b53c56db95. No source-normalization step or
alternative owned-C frontend is selected.

The native test entry point requires an explicit ICK executable and retains
strict warnings and its independent geometric assertions. The Android
producer compiles every owned translation unit through ICK to assembly;
NDK 27.2.12479018 assembles and links it with unchanged upstream
native_app_glue. API21, ARMv7/AArch64 package profiles, warning flags, page
alignment, package identity and the established public development signer
remain unchanged. Compiler builtin headers precede Bionic headers and both
Android API macros agree. Clang-only diagnostic pragmas are conditional on
Clang; ICK warning checks remain enabled.

Local acceptance with the exact frontend and actual r27c includes the
independent orthant/face/angle/attachment/projected-touch/sticky-drag host
tests and complete API21 ARMv7/AArch64 library compilation, assembly, link
and NativeActivity entry-point checks. The shared C-stage contract is
required by the host and APK workflows. Hosted APK checks and physical
installation/interaction retain their own acceptance boundary.

On the compositional branch, the historical v0.2 reference source remains
byte-for-byte frozen. Its equivalence runner compiles that reference and the
maintained ICK source separately; both are linked into the retained semantic
comparison. The old Python compatibility adapter is retained historical debt
but is no longer a producer of the host or Android application.
