#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"

#define MAX_OUTPUT_TOKENS 512

typedef struct
{
    token tokens[MAX_OUTPUT_TOKENS];
    int count;
} postfix_list;

/* Converts infix token_list to postfix (RPN) using shunting-yard.
   Returns 1 on success, 0 on failure (mismatched parens, unknown function, etc.) */
int parse_to_postfix(const token_list *infix, postfix_list *out);

#endif