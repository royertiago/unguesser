#ifndef UNGUESSER_H
#define UNGUESSER_H

/* Front-end for the functionality of this repository.
 */
#include <iostream>
#include <memory>
#include <random>
#include "database.h"

class Unguesser {
    std::streampos file_position;
    DataBase db;
    std::vector<Answer> partial_answers;
    std::unique_ptr<std::iostream> file_ptr;

    long long unsigned _seed;
    std::mt19937 rng;
public:
    /* Constructs the database of this object
     * using the specified file.
     *
     * This file is also written at the program end.
     */
    Unguesser( std::unique_ptr<std::iostream> && file );

    /* Writes the database to the file given in the constructor
     * and closes it.
     */
    ~Unguesser();

    /* Gets/sets the seed used in the random number generator.
     */
    long long unsigned seed() const;
    void seed( long long unsigned );

    /* Obtains a new question to be answered by the user.
     * If there is no new questions, throw std::runtime_error.
     *
     * No question that was already answered (via Unguesser::add_answer)
     * shall be returned by this method.
     */
    const Question * next_question();

    /* Registers that the given question has the given answer.
     * The question shall have had been returned
     * by a previous call to Unguesser::next_question.
     */
    void add_answer( const Question *, double answer );

    /* Returns a sorted vector with all the entities of this class.
     * Note that we return a pointer vector;
     * each pointer points to the actual object inside the database.
     */
    std::vector<const Entity*> entities();
};

#endif // UNGUESSER_H
