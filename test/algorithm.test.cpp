#include "algorithm.h"
#include "sample_database.h"
#include <catch.hpp>
#include <iostream>

TEST_CASE( "Similarity algorithm", "[similarity][algorithm]" ) {
    // Safe because we will not modify anything this test.
    Question * base = const_cast<Question *>(&sample_database.questions[0]);

    std::vector<Answer> partial_answers;

    SECTION( "Similarity with no answers" ) {
        CHECK( similarity(sample_database.entities[0], partial_answers) == 0 );
        CHECK( similarity(sample_database.entities[1], partial_answers) == 0 );
        CHECK( similarity(sample_database.entities[2], partial_answers) == 0 );
        CHECK( similarity(sample_database.entities[3], partial_answers) == 0 );
    }

    SECTION( "Similarity with one answer" ) {
        partial_answers = {{base, -1}};
        CHECK( similarity(sample_database.entities[0], partial_answers) == 0.5 );
        CHECK( similarity(sample_database.entities[1], partial_answers) == -1 );
        CHECK( similarity(sample_database.entities[2], partial_answers) == 0 );
        CHECK( similarity(sample_database.entities[3], partial_answers) == -1 );

        partial_answers = {{base + 1, 0.5}};
        CHECK( similarity(sample_database.entities[0], partial_answers) == -0.5 );
        CHECK( similarity(sample_database.entities[1], partial_answers) == 0.5 );
        CHECK( similarity(sample_database.entities[2], partial_answers) == 0.5 );
        CHECK( similarity(sample_database.entities[3], partial_answers) == -0.5 );

        partial_answers = {{base + 2, -0.5}};
        CHECK( similarity(sample_database.entities[0], partial_answers) == -0.5 );
        CHECK( similarity(sample_database.entities[1], partial_answers) == 0 );
        CHECK( similarity(sample_database.entities[2], partial_answers) == 0.5 );
        CHECK( similarity(sample_database.entities[3], partial_answers) == -0.5 );
    }

    SECTION( "Similarity with two answers" ) {
        partial_answers = {{base, -1}, {base + 1, 0.5}};
        CHECK( similarity(sample_database.entities[0], partial_answers) == 0 );
        CHECK( similarity(sample_database.entities[1], partial_answers) == -0.5 );
        CHECK( similarity(sample_database.entities[2], partial_answers) == 0.5 );
        CHECK( similarity(sample_database.entities[3], partial_answers) == -1.5 );

        partial_answers = {{base, -1}, {base + 2, -0.5}};
        CHECK( similarity(sample_database.entities[0], partial_answers) == 0 );
        CHECK( similarity(sample_database.entities[1], partial_answers) == -1 );
        CHECK( similarity(sample_database.entities[2], partial_answers) == 0.5 );
        CHECK( similarity(sample_database.entities[3], partial_answers) == -1.5 );

        partial_answers = {{base + 1, 0.5}, {base + 2, -0.5}};
        CHECK( similarity(sample_database.entities[0], partial_answers) == -1 );
        CHECK( similarity(sample_database.entities[1], partial_answers) == 0.5 );
        CHECK( similarity(sample_database.entities[2], partial_answers) == 1 );
        CHECK( similarity(sample_database.entities[3], partial_answers) == -1 );
    }

    SECTION( "Similarity with two answers and one null answer" ) {
        partial_answers = {{base, -1}, {base + 1, 0.5}, {base + 2, 0}};
        CHECK( similarity(sample_database.entities[0], partial_answers) == 0 );
        CHECK( similarity(sample_database.entities[1], partial_answers) == -0.5 );
        CHECK( similarity(sample_database.entities[2], partial_answers) == 0.5 );
        CHECK( similarity(sample_database.entities[3], partial_answers) == -1.5 );

        partial_answers = {{base, -1}, {base + 1, 0}, {base + 2, -0.5}};
        CHECK( similarity(sample_database.entities[0], partial_answers) == 0 );
        CHECK( similarity(sample_database.entities[1], partial_answers) == -1 );
        CHECK( similarity(sample_database.entities[2], partial_answers) == 0.5 );
        CHECK( similarity(sample_database.entities[3], partial_answers) == -1.5 );

        partial_answers = {{base, 0}, {base + 1, 0.5}, {base + 2, -0.5}};
        CHECK( similarity(sample_database.entities[0], partial_answers) == -1 );
        CHECK( similarity(sample_database.entities[1], partial_answers) == 0.5 );
        CHECK( similarity(sample_database.entities[2], partial_answers) == 1 );
        CHECK( similarity(sample_database.entities[3], partial_answers) == -1 );
    }
}

TEST_CASE( "compute_similarity function", "[similarity][algorithm]" ) {
    DataBase db = sample_database;
    Question * base = &db.questions[0];
    std::vector<Answer> ans = {{base, 0}, {base + 1, 0.5}, {base + 2, -0.5}};
    compute_similarity( db, ans );
    CHECK( db.entities[0].similarity == -1 );
    CHECK( db.entities[1].similarity == 0.5 );
    CHECK( db.entities[2].similarity == 1 );
    CHECK( db.entities[3].similarity == -1 );
}
