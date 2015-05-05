#include <stdexcept>
#include <iostream>
#include "database.h"

DataBase::DataBase( const DataBase & other ) :
    questions(other.questions),
    entities(other.entities)
{
    /* We need to reroute the pointers in the answers to inside this->questions.
     * For each answer a, a.question is a pointer inside &other.questions[0]
     * (the C++11 standard mandates std::vector to be contiguous - 23.3.6.1/1)
     * so a.question - &other.questions[0] gives the proper index.
     * So, we take the address of questions[index] to have the pointer
     * to same question, but inside our 'questions' vector.
     *
     * Note that this arithmetic is only valid because the memory pointed by
     * &other.questions[0] is still valid.
     */
    for( auto & e : entities )
        for( auto & a : e.answers )
            a.question = &questions[a.question - &other.questions[0]];
}

DataBase::DataBase( DataBase&& other ) :
    questions(other.questions),
    entities(other.entities)
{
    /* Move of std::vectors is just a change of pointers,
     * so the Answer::question pointers are not invalidated.
     */
}

DataBase & DataBase::operator=( const DataBase & other ) {
    questions = other.questions;
    entities = other.entities;
    for( auto & e : entities )
        for( auto & a : e.answers )
            a.question = &questions[a.question - &other.questions[0]];
    return *this;
}

DataBase & DataBase::operator=( DataBase&& other ) {
    questions = std::move( other.questions );
    entities = std::move( other.entities );
    return *this;
}

void DataBase::push_back( Question && q ) {
    if( questions.size() < questions.capacity() ) {
        questions.push_back( std::move(q) );
        return;
    }

    auto new_questions = questions;
    new_questions.push_back( std::move(q) );

    for( auto & e : entities )
        for( auto & a : e.answers )
            a.question = &new_questions[a.question - &questions[0]];

    questions.swap(new_questions);
}

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
