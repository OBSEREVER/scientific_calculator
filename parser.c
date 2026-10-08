#include "parser.h"
#include "stack.h"
#include <string.h>
#include <stdio.h>

static int is_known_function(const char *name)
{
    return strcmp(name, "sin") == 0 || strcmp(name, "cos") == 0 ||
           strcmp(name, "tan") == 0 || strcmp(name, "log") == 0 ||
           strcmp(name, "ln") == 0 || strcmp(name, "sqrt") == 0;
}

static int precedence(char op)
{
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/' || op == '%')
        return 2;
    if (op == '^')
        return 3;
    return 0;
}

static int is_right_assoc(char op)
{
    return op == '^';
}

static int push_output(postfix_list *out, token t)
{
    if (out->count >= MAX_OUTPUT_TOKENS)
    {
        fprintf(stderr, "Error: expression too long for parser output\n");
        return 0;
    }
    out->tokens[out->count++] = t;
    return 1;
}

int parse_to_postfix(const token_list *infix, postfix_list *out)
{
    op_stack ops;
    op_init(&ops);
    out->count = 0;

    /* We store operator characters on op_stack; functions and parens
       need tracking too, so we keep a parallel small marker scheme:
       '(' and ')' go on op_stack as themselves,
       function tokens are pushed onto op_stack as 'f' marker is not enough,
       so instead we keep a separate array of pending function tokens. */

    token func_pending[STACK_CAP];
    int func_top = -1;

    for (int i = 0; i < infix->count; i++)
    {
        token t = infix->tokens[i];

        if (t.type == TOKEN_NUMBER)
        {
            if (!push_output(out, t))
            {
                return 0;
            }
        }
        else if (t.type == TOKEN_FUNCTION)
        {
            if (!is_known_function(t.text))
            {
                fprintf(stderr, "Error: unknown function '%s'\n", t.text);
                return 0;
            }
            if (func_top >= STACK_CAP - 1)
            {
                fprintf(stderr, "Error: function stack overflow\n");
                return 0;
            }
            func_pending[++func_top] = t;
            if (!op_push(&ops, 'f'))
            {
                return 0;
            }
        }
        else if (t.type == TOKEN_LPAREN)
        {
            if (!op_push(&ops, '('))
            {
                return 0;
            }
        }
        else if (t.type == TOKEN_RPAREN)
        {
            char top;
            int found_lparen = 0;
            while (op_pop(&ops, &top))
            {
                if (top == '(')
                {
                    found_lparen = 1;
                    break;
                }
                if (top == 'f')
                {
                    if (func_top < 0)
                    {
                        fprintf(stderr, "Error: function stack underflow\n");
                        return 0;
                    }
                    if (!push_output(out, func_pending[func_top--]))
                    {
                        return 0;
                    }
                }
                else
                {
                    token op_tok;
                    op_tok.type = TOKEN_OPERATOR;
                    op_tok.text[0] = top;
                    op_tok.text[1] = '\0';
                    op_tok.value = 0.0;
                    if (!push_output(out, op_tok))
                    {
                        return 0;
                    }
                }
            }
            if (!found_lparen)
            {
                fprintf(stderr, "Error: mismatched parentheses (missing '(')\n");
                return 0;
            }
        }
        else if (t.type == TOKEN_OPERATOR)
        {
            char cur = t.text[0];
            char top;
            while (op_peek(&ops, &top) && top != '(')
            {
                if (top == 'f')
                {
                    break;
                }
                if ((precedence(top) > precedence(cur)) ||
                    (precedence(top) == precedence(cur) && !is_right_assoc(cur)))
                {
                    op_pop(&ops, &top);
                    token op_tok;
                    op_tok.type = TOKEN_OPERATOR;
                    op_tok.text[0] = top;
                    op_tok.text[1] = '\0';
                    op_tok.value = 0.0;
                    if (!push_output(out, op_tok))
                    {
                        return 0;
                    }
                }
                else
                {
                    break;
                }
            }
            if (!op_push(&ops, cur))
            {
                return 0;
            }
        }
        else
        {
            fprintf(stderr, "Error: unknown token type in parser\n");
            return 0;
        }
    }

    char top;
    while (op_pop(&ops, &top))
    {
        if (top == '(')
        {
            fprintf(stderr, "Error: mismatched parentheses (missing ')')\n");
            return 0;
        }
        if (top == 'f')
        {
            if (func_top < 0)
            {
                fprintf(stderr, "Error: function stack underflow\n");
                return 0;
            }
            if (!push_output(out, func_pending[func_top--]))
            {
                return 0;
            }
        }
        else
        {
            token op_tok;
            op_tok.type = TOKEN_OPERATOR;
            op_tok.text[0] = top;
            op_tok.text[1] = '\0';
            op_tok.value = 0.0;
            if (!push_output(out, op_tok))
            {
                return 0;
            }
        }
    }

    return 1;
}