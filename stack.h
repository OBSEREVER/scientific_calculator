#ifndef STACK_H
#define STACK_H

#define STACK_SIZE 100

typedef struct
{
    int operand[STACK_SIZE];
    int top;
} operand_stack;

typedef struct
{
    char operator[STACK_SIZE];
    int top;
} operator_stack;

/* For operand stack */
void operand_stack_init(operand_stack *s);
void operand_stack_push(operand_stack *s, int value);
int operand_stack_pop(operand_stack *s);
int operand_stack_top(operand_stack *s);
int operand_stack_empty(operand_stack *s);
int operand_stack_full(operand_stack *s);

/* For operator stack */
void operator_stack_init(operator_stack *s);
void operator_stack_push(operator_stack *s, char value);
char operator_stack_pop(operator_stack *s);
char operator_stack_top(operator_stack *s);
int operator_stack_empty(operator_stack *s);
int operator_stack_full(operator_stack *s);

#endif