# Seifert instructions

Apply the shared [ai-ci instructions](https://github.com/isomorphisms/ai-ci/blob/main/AGENTS.md).
Before Idriç source changes, read the canonical
[Idriç style guide](https://github.com/dilapidated-shed/Idric/blob/Idri%C3%A7/STYLE.md)
and its intent examples. Preserve [the compositional ICK source contract](notes/icky-style.md).

`icky/seifert.c` owns geometry. `types/ArithmeticTopology.idric` defines the
host semantic readouts; `icky/seifert_readouts.c` implements their explicit
binary32 native consumer. It is not compiled Idriç. The renderer only draws
the labelled results. Do not infer geometric twist, writhe, linking or genus
from commanded endpoint rotations or an open-ribbon picture.

Run the existing geometry/reference comparisons and the Grease live-readout
checks with the pinned ICK compiler. Host GLES, signed packaging, and physical
handset interaction are separate acceptance boundaries.
