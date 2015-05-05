#include "database.h"
#include "sample_database.h"
#include <sstream>
#include <catch.hpp>

#include <iostream>
TEST_CASE( "DataBase::parse", "[DataBase][parse]" ) {
    std::stringstream stream( sample_database_text );
    DataBase db = DataBase::parse(stream);

    REQUIRE( db.questions.size() == 3 );
    CHECK( db.questions[0].text == "Is this color bright?" );
    CHECK( db.questions[1].text == "This color appear in Brazil's flag?" );
    CHECK( db.questions[2].text == "Is this color associated with girls and women?" );
    REQUIRE( db.entities.size() == 4 );

    Question * base = &db.questions[0];

    Entity e = db.entities[0];
    CHECK( e.name == "Red" );
    REQUIRE( e.answers.size() == 3 );
    CHECK( e.answers[0].question == base );
    CHECK( e.answers[0].answer == -0.5 );
    CHECK( e.answers[1].question == base + 1 );
    CHECK( e.answers[1].answer == -1 );
    CHECK( e.answers[2].question == base + 2 );
    CHECK( e.answers[2].answer == 1 );

    e = db.entities[1];
    CHECK( e.name == "Green" );
    REQUIRE( e.answers.size() == 2 );
    CHECK( e.answers[0].question == base );
    CHECK( e.answers[0].answer == 1 );
    CHECK( e.answers[1].question == base + 1 );
    CHECK( e.answers[1].answer == 1 );

    e = db.entities[2];
    CHECK( e.name == "Blue" );
    REQUIRE( e.answers.size() == 2 );
    CHECK( e.answers[0].question == base + 1 );
    CHECK( e.answers[0].answer == 1 );
    CHECK( e.answers[1].question == base + 2 );
    CHECK( e.answers[1].answer == -1 );

    e = db.entities[3];
    CHECK( e.name == "Pink" );
    REQUIRE( e.answers.size() == 3 );
    CHECK( e.answers[0].question == base );
    CHECK( e.answers[0].answer == 1 );
    CHECK( e.answers[1].question == base + 1 );
    CHECK( e.answers[1].answer == -1 );
    CHECK( e.answers[2].question == base + 2 );
    CHECK( e.answers[2].answer == 1 );
}

TEST_CASE( "DataBase::write", "[DataBase][write]" ) {
    std::stringstream stream;
    sample_database.write(stream);
    REQUIRE( stream.str() == sample_database_text );
}

TEST_CASE( "DataBase::push_back", "[DataBase]" ) {
    DataBase db = sample_database;

    for( int i = db.questions.capacity() - db.questions.size() + 1; i >= 0; i-- )
        db.push_back( {"Dummy question"} );

    Question * base = &db.questions[0];

    CHECK( db.questions[0].text == "Is this color bright?" );
    CHECK( db.questions[1].text == "This color appear in Brazil's flag?" );
    CHECK( db.questions[2].text == "Is this color associated with girls and women?" );

    Entity e = db.entities[0];
    CHECK( e.answers[0].question == base );
    CHECK( e.answers[1].question == base + 1 );
    CHECK( e.answers[2].question == base + 2 );

    e = db.entities[1];
    CHECK( e.answers[0].question == base );
    CHECK( e.answers[1].question == base + 1 );

    e = db.entities[2];
    CHECK( e.answers[0].question == base + 1 );
    CHECK( e.answers[1].question == base + 2 );

    e = db.entities[3];
    CHECK( e.answers[0].question == base );
    CHECK( e.answers[1].question == base + 1 );
    CHECK( e.answers[2].question == base + 2 );
}
