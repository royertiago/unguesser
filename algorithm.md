Algorithm used by the unguesser
===============================

Overview
--------

The algorithm used is based on basic linear algebra.
The program computes the inner product between the partial answer vector
and every entity in the database.
(The inner product tries to capture the notion of orthogonality.)
The higher the inner product, the more similar is the partial answer vector
to that specific entity, so we will choose new questions
trying to either validate that entity or separate it from other similar entries.
