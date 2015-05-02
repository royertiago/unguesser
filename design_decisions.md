Design Decisions
================

This text documents the design decisions that are
nontrivial, potentially controversial, questionable,
and/or have deep impact in the program structure.

Algorithmic attributes
----------------------

An important design decision is to put some of the algorithm's data
directly inside the top-level structures.
For instance, a `Question` does not have only the attribute `text`,
but also the `positive_factor` and `negative_factor` attributes.

From the object-orientation perspective, the algorithm-only attributes
should not be pasted together with the object's real/effective attributes
--- these attributes should be added only in the execution of the algorithm,
using, for instance, the decorator pattern ---
but doing so simplifies the algorithm implementation.

Still, we make a clear distincion between effective attributes
and algorithmic attributes; every struct that have algorithmic attributes
will have a comment `// Algorithmic attributes`, that "defines" a section
--- every algorithmic attribute and only algorithmic attributes
appear after this commentary.

Structs instead of classes
--------------------------

The emphasis of this program is on the algorithm, not in great software engineering
nor object-orientation principles.
Thus, most of the data is held in structs rather than classes.

Class invariants
----------------

Any class or struct may define "Invariants"; informal descriptions
of conditions that must be held true for the lifetime of that class' objects.
(These descriptions must remain informal because of
C++'s lack of native support to invariants.)
No function will state explicitly that the invariant is assumed or will be mantained.
