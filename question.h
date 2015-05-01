#ifndef QUESTION_H
#define QUESTION_H

#include <string>

struct Question {
    /* Full text of the question, reqdy to be displayed.
     * Includes the question mark '?'.
     */
    std::string text;

// Algorithmic attributes
    /* Classification potential (binary search factor)
     * These values try to codify how well this question bissects the database.
     * positive_factor is the sum of all positive answers for this question.
     * negative_factor is the sum of all negative answers for this question.
     * The higher is the value of min(positive_factor, negative_factor),
     * the greater is the worst possible bissection,
     * and the greater is the chance that this question will be chosen next.
     *
     * These attributes change each time the program
     * needs to make a new question to the user.
     */
    double positive_factor;
    double negative_factor;
};

#endif // QUESTION_H
