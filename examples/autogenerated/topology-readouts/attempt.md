# Check the existing Seifert readouts

Purpose: qualify the previously uncompiled pure functions on the existing
readout branch, without adding a HUD, changing ribbons, or inventing invariants.

Type-system sketch: a scene contains signed unwrapped block angles; choosing
an endpoint diagnostic selects an outer block and subtracts the corner angle.
An available diagnostic carries a label and units. Geometric invariants carry
missing prerequisites instead of an invented numeric value. Conditional genus
and mixed-crossing linking arithmetic use exact Integer values and reject
inconsistent input. These functions do not certify a surface or diagram.

The original sketch aliases `Number` to `Double`. Current Idriç instead lowers
`Number` to the whole-number type. The repair names the inherited host-only
binary64 oracle `PrototypeScalar`; it does not widen a Float32 application
value or claim a Float16/Float32 backend. The native renderer's C float boundary
and target lowering remain separate and unqualified by this prototype.

Initial 8 October 2026 compiler source: isomorphisms/Idric `Idriç` at
`ff4d852862a3942592f8ade9afde8d409d9803be`. Source operations are kept in
`types/`; this directory holds acceptance calls, expected results, and failures.
Original attempts remain recoverable at the branch parent; no failed result is
to be represented as a successful compiler check.

Execution/result: PASS with the source-built current compiler. The ordinary
bootstrap tried inherited RefC support and failed on missing gmp.h. No GMP or
RefC fallback was installed. Explicit existing C/Chez support targets and
compiler/library targets are used instead. Chez is only the host test runtime,
not Android or a direct machine-code backend claim.

The first actual module check rejected `total` as a reserved keyword. Renaming
the crossing sum repaired that parse error. Module/file paths were reconciled;
undefined future geometry declarations are comments, not pretend implementations.
Both modules now typecheck. The executed ReadoutChecks program passes 17 groups,
including six signed endpoint fixtures, 2π/4π unwrapped history, exact integer
genus/linking cases, impossible/odd inputs, unavailable invariants, NaN,
infinity, and subtraction overflow. Invalid numbers produce Unavailable with
FiniteCommandedAngles. All checks exercise the actual module definitions.

Compiler image SHA-256:
`10002074cfae31a15e6f136cd191b6abe9f8d58efcec438d21145e72802b2819`.
Version output: `Idris 2, version 0.8.0-ff4d85286` (the current Idriç executable
retains the inherited product banner). The source checkout remained clean.
The checked-in Grease runner requires an explicit `IDRIC_COMPILER`; the host
Chez prefix/runtime must be provided. It does not install or select a fallback.

## Division glyph acceptance

The account-wide notation migration changes seven mathematical `/` tokens to
`÷` in the readout definitions and acceptance calls, including the compact
`tau÷4` form and the intentional `1÷0` and `0÷0` nonfinite fixtures.
Paths, comments and inherited inequality operators remain distinct.

The migrated modules typecheck and all 17 host check groups execute with
isomorphisms/Idric compiler source
`94dfd99bd3e376507fedc8611053b7173b2519f0`, whose maintained frontend maps `÷`
to the same checked division operation. The resulting compiler reports
`Idris 2, version 0.8.0-94dfd99bd`; its compiled Chez payload SHA-256 is
`4864ed84e76332e6bc295d158524fa113e8b6e078aa6b9f586bdd9e7e06210ab`.
This is host semantic evidence; Android lowering and physical acceptance keep
their existing separate requirements.
