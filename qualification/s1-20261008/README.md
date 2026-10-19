# S1 Android and host qualification — 2026-10-08

Published application source: `68b41ba7b4624610813429102ebb8f404426b075` on
`style/functorial-icky-c`. The subsequent qualification commit adds only
the build contract, artifacts and evidence. No geometry or visual changes.

The local tested merge checkpoint `16d1a45e05895cea3355de0bc3465d6f72fb9a0c`
and published source have exactly the same tree
`03a46af9401f4b3b58459ab1cc922b2caaad576e`. Publication used authenticated
GitHub object/ref operations after ordinary git push lacked credentials.
All source paths were compared after fetching the published head. The final
APKs were repackaged with that published source SHA; their receipts are canonical.

The ICKY source digest is
`d0723c5aa2d199a3875685da17038a4b0c6d41eeafd7c8adf4e151a6cf240af6`.
The existing lexical adapter emits ordinary C with digest
`28bb198a694c5d30e94f0d40d6d87e66b5a33e279298785682737d3745c7d98c`.
This is an explicit syntax-only compatibility path, not direct ICK Android
compilation. The adapter and its inherited Python runtime were reused unchanged.

Host tests ran with the existing ICK scalar compiler (GCC 17 driver digest
`acde6cf158c527a415105cd692796c3c998a636f6887ab0d30262773e3611e91`,
cc1 digest `4fbf0c20391dc3e46d0840df795ebbc7e9b878d54858f6e0006f143501feb08c`).
Its separately supplied libatomic archive has digest
`ea5855f573bce90a1dad67d29929dd165e547f2300fc2b2e661851c913cdaed3`.
These are binary-pinned scalar tests; this does not qualify ICK complex runtime,
nor assert a reproducible clean-source build of the supplied compiler/runtime.
The first link attempt lacked libatomic_asneeded; explicitly supplying the
existing ICK runtime directory resolved it.

PASS: lexical rejection/normalization checks; all 343 angle states against the
immutable v0.2 reference; orthant layout; rotating face attachments; picking;
captured pointer/drag persistence; finite geometry and mesh checks.

Android uses NDK 27.2.12479018 (r27c), API 21, SDK platform 34 revision 3,
build-tools 35.0.0 and the canonical android-NDK packager
`7c61ee43e75f7c2dab9288edb0e10055898b36e6`.
Official archive SHA-1 values, verified against Google's repository metadata:

| Input | SHA-1 |
| --- | --- |
| android-ndk-r27c-linux.zip | 090e8083a715fdb1a3e402d0763c388abb03fb4e |
| platform-34-ext7_r03.zip | 1f2e9478d6a7601425ceaa553311dc43191f103d |
| build-tools_r35_linux.zip | 2cfaa0bbb2336e9ec18ed3ecea84fa2e2af607bc |

Package `org.isomorphisms.seifert`, versionCode 2, public test certificate
`de9b1d47c5a65e6d46a204b79dd9ee566b9d3c9832ba81ebc4213d3392e92ff9`.
The original package, version and signer are preserved. APK and packager receipt
digests are retained in `candidates/`; current producer results are separate.
The NDK native-app-glue emits two inherited unused-parameter warnings; all
first-party compilation retains `-Werror`.

Physical install, replacement update, launch, touch, lifecycle and GPU acceptance
are NOT_RUN. Historical A1 v1/v2 evidence is not renewed by these builds.
The Idriç readout child remains a separate branch and is not packaged in this APK.

## Producer result and remaining dependency

Both actual Flexible Pipes producer runs against merged policy
`01608a2493fa409463f70e8fbfd8a123ef59ee85` fail with
`UNREGISTERED_PACKAGE_LANE package=org.isomorphisms.seifert lane=test`.
The pipeline is pinned at `9775aa324f523cbc6c758e89461915484a5a0fc7`.
Full stage output and receipts are in `pipeline/`. Earlier attempts there used
the local-checkpoint package receipts; later attempts use the published source.
No failed producer has been converted into a PASS.

Dependency: [isomorphisms/ai-ci #236 — Register Seifert's stable public test Android signer](https://github.com/isomorphisms/ai-ci/pull/236).
The candidate registry passes all nine inherited self-tests, accepts Seifert's
test signer, and rejects a wrong digest and the unregistered release lane.
After that policy is merged, rerun the canonical producer on these final APKs:

| ABI | SHA-256 |
| --- | --- |
| armeabi-v7a | bbfcefae3e3ab230d555cfbc26a471efbceab58e6e8fb4624c5405a8d9dd76b7 |
| arm64-v8a | 2a929495f0843ca29c9603600e384b2c9c5a12db26d280c33f30dfb3553596e3 |

These remain review candidates, not accepted installation handoffs.
