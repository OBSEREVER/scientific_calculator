#include "stack.h"
#include <ctype.h>
#include <stdio.h>

int main(void)
{
    NumStack nums;
    OpStack ops;
    num_init(&nums);
    op_init(&ops);

    char expression[STACK_SIZE];
    int num = 0;
    int in_number = 0;

    printf("Enter expression: ");
    if (fgets(expression, sizeof(expression), stdin) == NULL)
    {
        return 1;
    }

    for (int i = 0; expression[i] != '\0'; i++)
    {
        char ch = expression[i];

        if (isdigit((unsigned char)ch))
        {
            num = num * 10 + (ch - '0');
            in_number = 1;
        }
        else
        {
            if (in_number)
            {
                num_push(&nums, num);
                num = 0;
                in_number = 0;
            }
            if (!isspace((unsigned char)ch))
            {
                op_push(&ops, ch);
            }
        }
    }
    if (in_number)
    {
        num_push(&nums, num);
    }

    printf("Numbers (top first): ");
    while (!num_is_empty(&nums))
    {
        printf("%g ", num_pop(&nums));
    }

    printf("\nOperators (top first): ");
    while (!op_is_empty(&ops))
    {
        printf("%c ", op_pop(&ops));
    }
    printf("\n");

    return 0;
}