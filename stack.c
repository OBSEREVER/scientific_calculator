#include "stack.h"

/* Number stack */
void num_init(NumStack *nums)
{
    nums->n_top = -1;
}

void num_push(NumStack *nums, double val)
{
    if (!num_is_full(nums))
    {
        nums->n_data[++nums->n_top] = val;
    }
}

double num_pop(NumStack *nums)
{
    if (!num_is_empty(nums))
    {
        return nums->n_data[nums->n_top--];
    }
    return 0.0;
}

double num_peek(const NumStack *nums)
{
    if (!num_is_empty(nums))
    {
        return nums->n_data[nums->n_top];
    }
    return 0.0;
}

int num_is_empty(const NumStack *nums)
{
    return nums->n_top == -1;
}

int num_is_full(const NumStack *nums)
{
    return nums->n_top == STACK_SIZE - 1;
}

/* Operator stack */
void op_init(OpStack *ops)
{
    ops->o_top = -1;
}

void op_push(OpStack *ops, char val)
{
    if (!op_is_full(ops))
    {
        ops->o_data[++ops->o_top] = val;
    }
}

char op_pop(OpStack *ops)
{
    if (!op_is_empty(ops))
    {
        return ops->o_data[ops->o_top--];
    }
    return '\0';
}

char op_peek(const OpStack *ops)
{
    if (!op_is_empty(ops))
    {
        return ops->o_data[ops->o_top];
    }
    return '\0';
}

int op_is_empty(const OpStack *ops)
{
    return ops->o_top == -1;
}

int op_is_full(const OpStack *ops)
{
    return ops->o_top == STACK_SIZE - 1;
}