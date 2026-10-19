# Books: arithmetic topology and supporting topology

This is a **bibliographic reading list**, not a repository of copyrighted book files.
Books are kept separate from [historical papers](../papers/README.md).

## Arithmetic topology

### Masanori Morishita — *Knots and Primes: An Introduction to Arithmetic Topology*

- **Preferred edition:** 2nd ed., Springer, Universitext, 2024.
  [Publisher / DOI](https://doi.org/10.1007/978-981-99-9255-3).
- **Earlier edition:** 1st ed., Springer, Universitext, 2012.
  [Publisher / DOI](https://doi.org/10.1007/978-1-4471-2158-9).
- **Subject:** systematic comparison of knot and link topology in
  3-manifolds with arithmetic of primes and number fields. The second
  edition adds material on idelic class field theory and
  Dijkgraaf–Witten theory.

Reading map for eventual mathematical definitions:

| Topological object or invariant | Arithmetic counterpart covered by Morishita |
| --- | --- |
| Knot / link in a 3-manifold | Prime ideals in a number ring |
| Linking numbers | Legendre and higher power residue symbols |
| Link groups | Galois groups with restricted ramification |
| Milnor invariants | Multiple power residue symbols |
| Alexander modules | Iwasawa modules |
| 3-manifold homology | Ideal class groups |

These are **analogies and correspondences**, not assertions that the
objects in each row are interchangeable or numerically equal.

## Supporting geometric and knot-theoretic background

### John Milnor — *Singular Points of Complex Hypersurfaces*

- **Edition:** Princeton University Press, *Annals of Mathematics Studies*
  61, 1968.
  [Publisher-linked bibliographic record](https://www.jstor.org/stable/j.ctt1bd6kvv).
- **Subject:** topology near complex hypersurface singularities, their
  links, Milnor fibrations, and the topology of the fibers.
- **Role here:** a foundation for understanding knots, links, and
  fibered surfaces arising from singularities. **Not** itself a
  textbook on arithmetic topology.

## Relation to Seifert's interactive ribbons

Keep three kinds of quantity distinct when definitions are added:

1. **Geometry:** an unwrapped twist angle can vary continuously under
   dragging.
2. **Framing/ribbon data:** twist and self-linking of a *framed* knot
   can depend on how the ribbon is attached.
3. **Ambient-isotopy invariants:** ordinary knot type, Alexander
   polynomial, and related invariants do **not** change under a mere
   deformation that preserves the knot type.

A changing on-screen integer must therefore name its mathematical
input and hypotheses; a user turning a block is not, by itself,
evidence that an unframed knot invariant has changed.

For source papers on the Alexander polynomial, Seifert surfaces,
cyclic coverings, and knotted flows, see [papers](../papers/README.md).


## Additional books for implementation

These works fill out the geometric and algebraic prerequisites:

- **Dale Rolfsen, *Knots and Links* (1976; AMS Chelsea 2003 reprint)**:
  knot groups, Seifert surfaces, link invariants and diagrams.
- **Colin C. Adams, *The Knot Book* (AMS, 2004)**: diagrams, crossings,
  Reidemeister moves, and Seifert's algorithm.
- **Herbert Seifert and William Threlfall, *A Textbook of Topology*
  (1934 German original; 1980 English translation)**: general topology
  and fibered 3-dimensional spaces.
- **Allen Hatcher, *Algebraic Topology* (Cambridge, 2002)**:
  fundamental groups, covering spaces and homology. An author-hosted
  free electronic edition is [available under its stated terms](https://pi.math.cornell.edu/~hatcher/AT/ATpage.html).

Machine-readable entries: [books.bib](books.bib). Source-paper entries:
[papers.bib](../papers/papers.bib).

## Proposed live mathematical quantities and their validity

| Function | Required inputs | Current display validity |
| --- | --- | --- |
| relativeEndpointTurns | Unwrapped block angles: outer minus corner, divided by 2π | **Valid now as relative commanded rotations**; not geometric ribbon twist. |
| geometricRibbonTwist | Regular centerline and continuous transverse unit framing | Not yet computable. |
| writhe | Regular centerline and validated geometric integral | Not yet computable, and not invariant under ambient isotopy. |
| linkingNumber | Two disjoint, oriented **closed** curves | Not valid for the present open bands. |
| surfaceGenusFromCertifiedCounts | Connected oriented surface with certified Euler characteristic and number of boundary components | Conditional. Not automatically the *minimal knot genus*. |
| alexanderPolynomial | Certified closed link and Seifert matrix or equivalent algebra | Not yet computable. |
| LegendreSymbol | Explicit arithmetic inputs including an odd prime | Not a number supplied by dragging a ribbon. |

A full endpoint revolution can change the visible ribbon while returning
the cube's rendered orientation. The unwrapped angle history must remain
available to future Idriç functions. A missing invariant must be reported
as *unavailable*, never as a spurious zero.

## Cross-reference: Seifert-fibered manifolds

**Unbuilt feature:** [Seifert-fibered 3-manifold exploration #8](https://github.com/isomorphismes/seifert/issues/8). This is separate from the [current Seifert ribbon work #6](https://github.com/isomorphismes/seifert/issues/6) and cross-links [Montesinos orbifold examples](https://github.com/isomorphisms/montesinos/issues/1).

The *Seifert surface* of a knot (the subject of the interactive ribbons) should not be confused with a *Seifert-fibered 3-manifold*. For the latter, see **Peter Scott**, [“The Geometries of 3-Manifolds” (1983)](https://doi.org/10.1112/blms/15.5.401), §§3–5: the theory of Seifert fiber spaces and their place among Thurston's eight geometries. This is a **survey paper**, catalogued in [papers](../papers/README.md), not an extra book.

## Cross-reading for arithmetic workloads

[Arithmetic topology, polynomial invariants, and multiplication benchmarks](arithmetic-examples-for-multiplication.md) gives possible future operand families and backlinks to the ComputerScience and Fulton books shelves. It is a research map, not a benchmark result.
