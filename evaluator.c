#include "evaluator.h"
#include "stack.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

static int apply_operator(num_stack *s, char op)
{
    double a, b;
    if (!num_pop(s, &b))
    {
        return 0;
    }
    if (!num_pop(s, &a))
    {
        fprintf(stderr, "Error: missing operand for operator '%c'\n", op);
        return 0;
    }

    double res;
    switch (op)
    {
        case '+':
            res = a + b;
            break;
        case '-':
            res = a - b;
            break;
        case '*':
            res = a * b;
            break;
        case '/':
            if (b == 0.0)
            {
                fprintf(stderr, "Error: division by zero\n");
                return 0;
            }
            res = a / b;
            break;
        case '%':
            if (b == 0.0)
            {
                fprintf(stderr, "Error: modulo by zero\n");
                return 0;
            }
            res = fmod(a, b);
            break;
        case '^':
            res = pow(a, b);
            break;
        default:
            fprintf(stderr, "Error: unknown operator '%c'\n", op);
            return 0;
    }

    if (isnan(res) || isinf(res))
    {
        fprintf(stderr, "Error: result out of range for operator '%c'\n", op);
        return 0;
    }

    return num_push(s, res);
}

static int apply_function(num_stack *s, const char *name)
{
    double a;
    if (!num_pop(s, &a))
    {
        fprintf(stderr, "Error: missing operand for function '%s'\n", name);
        return 0;
    }

    double res;
    if (strcmp(name, "sin") == 0)
    {
        res = sin(a);
    }
    else if (strcmp(name, "cos") == 0)
    {
        res = cos(a);
    }
    else if (strcmp(name, "tan") == 0)
    {
        res = tan(a);
    }
    else if (strcmp(name, "sqrt") == 0)
    {
        if (a < 0.0)
        {
            fprintf(stderr, "Error: square root of negative number\n");
            return 0;
        }
        res = sqrt(a);
    }
    else if (strcmp(name, "log") == 0)
    {
        if (a <= 0.0)
        {
            fprintf(stderr, "Error: log of non-positive number\n");
            return 0;
        }
        res = log10(a);
    }
    else if (strcmp(name, "ln") == 0)
    {
        if (a <= 0.0)
        {
            fprintf(stderr, "Error: ln of non-positive number\n");
            return 0;
        }
        res = log(a);
    }
    else
    {
        fprintf(stderr, "Error: unknown function '%s'\n", name);
        return 0;
    }

    if (isnan(res) || isinf(res))
    {
        fprintf(stderr, "Error: result out of range for function '%s'\n", name);
        return 0;
    }

    return num_push(s, res);
}

int evaluate_postfix(const postfix_list *postfix, double *result)
{
    num_stack nums;
    num_init(&nums);

    for (int i = 0; i < postfix->count; i++)
    {
        token t = postfix->tokens[i];

        if (t.type == TOKEN_NUMBER)
        {
            if (!num_push(&nums, t.value))
            {
                return 0;
            }
        }
        else if (t.type == TOKEN_OPERATOR)
        {
            if (!apply_operator(&nums, t.text[0]))
            {
                return 0;
            }
        }
        else if (t.type == TOKEN_FUNCTION)
        {
            if (!apply_function(&nums, t.text))
            {
                return 0;
            }
        }
        else
        {
            fprintf(stderr, "Error: unexpected token in postfix evaluation\n");
            return 0;
        }
    }

    double final;
    if (!num_pop(&nums, &final))
    {
        fprintf(stderr, "Error: empty expression, nothing to evaluate\n");
        return 0;
    }

    if (!num_is_empty(&nums))
    {
        fprintf(stderr, "Error: malformed expression, leftover operands\n");
        return 0;
    }

    *result = final;
    return 1;
}