#include <algorithm>
#include <cfloat>
#include <chrono>
#include "algorithm.h"
#include "unguesser.h"

Unguesser::Unguesser( std::unique_ptr<std::iostream> && file ) {
    file_ptr = std::move(file);
    auto file_position = file_ptr->tellg();
    db = DataBase::parse(*file_ptr);
    file_ptr->seekg(file_position);

    _seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng.seed(_seed);

    // partial_answers starts empty.
}

Unguesser::~Unguesser() {
    if( file_ptr )
        db.write(*file_ptr);
}

void Unguesser::recompute_statistics() {
    if( clean )
        return;

    compute_similarity( db, partial_answers );
    double min = DBL_MAX, max = -DBL_MAX;
    for( const auto & e : db.entities ) {
        min = std::min(min, e.similarity);
        max = std::max(max, e.similarity);
    }
    threshold = ( min*percentile_tolerance + max*(1 - percentile_tolerance) );

    compute_bisection_factor( db, threshold );

    clean = true;
}

void Unguesser::disable_write() {
    file_ptr = nullptr;
}

long long unsigned Unguesser::seed() const {
    return _seed;
}

void Unguesser::seed( long long unsigned new_seed ) {
    rng.seed(new_seed);
    _seed = new_seed;
}

UnguesserMove Unguesser::next_move() {
    switch( move_state ) {
        case UnguesserMove::ASK_QUESTION:
            /* If our last move was a question and we narrowed the guess space
             * to a single guess, let's make this guess.
             *
             * Alternatively, if there is no more questions to be asked,
             * we will first try to guess something
             * before asking the user for help.
             */
            if( best_guesses().size() == 1 ||
                remaining_questions() == 0 )
                return move_state = UnguesserMove::GUESS_ANSWER;

            return move_state = UnguesserMove::ASK_QUESTION;

        case UnguesserMove::GUESS_ANSWER:
            /* inform_answer unconditionally resets the move_state
             * to UnguesserMove::RESTART_GAME.
             * If we reach here, it means our guess was wrong,
             * but the user still wants we to continue guessing.
             */

            if( remaining_questions() == 0 )
                return move_state = UnguesserMove::GIVE_UP;

            // We still have other questions to make.
            percentile_tolerance = std::min( 3 * percentile_tolerance, 0.5 );
            return move_state = UnguesserMove::ASK_QUESTION;

        case UnguesserMove::GIVE_UP:
            if( answer_is_new_entity &&
                guess()->similarity > 0.75 * partial_answers.size()
            )
                return move_state = UnguesserMove::ASK_NEW_QUESTION;
            if( !answer_is_new_entity && db.questions.size() > partial_answers.size() )
                return move_state = UnguesserMove::ASK_NEW_QUESTION;

            // fall-through
        case UnguesserMove::ASK_NEW_QUESTION:
            return move_state = UnguesserMove::ASK_NEW_QUESTION;

        case UnguesserMove::RESTART_GAME:
            return move_state = UnguesserMove::RESTART_GAME;
    }
}

void Unguesser::restart() {
    partial_answers.clear();
    move_state = UnguesserMove::ASK_QUESTION;
    percentile_tolerance = 0.5;
    answer_is_new_entity = false;
    clean = false;
}

const Question * Unguesser::next_question() {
    clean = false;
    recompute_statistics();
    percentile_tolerance *= (3.0/4);

    auto vec = rank_questions( db );
    if( vec.empty() )
        throw std::runtime_error("No more questions in the database.");

    double shift = vec.back().second * 1.125;
    std::vector<double> chances;
    for( auto & pair : vec )
        chances.push_back( pair.second - shift );

    std::discrete_distribution<int> dist(chances.begin(), chances.end());
    return vec[dist(rng)].first;
}

void Unguesser::add_answer( const Question * question, double answer ) {
    Question * q = const_cast<Question *>(question);
    partial_answers.push_back({q, answer});
    /* This cast is safe because both 'question' points to a modifiable element
     * inside db.questions and because we will not modify that question
     * through the partial_answers vector.
     */
    std::sort( partial_answers.begin(), partial_answers.end(),
        []( auto a1, auto a2 ) {
            return a1.question < a2.question;
        }
    );
}

