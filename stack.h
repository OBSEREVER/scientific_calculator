#ifndef STACK_H
#define STACK_H

#define STACK_SIZE 100

typedef struct
{
    double n_data[STACK_SIZE];
    int n_top;
} NumStack;

typedef struct
{
    char o_data[STACK_SIZE];
    int o_top;
} OpStack;

/* Number stack */
void   num_init(NumStack *nums);
void   num_push(NumStack *nums, double val);
double num_pop(NumStack *nums);
double num_peek(const NumStack *nums);
int    num_is_empty(const NumStack *nums);
int    num_is_full(const NumStack *nums);

/* Operator stack */
void op_init(OpStack *ops);
void op_push(OpStack *ops, char val);
char op_pop(OpStack *ops);
char op_peek(const OpStack *ops);
int  op_is_empty(const OpStack *ops);
int  op_is_full(const OpStack *ops);

#endif