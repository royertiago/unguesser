#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include "answer.h"

/* An entity that the user will think of and the program will try to guess.
 */
struct Entity {
    /* Full display name of this entity.
     */
    std::string name;

    /* Answers that players had given to this entity.
     * Note that this list almost surely is incomplete;
     * that is, not all questions in the database have answers here.
     *
     * Invariant: the Answer::question pointers are in increasing order.
     */
    std::vector<Answer> answers;
};

#endif // ENTITY_H
