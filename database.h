#ifndef DATABASE_H
#define DATABASE_H

#include <iosfwd>
#include <vector>
#include "question.h"
#include "entity.h"

/* Structure that groups entities, questions and answers.
 *
 * Class invariant: Every Answer::question pointer
 * points to some question inside DataBase::questions.
 */
struct DataBase {
    std::vector<Question> questions;
    std::vector<Entity> entities;

    /* Parse a database from the given input stream.
     * The database format is described in database_format.md.
     */
    static DataBase parse( std::istream & );

    /* Writes the database in the given output stream.
     * The database format is described in database_format.md.
     */
    void write( std::ostream & ) const;
};

#endif // DATABASE_H
