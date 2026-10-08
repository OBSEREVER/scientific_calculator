#ifndef TOKENIZER_H
#define TOKENIZER_H

#define MAX_TOKENS 512
#define MAX_TOKEN_LEN 32

typedef enum
{
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_FUNCTION,
    TOKEN_UNKNOWN
} token_type;

typedef struct
{
    token_type type;
    char text[MAX_TOKEN_LEN];
    double value;
} token;

typedef struct
{
    token tokens[MAX_TOKENS];
    int count;
} token_list;

/* returns 1 on success, 0 on failure (bad input or too many tokens) */
int tokenize(const char *expression, token_list *out);

#endif