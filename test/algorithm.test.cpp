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
    SECTION( "Single valid answer" ) {
        std::vector<Answer> ans = {{base, 0.0}};
        compute_similarity( db, ans );
        CHECK( db.entities[0].similarity == 0 );
        CHECK( db.entities[1].similarity == 0 );
        CHECK( db.entities[2].similarity == 0 );
        CHECK( db.entities[3].similarity == 0 );
        CHECK( db.questions[0].used == true );
        CHECK( db.questions[1].used == false );
        CHECK( db.questions[2].used == false );
    }
    SECTION( "Two valid answers" ) {
        std::vector<Answer> ans = {{base + 1, 0.5}, {base + 2, -0.5}};
        compute_similarity( db, ans );
        CHECK( db.entities[0].similarity == -1 );
        CHECK( db.entities[1].similarity == 0.5 );
        CHECK( db.entities[2].similarity == 1 );
        CHECK( db.entities[3].similarity == -1 );
        CHECK( db.questions[0].used == false );
        CHECK( db.questions[1].used == true );
        CHECK( db.questions[2].used == true );
    }
    SECTION( "Three valid answers" ) {
        std::vector<Answer> ans = {{base, 0}, {base + 1, 0.5}, {base + 2, -0.5}};
        compute_similarity( db, ans );
        CHECK( db.entities[0].similarity == -1 );
        CHECK( db.entities[1].similarity == 0.5 );
        CHECK( db.entities[2].similarity == 1 );
        CHECK( db.entities[3].similarity == -1 );
        CHECK( db.questions[0].used == true );
        CHECK( db.questions[1].used == true );
        CHECK( db.questions[2].used == true );
    }
}

TEST_CASE( "compute_bisection_factor function", "[algorithm]" ) {
    DataBase db = sample_database;
    Question * base = &db.questions[0];
    std::vector<Answer> ans = {{base, 0}, {base + 1, 0.5}, {base + 2, -0.5}};
    compute_similarity( db, ans );

    SECTION( "highest possible threshold" ) {
        compute_bisection_factor( db, 1.5 );
        CHECK( db.questions[0].positive_factor == 0 );
        CHECK( db.questions[0].negative_factor == 0 );
        CHECK( db.questions[1].positive_factor == 0 );
        CHECK( db.questions[1].negative_factor == 0 );
        CHECK( db.questions[2].positive_factor == 0 );
        CHECK( db.questions[2].negative_factor == 0 );
    }
    SECTION( "lowest possible threshold" ) {
        compute_bisection_factor( db, -1 );
        CHECK( db.questions[0].positive_factor == 2 );
        CHECK( db.questions[0].negative_factor == 0.5 );
        CHECK( db.questions[1].positive_factor == 2 );
        CHECK( db.questions[1].negative_factor == 2 );
        CHECK( db.questions[2].positive_factor == 2 );
        CHECK( db.questions[2].negative_factor == 1 );
    }
    SECTION( "intermediate threshold" ) {
        compute_bisection_factor( db, 0 );
        CHECK( db.questions[0].positive_factor == 1 );
        CHECK( db.questions[0].negative_factor == 0 );
        CHECK( db.questions[1].positive_factor == 2 );
        CHECK( db.questions[1].negative_factor == 0 );
        CHECK( db.questions[2].positive_factor == 0 );
        CHECK( db.questions[2].negative_factor == 1 );
    }
}

TEST_CASE( "rank_questions function", "[algorithm]" ) {
    DataBase db = sample_database;
    Question * base = &db.questions[0];
    std::vector<Answer> ans = {{base, 0}, {base + 1, 0.5}, {base + 2, -0.5}};

    compute_similarity( db, ans );
    /* The default behavior for compute_similarity is to also
     * write Question::used. We will override it for this test only.
     */
    for( auto & q : db.questions )
        q.used = false;

    compute_bisection_factor( db, -1.0 );
    auto vec = rank_questions( db );
    REQUIRE( vec.size() == 3 );
    CHECK( vec[0].first == base + 1 );
    CHECK( vec[0].second == Approx(0) );
    CHECK( vec[1].first == base + 2 );
    CHECK( vec[1].second == Approx(-1) );
    CHECK( vec[2].first == base );
    CHECK( vec[2].second == Approx(-1.5) );
}
