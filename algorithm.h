#ifndef ALGORITHM_H
#define ALGORITHM_H

/* His header contains all the basic algorithms of this program.
 */
#include <vector>
#include "database.h"
#include "entity.h"

/* Computes the similarity between the given entity
 * and the given partial answer vector.
 *
 * If the answer vector is empty, the similarity is zero.
 */
double similarity( const Entity &, const std::vector<Answer>& partial_answer_vector );

/* Assign the attribute Entity::similarity of each entity in the database
 * with the similarity computed by the function ::similarity.
 */
void compute_similarity( DataBase &, const std::vector<Answer>& );

#endif // ALGORITHM_H
