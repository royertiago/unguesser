#ifndef TEST_SAMPLE_DATABASE_H
#define TEST_SAMPLE_DATABASE_H

/* Sample database used in testing.
 *
 * sample_database_text is the contents of a file
 * formatted accroding to database_format.md.
 * sample_database is the corresponding database.
 */
#include "database.h"

constexpr char sample_database_text[] =
"3\n"
"Is this color bright?\n"
"This color appear in Brazil's flag?\n"
"Is this color associated with girls and women?\n"
"4\n"
"Red\n"
"3\n"
"1 -0.5\n"
"2 -1\n"
"3 1\n"
"Green\n"
"2\n"
"1 1\n"
"2 1\n"
"Blue\n"
"2\n"
"2 1\n"
"3 -1\n"
"Pink\n"
"3\n"
"1 1\n"
"2 -1\n"
"3 1\n"
;

extern const DataBase sample_database;

#endif // TEST_SAMPLE_DATABASE_H
