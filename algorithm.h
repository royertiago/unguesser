#ifndef ALGORITHM_H
#define ALGORITHM_H

/* His header contains all the basic algorithms of this program.
 */
#include <vector>
#include "entity.h"

/* Computes the similarity between the given entity
 * and the given partial answer vector.
 *
 * If the answer vector is empty, the similarity is zero.
 */
double similarity( const Entity &, const std::vector<Answer>& partial_answer_vector );

#endif // ALGORITHM_H
