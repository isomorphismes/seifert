# Papers: knot theory, Seifert surfaces, and arithmetic links

Historical papers and adjacent research sources. These are **papers**,
not books; the book catalog is [books/README.md](../books/README.md).
Links point to journal, publisher, or author records rather than
checked-in copies.

## Knot invariants and surfaces

1. **J. W. Alexander**, “Topological Invariants of Knots and Links,”
   *Transactions of the American Mathematical Society* **30** (1928),
   no. 2, 275–306.
   [Journal / DOI](https://doi.org/10.1090/S0002-9947-1928-1501429-1).
   Foundational source for the Alexander polynomial.

2. **Herbert Seifert**, “Über das Geschlecht von Knoten,”
   *Mathematische Annalen* **110** (1935), 571–592.
   [Journal / DOI](https://doi.org/10.1007/BF01448044).
   Foundational source for Seifert surfaces and knot genus.

3. **Herbert Seifert**, “Die Verschlingungsinvarianten der zyklischen
   Knotenüberlagerungen,” *Abhandlungen aus dem Mathematischen Seminar
   der Universität Hamburg* **11** (1935), 84–101.
   [Journal archive, vol. 11](https://abhandlungen.math.uni-hamburg.de/archiv.php?vol=11).
   Linking invariants associated with cyclic knot coverings.

## Dynamics and arithmetic connections

4. **Étienne Ghys**, “Knots and Dynamics,” in *Proceedings of the
   International Congress of Mathematicians, Madrid 2006*, vol. I
   (published 2007), 247–277.
   [Publisher / DOI](https://doi.org/10.4171/022-1/11).
   Knotted periodic orbits, linking, and the arithmetic of modular
   geodesic flow. An adjacent research survey, **not** a standalone
   arithmetic-topology book.

## Reading order for the interactive Seifert project

- Alexander and Seifert: distinguish polynomial/link invariants from
  the geometry and framing of a visibly twisted ribbon.
- Ghys: look for dynamics-driven links between knots and arithmetic.
- Morishita (in the [books catalog](../books/README.md)): the systematic
  arithmetic-topology correspondences, including linking/residue
  symbols and Alexander/Iwasawa modules.

No paper here is claimed to be implemented in Idriç yet.


## Further references for measured twist and linking

5. **John Milnor**, “Link Groups,” *Annals of Mathematics* **59**
   (1954), no. 2, 177–195.
   [DOI](https://doi.org/10.2307/1969685).
   Link-homotopy and link groups, relevant to later Milnor invariants.

6. **M. R. Dennis and J. H. Hannay**, “Geometry of Călugăreanu's
   theorem,” *Proceedings of the Royal Society A* **461** (2005).
   [Author version (arXiv)](https://arxiv.org/abs/math-ph/0503012).
   In the closed-framed-ribbon setting, the relationship
   **linking number = geometric twist + writhe**. The present two
   open ribbons do not meet the closure assumptions.

Bibliography file: [papers.bib](papers.bib).

## Seifert fiber spaces and Thurston geometries

7. **Peter Scott**, “The Geometries of 3-Manifolds,” *Bulletin of the London Mathematical Society* **15** (1983), no. 5, 401–487. [DOI](https://doi.org/10.1112/blms/15.5.401). Especially §§3–5: Seifert fiber spaces, all eight Thurston model geometries, and their classification.

The distinction matters in this repository: a **Seifert surface** is an oriented surface bounding a link, whereas a **Seifert-fibered 3-manifold** is a 3-manifold foliated by circles with a specified local fiber structure. They share a historical name, not a definition. Scott is a reference for the second meaning; the ribbon-twist viewer does not yet model it.

The associated bibliographic entry is `scott1983` in [papers.bib](papers.bib). Cross-listed from the [books reading shelf](../books/README.md) as a survey paper, not a book.
