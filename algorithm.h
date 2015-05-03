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
 *
 * Also set Question::used accordingly to each question in the answer vector.
 */
void compute_similarity( DataBase &, const std::vector<Answer>& );

/* Assign the attributes Question::positive_factor and Question::negative_factor
 * with its correct values; but uses only the entities whose similarity
 * is equal to or greater than the threshold.
 *
 * The threshold argument helps choosing the question for the "next" bisection.
 */
void compute_bisection_factor( DataBase &, double threshold );

/* Returns a list of pairs (question, score), ordered by score.
 * Any used question (a question with Question::used set) is ignored.
 * The score is calculated in the following way:
 *  First, let X be the maximum possible sum of positive_factor and negative_factor
 *  in the entire database.
 *  Then, treat each question as a pair in the xy-plane, with coordinates
 *  (positive_factor, negative_factor), and compute the distance to the point (X/2, X/2)
 *  using simple euclidean distance.
 *  Finally, multiply the distance by -1. This is the score of that question.
 */
std::vector<std::pair<Question *, double>> rank_questions( DataBase & );

#endif // ALGORITHM_H
