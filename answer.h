#ifndef ANSWER_H
#define ANSWER_H

struct Question;

/* One answer that some entity gives to some question.
 * Note that the answer does not know in which entity it is contained.
 */
struct Answer {
    /* Question that this Answer answers.
     */
    Question * question;

    /* A value between -1 and 1 indicating
     * how positive is the answer to this question.
     * 1 is a strong "Yes",
     * -1 is a strong "No".
     */
    double answer;
};

#endif // ANSWER_H
