Algorithm used by the unguesser
===============================

Overview
--------

The algorithm used is based on basic linear algebra.
The program computes the inner product between the partial answer vector
and every entry in the database.
(The inner product tries to capture the notion of orthogonality.)
The higher the inner product, the more similar is the partial answer vector
to that specific entry, so we will choose new questions
trying to either validate that entry or separate it from other similar entries.
