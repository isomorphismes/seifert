# Seifert ↔ arithmetic topology ↔ multiplication workloads

Cross-reading note, not an implementation or a new knot invariant.

The [existing book bibliography](README.md) uses Masanori Morishita's _Knots and Primes_ to relate link invariants and arithmetic of prime ideals. This file records a possible future computational bridge to the [ComputerScience number-theory shelf](https://github.com/walnut-burgundy/computer-science/tree/how-long%2Bhow-wide/books), the [Fulton algebraic-curves source](https://github.com/walnut-burgundy/fulton/tree/main/sources/fulton-algebraic-curves), and the [ICK large-integer benchmark](https://github.com/dilapidated-shed/ick/tree/main/benchmarks/bigmul).

## Quantities to keep distinct

1. **Link and ribbon geometry:** framing/twist or crossing choices; no bare integer may substitute for their definitions.
2. **Polynomial invariants:** Alexander polynomials and related algebraic expressions, with coefficient ring, normalization, and validity assumptions specified.
3. **Arithmetic topology:** linking numbers and power-residue symbols are related by *mathematical analogies* in the literature; they are not interchangeable values.
4. **Machine multiplication:** two unsigned integers, as arrays of words, have an exact integer product. Encoding polynomial data as integers is an extra step with requirements for sign, coefficient bounds, and radix size.

## Candidate benchmark paths (not yet generated)

- For a *certified* small Seifert matrix \(V\), calculate the Laurent polynomial represented by \(\det(V-tV^\mathsf T)\), checking normalization and invariance assumptions. Encode bounded integer coefficient arrays into nonoverlapping radix words; test the coefficient convolution against symbolic polynomial multiplication.
- For a documented knot or link whose Alexander polynomial has known coefficients, collect degree, support size, coefficient height, and exact signed coefficient data. These reveal how sparse or dense convolution operands differ.
- For arithmetic analogies in Morishita, select a specific theorem/example and derive concrete finite-ring operands, explicitly identifying modulo-reduction costs rather than relabeling them as plain large-integer multiplication.
- Keep provenance: source page or theorem, model input, exact generator, independent expected result, and a statement of what the kernel measures.

A useful **qualitative trace** can record nonzero coefficient interactions, carry propagation after coefficient packing, recursive split patterns, butterfly depth and address-permutation cycles. For a fair speed comparison, run all candidate integer multipliers on the **same encoded integer pairs**; polynomial algorithms should be separately classified.

See also [Seifert source papers](../papers/README.md).
