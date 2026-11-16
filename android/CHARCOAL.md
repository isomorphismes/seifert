# Charcoal presentation

The C palette and four resource files are byte-identical vendored copies from
android-NDK source `3e682fb1175beef5625dbd2418730220e49fc9bd`. The shared contract
workflow checks every copy against that immutable owner. The local fullscreen
alias preserves this application's window layout. No new UI framework is used.

The existing packager consumes these resources through `ANDROID_RES_DIR`.
Package, version and permanent development signer remain unchanged. Neutral
scene/background colors use the shared C roles; mathematical objects and their
semantic color identities retain their intended colors.

`check-charcoal.grease` has one checkout-path input and fails on a source-pin or
byte mismatch. It is bounded file/command orchestration through canonical Grease.
No language or compiler fallback is introduced. The fixed CI action restores the
same host engine from available artifact 13062637698 and the canonical portable
launcher, replacing the missing historical artifact.

Hosted execution must bind this exact product commit before acceptance. Signed
APK production alone does not establish Android visual acceptance. API29/34,
both system night modes, startup transitions, rotation and physical C67 checks
remain separately recorded until actually executed. No application is installed,
merged or released by this change.
