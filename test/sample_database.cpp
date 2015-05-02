#include "sample_database.h"

namespace {
    DataBase generate_database() {
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
        return db;
    };
} // anonymous namespace

const DataBase sample_database = generate_database();
