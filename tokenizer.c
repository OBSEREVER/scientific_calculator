#include "stack.h"
#include <ctype.h>
#include <stdio.h>

int main()
{
    operand_stack s;
    operand_stack_init(&s);
    operator_stack s;
    operator_stack_init(&s);

    char expression[STACK_SIZE];
    char ch;
    int num = 0;

    printf("Enter expression: ");
    fgets(expression, sizeof(expression), stdin);
    for (int i = 0; expression[i] != '\0'; i++)
    {
        ch = expression[i];
        if (!isdigit(ch))
        {
            operator_stack_push(&s, ch);
        }
        else
        {
            num = num * 10 + (ch - '0');
        }
    }
    operand_stack_push(&s, num);

    int stacked;
    while (!operand_stack_empty(&s));
    {
        stacked = operand_stack_pop(&s);
        printf("%d ", stacked);
    }

    return 0;
}