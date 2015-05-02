#include "database.h"
#include <sstream>
#include <catch.hpp>

static const char sample_database[] =
"3\n"
"Is this color bright?\n"
"This color appear in Brazil's flag?\n"
"Is this color associated with girls and women?\n"
"4\n"
"Red\n"
"3\n"
"1 -0.5\n"
"2 -1\n"
"3 1\n"
"Green\n"
"2\n"
"1 1\n"
"2 1\n"
"Blue\n"
"2\n"
"2 1\n"
"3 -1\n"
"Pink\n"
"3\n"
"1 1\n"
"2 -1\n"
"3 1\n"
;

TEST_CASE( "DataBase::parse", "[DataBase][parse]" ) {
    std::stringstream stream( sample_database );
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
    DataBase db;
    db.questions = {
        {"Is this color bright?"},
        {"This color appear in Brazil's flag?"},
        {"Is this color associated with girls and women?"},
    };
    Question * base = &db.questions[0];
    db.entities = {
        {"Red", {
            {base,     -0.5},
            {base + 1, -1},
            {base + 2, 1},
        }},
        {"Green", {
            {base,     1},
            {base + 1, 1},
                  }},
        {"Blue", {
            {base + 1, 1},
            {base + 2, -1},
        }},
        {"Pink", {
            {base,     1},
            {base + 1, -1},
            {base + 2, 1},
        }},
    };
    db.write(stream);
    REQUIRE( stream.str() == sample_database );
}
