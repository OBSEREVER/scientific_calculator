#include "stack.h"
#include <stdio.h>

/* ---------- Number stack ---------- */

void num_init(num_stack *s)
{
    s->top = -1;
}

int num_push(num_stack *s, double val)
{
    if (num_is_full(s))
    {
        fprintf(stderr, "Error: number stack overflow (expression too long)\n");
        return 0;
    }
    s->data[++(s->top)] = val;
    return 1;
}

int num_pop(num_stack *s, double *out)
{
    if (num_is_empty(s))
    {
        fprintf(stderr, "Error: number stack underflow (missing operand)\n");
        return 0;
    }
    *out = s->data[(s->top)--];
    return 1;
}

int num_peek(const num_stack *s, double *out)
{
    if (num_is_empty(s))
    {
        fprintf(stderr, "Error: number stack is empty, cannot peek\n");
        return 0;
    }
    *out = s->data[s->top];
    return 1;
}

int num_is_empty(const num_stack *s)
{
    return s->top == -1;
}

int num_is_full(const num_stack *s)
{
    return s->top == STACK_CAP - 1;
}

/* ---------- Operator stack ---------- */

void op_init(op_stack *s)
{
    s->top = -1;
}

int op_push(op_stack *s, char val)
{
    if (op_is_full(s))
    {
        fprintf(stderr, "Error: operator stack overflow (expression too long)\n");
        return 0;
    }
    s->data[++(s->top)] = val;
    return 1;
}

int op_pop(op_stack *s, char *out)
{
    if (op_is_empty(s))
    {
        fprintf(stderr, "Error: operator stack underflow (missing operator)\n");
        return 0;
    }
    *out = s->data[(s->top)--];
    return 1;
}

int op_peek(const op_stack *s, char *out)
{
    if (op_is_empty(s))
    {
        fprintf(stderr, "Error: operator stack is empty, cannot peek\n");
        return 0;
    }
    *out = s->data[s->top];
    return 1;
}

int op_is_empty(const op_stack *s)
{
    return s->top == -1;
}

int op_is_full(const op_stack *s)
{
    return s->top == STACK_CAP - 1;
}