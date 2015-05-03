#include <algorithm>
#include "algorithm.h"

double similarity( const Entity & e, const std::vector<Answer> & ans ) {
    double sim = 0;
    auto it = e.answers.begin();
    auto jt = ans.begin();
    while( it != e.answers.end() && jt != ans.end() ) {
        if( it->question < jt->question )
            ++it;
        else if( it->question > jt->question )
            ++jt;
        else {
            sim += it->answer * jt->answer;
            ++it;
            ++jt;
        }
    }
    return sim;
}

void compute_similarity( DataBase & db, const std::vector<Answer> & ans ) {
    for( auto & e : db.entities )
        e.similarity = similarity(e, ans);
    for( auto & q : db.questions )
        q.used = false;
    Question * base = &db.questions[0];
    for( auto & a : ans )
        db.questions[a.question - base].used = true;
}

void compute_bisection_factor( DataBase & db, double threshold ) {
    for( auto & q : db.questions )
        q.positive_factor = q.negative_factor = 0.0;

    for( auto & e : db.entities )
        if( e.similarity >= threshold )
            for( auto & a : e.answers ) {
                if( a.answer > 0 )
                    a.question->positive_factor += a.answer;
                else
                    a.question->negative_factor -= a.answer;
            }
}

std::vector<std::pair<Question *, double>> rank_questions( DataBase & db ) {
    double X = 0;
    for( auto & q : db.questions )
        if( !q.used )
            X = std::max(X, q.positive_factor + q.negative_factor);

    std::vector<std::pair<Question *, double>> vec;
    for( auto & q : db.questions )
        if( !q.used )
            vec.push_back({&q,
                -std::hypot(X/2-q.positive_factor, X/2-q.negative_factor)
            });

    std::sort( vec.begin(), vec.end(), []( const auto & p1, const auto & p2 ){
        return p1.second > p2.second;
    });

    return vec;
}
