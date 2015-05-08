#ifndef UNGUESSER_H
#define UNGUESSER_H

/* Front-end for the functionality of this repository.
 */
#include <iostream>
#include <memory>
#include <random>
#include "database.h"

/* Possible moves for the Unguesser.
 * These values are returned by Unguesser::next_move().
 */
enum class UnguesserMove {
    /* The Unguesser wants to ask the user a new question,
     * through the method Unguesser::next_question().
     */
    ASK_QUESTION,

    /* The Unguesser thinks it has found the correct answer,
     * avaliable in Unguesser::guess().
     *
     * If the Unguesser guessed right, Unguesser::inform_answer
     * should be called with the return of Unguesser::guess().
     * If the guess was wrong, the user might let the Unguesser
     * ask some more questions - just call Unguesser::next_question
     * - or follow the same procedure as in GIVE_UP.
     *
     * Unguesser::next_move() will likely return ASK_QUESTION
     * when called again.
     */
    GUESS_ANSWER,

    /* The Unguesser have made lots of questions and still has no idea
     * of which entity the user tought.
     * It want to be informed about the answer,
     * throughout Unguesser::inform_answer().
     *
     * Note that the answer may already be in the database.
     * You should call Unguesser::match_name before inserting a new value.
     *
     * Do not assume the next_move() will be RESTART;
     * the Unguesser may want the user to add a new question to the database.
     */
    GIVE_UP,

    /* The Unguesser had made lots of questions
     * and has only a few possible options,
     * but cannot separate between these options,
     * and wants the user to formulate a question to separate them.
     * The guess vector is avaliable in Unguesser::best_guesses().
     * The new question should be informed via Unguesser::inform_new_question.
     *
     * This is a qualified GIVE_UP.
     *
     * If the user does not want to provide a new question,
     * simply call Unguesser::restart.
     */
    ASK_NEW_QUESTION,

    /* The game ended and should be restarted,
     * via Unguesser::restart.
     */
    RESTART_GAME,
};

class Unguesser {
    DataBase db;
    std::vector<Answer> partial_answers;
    std::unique_ptr<std::iostream> file_ptr;

    long long unsigned _seed;
    std::mt19937 rng;

    /* Current state of the Unguesser.
     */
    UnguesserMove move_state = UnguesserMove::ASK_QUESTION;

    /* How much an entity must be close to the top
     * to be treated as a possible answer to the question.
     * "Close to the top" means higher similarity value.
     *
     * The closer to 0 is percentile_tolerance, the smallest is the vector
     * best_guesses.
     */
    double percentile_tolerance = 0.5;

    /* True when the answer given by the user
     * constitutes a new entity in the database.
     */
    bool answer_is_new_entity = false;

    /* Entities with similarity higher than the threshold
     * are returned by best_guesses(). This value is computed in next_question.
     */
    double threshold;

    /* If the attribute 'clean' is true, return immediately; otherwise,
     * runs 'compute_similarity' and 'compute_bisection_factor' on the database,
     * updates the threshold value, and sets 'clean' to true.
     */
    void recompute_statistics();

    /* Flag used to control when recompute_statistics
     * will actually do its work.
     */
    bool clean = false;

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

    /* Do not write to the file given in the input.
     */
    void disable_write();

    /* Gets/sets the seed used in the random number generator.
     */
    long long unsigned seed() const;
    void seed( long long unsigned );

    /* Gets the next move.
     *
     * Unguesser::next_move is designed to be called from within a
     * switch/case statement; so, it should have full control
     * over the game flow.
     * The only exceptions are documented in each enum value in UnguesserMove.
     */
    UnguesserMove next_move();

    /* Restarts the game.
     * No new information is recorded in the database.
     */
    void restart();

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

    /* Returns the best guess the Unguesser have to the player.
     * This may be called at any moment, altough the result
     * should be more precise when next_move() returns GUESS_ANSWER.
     */
    const Entity * guess();

    /* Returns the best guesses the Unguesser have to the player.
     * This may be called at any moment, altough the result
     * should be more precise when next_move() returns ASK_NEW_QUESTION.
     */
    std::vector< const Entity * > best_guesses();

    /* Returns a sorted vector with all the entities of this class.
     * Note that we return a pointer vector;
     * each pointer points to the actual object inside the database.
     */
    std::vector< const Entity * > entities();

    /* Set of methods that allows the user to inform the correct answer
     * to the Unguesser.
     * Best used only when the Unguesser gives up.
     *
     * match_name asks for a name, or a partial name,
     * and will return every Entity in the database whose name matched,
     * either fully or partially.
     * Note it might return a null vector.
     *
     * The first inform_answer receives a pointer from the database,
     * usually one of the pointers returned via match_name.
     * The database is then updated with the current answers.
     * This question should also be called with the return of Unguesser::guess(),
     * to thell the game it got that right.
     *
     * The second inform_answer introduces a new value in the database.
     */
    std::vector< const Entity * > match_name( std::string name );
    void inform_answer( const Entity * );
    void inform_answer( std::string name );

    /* Add a new question in the database.
     * question_text is the question text, including the question mark.
     * answers is a vector with pairs (Entity, answer_for_that_entity),
     * that the user gave.
     * Usually, the entities in this vector are from Unguesser::best_guesses,
     * after next_move returned ASK_NEW_QUESTION.
     */
    void inform_new_question(
        std::string question_text,
        std::vector< std::pair<const Entity *, double> > answers
    );

// Internal Unguesser information

    /* Return the threshold that determines which entities will be used
     * to determine the next question.
     */
    double similarity_threshold() const;

    /* Return how many questions the Unguesser may guess.
     */
    std::size_t remaining_questions() const;
};

#endif // UNGUESSER_H
