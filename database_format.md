Database Format
===============

The database used by the program follows an ad-hoc format designed to ease
manual edition and simplify the task of writing a parser by hand.
The format is "line-oriented"; that is, each piece of information
has its own line.

The database is divided in two sections.
The first section contains the questions,
and the second contains the database entities,
together with their respective answers.

The question section contains the number of questions
(lets call Q)
followed by Q lines, each line with a question.

The entity section contains the number of entries
(lets call E)
followed by E entries.
Each entity contains its name (in a single line),
followed by the number of answers
(lets call A),
followed by A pairs `question_index answer`.
The `question_index` is the index of the question in the database.
The `answer` is the Answer::answer field.


Database example
================

    3
    Is this color bright?
    This color appear in Brazil's flag?
    Is this color associated with girls and women?
    4
    Red
    3
    1 -0.5
    2 -1
    3 1
    Green
    2
    1 1
    2 1
    Blue
    2
    2 1
    3 -1
    Pink
    3
    1 1
    2 -1
    3 1

An extended database can be found in [`ine.db`](ine.db).
