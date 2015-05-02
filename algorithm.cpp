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
