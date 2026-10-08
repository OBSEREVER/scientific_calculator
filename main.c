#include "tokenizer.h"
#include "parser.h"
#include "evaluator.h"
#include "ui.h"
#include <stdio.h>

int main(void)
{
    char expression[MAX_TOKENS * MAX_TOKEN_LEN];

    if (ui_init() == 0)
    {
        fprintf(stderr, "Error: failed to initialize UI\n");
        return 1;
    }

    while (ui_get_expression(expression, sizeof(expression)))
    {
        token_list tokens;
        postfix_list postfix;
        double result;

        if (!tokenize(expression, &tokens))
        {
            ui_show_error("Invalid expression: could not tokenize input");
            continue;
        }

        if (!parse_to_postfix(&tokens, &postfix))
        {
            ui_show_error("Invalid expression: parsing failed");
            continue;
        }

        if (!evaluate_postfix(&postfix, &result))
        {
            ui_show_error("Invalid expression: evaluation failed");
            continue;
        }

        ui_show_result(result);
    }

    ui_close();
    return 0;
}