const Entity * Unguesser::guess() {
    recompute_statistics();

    double best = 0;
    const Entity * ret;
    for( auto & e : db.entities )
        if( e.similarity > best ) {
            best = e.similarity;
            ret = &e;
        }
    return ret;
}

std::vector< const Entity * > Unguesser::best_guesses() {
    recompute_statistics();

    std::vector< const Entity * > ret_val;
    for( auto & e : db.entities )
        if( e.similarity > threshold )
            ret_val.push_back( &e );

    return ret_val;
}

std::vector< const Entity * > Unguesser::entities() {
    recompute_statistics();

    std::vector<const Entity*> ret_val;
    for( const auto& e : db.entities )
        ret_val.push_back( &e );

    std::sort(ret_val.begin(), ret_val.end(),
        []( const Entity * p1, const Entity * p2 ) {
            return p1->similarity > p2->similarity;
        }
    );
    return ret_val;
}

std::vector< const Entity * > Unguesser::match_name( std::string str ) {
    std::vector<const Entity*> ret_val;
    for( auto & e : db.entities )
        if( e.name.find(str) != std::string::npos )
            ret_val.push_back( &e );
    return ret_val;
}

void Unguesser::inform_answer( const Entity * entity ) {
    clean = false;

    // Safe because we have ownership over e.
    Entity & e = const_cast<Entity &>( *entity );

    auto it = e.answers.begin();
    auto jt = partial_answers.begin();
    while( it != e.answers.end() && jt != partial_answers.end() ) {
        if( it->question < jt->question ) {
            /* e.answers have a question partial_answers does not.
             * Just increment 'it', since we cannot extract more data from here.
             */
            ++it;
        }
        else if( jt->question < it->question ) {
            /* partial_answers have a question e.answers does not.
             * We must insert that question in e.answers.
             */
            it = e.answers.insert(it, *jt);
            ++jt;
        }
        else {
            // A question both vectors have.
            it->answer = (3 * it->answer + jt->answer)/4.0;
            ++it;
            ++jt;
        }
    }
    while( jt != partial_answers.end() )
        e.answers.push_back( *jt++ );

    /* If this method (or its overloaded version) is called,
     * it means that either we got the right answer,
     * or the user is providing us with the right answer.
     * In both cases, it makes no sense to keep asking questions to the user.
     * But it might be useful to ask the user to inform a new question,
     * if several entities are too close to each other.
     */
    if( best_guesses().size() > 1 )
        move_state = UnguesserMove::ASK_NEW_QUESTION;
    else
        move_state = UnguesserMove::RESTART_GAME;
}

void Unguesser::inform_answer( std::string name ) {
    clean = false;

    double similarity = 0;
    for( auto & ans : partial_answers )
        similarity += ans.answer * ans.answer;

    db.entities.push_back({name, partial_answers, similarity});

    if( best_guesses().size() > 1 )
        move_state = UnguesserMove::ASK_NEW_QUESTION;
    else
        move_state = UnguesserMove::RESTART_GAME;
}

void Unguesser::inform_new_question(
    std::string question_text,
    std::vector< std::pair<const Entity *, double> > answers
) {
    clean = false;

    db.push_back({question_text});
    Question * ptr = &db.questions.back();
    for( auto pair: answers ) {
        // Safe because we have ownership over pair.first.
        Entity & e = const_cast<Entity &>(*pair.first);
        e.answers.push_back({ ptr, pair.second });
    }

    /* After ASK_NEW_QUESTION, the only transition is to RESTART_GAME.
     */
    move_state = UnguesserMove::RESTART_GAME;
}

double Unguesser::similarity_threshold() const {
    return threshold;
}

std::size_t Unguesser::remaining_questions() const {
    return db.questions.size() - partial_answers.size();
}
