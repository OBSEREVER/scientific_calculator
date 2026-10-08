#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "parser.h"

/* Evaluates a postfix token list. Returns 1 on success, 0 on failure
   (division by zero, invalid function input, malformed expression). */
int evaluate_postfix(const postfix_list *postfix, double *result);

#endif