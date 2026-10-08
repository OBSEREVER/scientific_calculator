#include "tokenizer.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static int add_token(token_list *out, token_type type, const char *text, double value)
{
    if (out->count >= MAX_TOKENS)
    {
        fprintf(stderr, "Error: too many tokens, expression too long\n");
        return 0;
    }
    token *t = &out->tokens[out->count];
    t->type = type;
    t->value = value;
    strncpy(t->text, text, MAX_TOKEN_LEN - 1);
    t->text[MAX_TOKEN_LEN - 1] = '\0';
    out->count++;
    return 1;
}

int tokenize(const char *expression, token_list *out)
{
    out->count = 0;
    int i = 0;

    while (expression[i] != '\0')
    {
        char ch = expression[i];

        if (isspace((unsigned char)ch))
        {
            i++;
            continue;
        }

        if (isdigit((unsigned char)ch) || ch == '.')
        {
            char buf[MAX_TOKEN_LEN];
            int len = 0;
            while ((isdigit((unsigned char)expression[i]) || expression[i] == '.') &&
                   len < MAX_TOKEN_LEN - 1)
            {
                buf[len++] = expression[i++];
            }
            buf[len] = '\0';

            if (len >= MAX_TOKEN_LEN - 1 && (isdigit((unsigned char)expression[i]) || expression[i] == '.'))
            {
                fprintf(stderr, "Error: number too long at position %d\n", i);
                return 0;
            }

            double val = atof(buf);
            if (!add_token(out, TOKEN_NUMBER, buf, val))
            {
                return 0;
            }
            continue;
        }

        if (isalpha((unsigned char)ch))
        {
            char buf[MAX_TOKEN_LEN];
            int len = 0;
            while (isalpha((unsigned char)expression[i]) && len < MAX_TOKEN_LEN - 1)
            {
                buf[len++] = expression[i++];
            }
            buf[len] = '\0';

            if (len >= MAX_TOKEN_LEN - 1 && isalpha((unsigned char)expression[i]))
            {
                fprintf(stderr, "Error: function name too long at position %d\n", i);
                return 0;
            }

            if (!add_token(out, TOKEN_FUNCTION, buf, 0.0))
            {
                return 0;
            }
            continue;
        }

        if (ch == '(')
        {
            if (!add_token(out, TOKEN_LPAREN, "(", 0.0))
            {
                return 0;
            }
            i++;
            continue;
        }

        if (ch == ')')
        {
            if (!add_token(out, TOKEN_RPAREN, ")", 0.0))
            {
                return 0;
            }
            i++;
            continue;
        }

        if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^' || ch == '%')
        {
            char buf[2] = { ch, '\0' };
            if (!add_token(out, TOKEN_OPERATOR, buf, 0.0))
            {
                return 0;
            }
            i++;
            continue;
        }

        fprintf(stderr, "Error: unrecognized character '%c' at position %d\n", ch, i);
        return 0;
    }

    return 1;
}