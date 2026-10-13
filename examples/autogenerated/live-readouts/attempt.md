# Live endpoint-rotation display

The user wants the existing twist gestures to update visible mathematical
quantities. The scene owns three signed, unwrapped commanded angles. The two
displayed diagnostics are the upper-minus-corner and right-minus-corner
rotations, measured in turns. Neither is the geometric twist integral.

Inputs: a validated three-block scene with explicit binary32 angles.
Output: two labelled available/unavailable readouts in turns. Arithmetic is
pure; formatting and GLES drawing are separate presentation effects. Invalid
angles and overflowing subtraction cannot become numeric readouts. Open bands
do not certify linking number, writhe, genus, or geometric twist.

The existing Idriç program is retained in
`../topology-readouts/ReadoutChecks.idric`, with definitions in
`types/ArithmeticTopology.idric`. Its binary64 host prototype is an independent
semantic specification, not an Android binary32 ABI implementation.

Current attempt: source checkout `94dfd99bd3e376507fedc8611053b7173b2519f0`
was located. Invoking its retained `_/build/exec/idris2 --help` fails before
compilation with exit 127 because `idris2_app/idris2.so` is absent. This does
not establish a source-type error or qualify any Android Idriç backend.

The visible-result implementation therefore extends the application's existing
compositional ICK C owner and native C ABI. This is an explicit binary32
consumer of the documented readout convention, **not compiled Idriç**, RefC,
generated C, or a new geometry solver. Tests independently check the documented
integer-turn fixtures, common-angle invariance, unwrapped history, invalid
input, formatting, and real rendered pixel changes.

The remaining Idriç language boundary is direct, qualified Android lowering of
the readout operations with an explicit Float32 ABI and available/unavailable
transport. Acceptance for that future replacement must execute the actual
generated module on ARMv7/AArch64 and compare its outputs at this ABI; success
of this native consumer does not satisfy that boundary.
