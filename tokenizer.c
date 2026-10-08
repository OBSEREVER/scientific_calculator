#include "tokenizer.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Scalable table of mathematical and physical constants */
typedef struct
{
    const char *name;
    double value;
} math_constant;

static const math_constant CONSTANTS[] = {
    {"pi", 3.14159265358979323846},
    {"e",  2.71828182845904523536},
    {"c",  299792458.0},             /* Speed of light (m/s) */
    {"g",  9.80665},                 /* Standard gravity (m/s^2) */
    {"h",  6.62607015e-34},          /* Planck's constant (J*s) */
    {"k",  1.380649e-23},           /* Boltzmann constant (J/K) */
    {"na", 6.02214076e23},           /* Avogadro's number (1/mol) */
    {"qe", 1.602176634e-19}          /* Elementary charge (C) */
};

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

        /* Detect unary sign for numbers (e.g., -5, +3.14) */
        int is_unary_sign = (ch == '-' || ch == '+') &&
                            (out->count == 0 ||
                             out->tokens[out->count - 1].type == TOKEN_OPERATOR ||
                             out->tokens[out->count - 1].type == TOKEN_LPAREN) &&
                            (isdigit((unsigned char)expression[i + 1]) || expression[i + 1] == '.');

        if (isdigit((unsigned char)ch) || ch == '.' || is_unary_sign)
        {
            char buf[MAX_TOKEN_LEN];
            int len = 0;

            if (is_unary_sign)
            {
                buf[len++] = expression[i++];
            }

            /* Read base digits and decimal point */
            while ((isdigit((unsigned char)expression[i]) || expression[i] == '.') &&
                   len < MAX_TOKEN_LEN - 1)
            {
                buf[len++] = expression[i++];
            }

            /* Read scientific notation exponent (e.g., 1.5e-3, 6.022e23) */
            if ((expression[i] == 'e' || expression[i] == 'E') && len < MAX_TOKEN_LEN - 1)
            {
                int next = i + 1;
                if (expression[next] == '+' || expression[next] == '-')
                {
                    next++;
                }
                /* Only absorb if 'e' is followed by an actual exponent number */
                if (isdigit((unsigned char)expression[next]))
                {
                    buf[len++] = expression[i++];
                    if (expression[i] == '+' || expression[i] == '-')
                    {
                        buf[len++] = expression[i++];
                    }
                    while (isdigit((unsigned char)expression[i]) && len < MAX_TOKEN_LEN - 1)
                    {
                        buf[len++] = expression[i++];
                    }
                }
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
                fprintf(stderr, "Error: identifier too long at position %d\n", i);
                return 0;
            }

            /* Table-driven constants check */
            int found_constant = 0;
            size_t num_constants = sizeof(CONSTANTS) / sizeof(CONSTANTS[0]);
            for (size_t c = 0; c < num_constants; c++)
            {
                if (strcmp(buf, CONSTANTS[c].name) == 0)
                {
                    if (!add_token(out, TOKEN_NUMBER, buf, CONSTANTS[c].value))
                    {
                        return 0;
                    }
                    found_constant = 1;
                    break;
                }
            }

            if (found_constant)
            {
                continue;
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