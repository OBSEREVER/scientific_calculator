#ifndef STACK_H
#define STACK_H

#define STACK_CAP 512

typedef struct
{
    double data[STACK_CAP];
    int top;
} num_stack;

typedef struct
{
    char data[STACK_CAP];
    int top;
} op_stack;

/* Number stack */
void num_init(num_stack *s);
int  num_push(num_stack *s, double val);
int  num_pop(num_stack *s, double *out);
int  num_peek(const num_stack *s, double *out);
int  num_is_empty(const num_stack *s);
int  num_is_full(const num_stack *s);

/* Operator stack */
void op_init(op_stack *s);
int  op_push(op_stack *s, char val);
int  op_pop(op_stack *s, char *out);
int  op_peek(const op_stack *s, char *out);
int  op_is_empty(const op_stack *s);
int  op_is_full(const op_stack *s);

#endif