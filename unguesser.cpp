#include <algorithm>
#include <cfloat>
#include <chrono>
#include "algorithm.h"
#include "unguesser.h"

Unguesser::Unguesser( std::unique_ptr<std::iostream> && file ) {
    file_ptr = std::move(file);
    file_position = file_ptr->tellg();
    db = DataBase::parse(*file_ptr);

    _seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng.seed(_seed);

    // partial_answers starts empty.
}

Unguesser::~Unguesser() {
    file_ptr->seekg(file_position);
    db.write(*file_ptr);
}

long long unsigned Unguesser::seed() const {
    return _seed;
}

void Unguesser::seed( long long unsigned new_seed ) {
    rng.seed(new_seed);
    _seed = new_seed;
}

const Question * Unguesser::next_question() {
    compute_similarity( db, partial_answers );
    double min = -DBL_MAX, max = DBL_MAX;
    for( const auto & e : db.entities ) {
        min = std::min(min, e.similarity);
        max = std::max(max, e.similarity);
    }
    double threshold = (min + max)/2;
    compute_bisection_factor( db, threshold );

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

std::vector<const Entity*> Unguesser::entities() {
    compute_similarity( db, partial_answers );
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
