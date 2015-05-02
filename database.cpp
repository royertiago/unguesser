#include <iostream>
#include "database.h"

DataBase DataBase::parse( std::istream & is ) {
    DataBase database;
    int Q;
    is >> Q;
    is.ignore(); // trailing newline
    while( Q-- ) {
        Question q;
        std::getline( is, q.text );
        database.questions.push_back( q );
    }
    int E;
    is >> E;
    is.ignore(); // trailing newline
    while( E-- ) {
        Entity e;
        std::getline( is, e.name );
        int A;
        is >> A;
        while( A-- ) {
            Answer a;
            int question;
            is >> question >> a.answer;
            a.question = &database.questions[question - 1];
            e.answers.push_back( a );
        }
        is.ignore(); // trailing newline
        database.entities.push_back(e);
    }
    return database;
}

void DataBase::write( std::ostream & os ) const {
    const Question * base = &questions[0];
    os << questions.size() << '\n';
    for( const Question & q : questions )
        os << q.text << '\n';
    os << entities.size() << '\n';
    for( const Entity & e : entities ) {
        os << e.name << '\n';
        os << e.answers.size() << '\n';
        for( const Answer & a : e.answers )
            os << a.question - base + 1 << ' ' << a.answer << '\n';
    }
}
