#include "stack.h"

/* For operand stack */
void operand_stack_init(operand_stack *s)
{
    s->top = -1;
}
void operand_stack_push(operand_stack *s, int value)
{
    if (!operand_stack_full(s))
    {
        s->operand[++s->top] = value;
    }
}
int operand_stack_pop(operand_stack *s)
{
    if (!operand_stack_empty(s))
    {
        int value = s->operand[s->top];
        s->top--;
        return value;
    }
    return 0;
}
int operand_stack_top(operand_stack *s)
{
    return s->operand[s->top];
}
int operand_stack_empty(operand_stack *s)
{
    return s->top == -1;
}
int operand_stack_full(operand_stack *s)
{
    return s->top == STACK_SIZE - 1;
}

/* For operator stack */
void operator_stack_init(operator_stack *s)
{
    s->top = -1;
}
void operator_stack_push(operator_stack *s, char value)
{
    if (!operator_stack_full(s))
    {
        s->operator[++s->top] = value;
    }
}
char operator_stack_pop(operator_stack *s)
{
    if (!operator_stack_empty(s))
    {
        char value = s->operator[s->top];
        s->top--;
        return value;
    }
    return '\0';
}
char operator_stack_top(operator_stack *s)
{
    return s->operator[s->top];
}
int operator_stack_empty(operator_stack *s)
{
    return s->top == -1;
}
int operator_stack_full(operator_stack *s)
{
    return s->top == STACK_SIZE - 1;
}