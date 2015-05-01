#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include "answer.h"

/* A database entry.
 */
struct Entry {
    /* Full display name of this entry.
     */
    std::string name;

    /* Answers that players had given to this entry.
     * Note that this list almost surely is incomplete;
     * that is, not all questions in the database have answers here.
     */
    std::vector<Answer> answers;
};

#endif // ENTITY_H
