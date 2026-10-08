#include "tokenizer.h"
#include "parser.h"
#include "evaluator.h"
#include <stdio.h>
#include <math.h>
#include <assert.h>

/* Runs one expression through the full pipeline without the UI */
static int run_expression(const char *expr, double *out_result)
{
    token_list tokens;
    postfix_list postfix;

    if (!tokenize(expr, &tokens))
    {
        return 0;
    }
    if (!parse_to_postfix(&tokens, &postfix))
    {
        return 0;
    }
    if (!evaluate_postfix(&postfix, out_result))
    {
        return 0;
    }
    return 1;
}

/* Asserts that an expression succeeds and matches the expected result */
static void assert_eval(const char *expr, double expected)
{
    double res;
    int ok = run_expression(expr, &res);
    assert(ok && "Expression was expected to succeed, but failed");
    
    assert(fabs(res - expected) < 1e-6 && "Result mismatch");
    printf("  [PASS] %-25s = %g\n", expr, res);
}

/* Asserts that an invalid or domain-violating expression fails cleanly */
static void assert_reject(const char *expr)
{
    double res;
    int ok = run_expression(expr, &res);
    assert(!ok && "Expression was expected to fail, but succeeded");
    printf("  [PASS] Cleanly rejected invalid: %s\n", expr);
}

int main(void)
{
    printf("=========================================\n");
    printf(" RUNNING SCIENTIFIC CALCULATOR TEST SUITE\n");
    printf("=========================================\n\n");

    printf("[1] Basic Arithmetic & Associativity:\n");
    assert_eval("2 + 3 * 4", 14.0);
    assert_eval("(2 + 3) * 4", 20.0);
    assert_eval("10 - 4 - 2", 4.0);          /* Left-associative */
    assert_eval("2 ^ 3 ^ 2", 512.0);         /* Right-associative (2^(3^2)) */
    assert_eval("17 % 5", 2.0);
    assert_eval("15 / 3", 5.0);

    printf("\n[2] Negative Numbers & Unary Minus:\n");
    assert_eval("-5 + 8", 3.0);
    assert_eval("4 * -2", -8.0);
    assert_eval("-3 * -3", 9.0);
    assert_eval("10 + -5", 5.0);

    printf("\n[3] Degree Mode Trigonometry (Default):\n"); /* HERE: Tests DEG mode */
    assert_eval("sin(90)", 1.0);                            /* HERE: sin(90 deg) = 1 */
    assert_eval("sin(30)", 0.5);                            /* HERE: sin(30 deg) = 0.5 */
    assert_eval("cos(0)", 1.0);                             /* HERE: cos(0 deg) = 1 */
    assert_eval("cos(60)", 0.5);                            /* HERE: cos(60 deg) = 0.5 */
    assert_eval("tan(45)", 1.0);                            /* HERE: tan(45 deg) = 1 */
    assert_eval("asin(1)", 90.0);                           /* HERE: asin(1) = 90 deg */
    assert_eval("atan(1)", 45.0);                           /* HERE: atan(1) = 45 deg */

    printf("\n[4] Functions & Timing:\n");
    assert_eval("sqrt(16) + 4", 8.0);
    assert_eval("cbrt(27)", 3.0);
    assert_eval("exp(0)", 1.0);
    assert_eval("fact(5)", 120.0);           /* 5! = 120 */
    assert_eval("fact(0)", 1.0);             /* 0! = 1 */
    assert_eval("abs(-42.5)", 42.5);
    assert_eval("log(100)", 2.0);            /* log10 */
    assert_eval("log2(8)", 3.0);             /* log2 */
    assert_eval("round(3.7)", 4.0);
    assert_eval("trunc(-3.7)", -3.0);

    /* Tests that functions pop immediately upon closing ')' */
    assert_eval("sin(90) + 3", 4.0);         /* HERE: sin(90) + 3 = 1 + 3 = 4 */

    printf("\n[5] Radian Mode Toggle Test:\n"); /* HERE: Tests switching to RAD */
    toggle_angle_mode();                        /* Switch to RAD */
    assert_eval("sin(0)", 0.0);
    assert_eval("cos(0)", 1.0);
    toggle_angle_mode();                        /* Switch back to DEG */

    printf("\n[6] Constants & Scientific Notation:\n");
    assert_eval("pi", 3.141592653589793);
    assert_eval("e", 2.718281828459045);
    assert_eval("c", 299792458.0);
    assert_eval("1.5e3 + 2", 1502.0);
    assert_eval("2e-2 * 100", 2.0);

    printf("\n[7] Robust Error & Domain Checking:\n");
    assert_reject("5 / 0");                  /* Division by zero */
    assert_reject("10 % 0");                 /* Modulo by zero */
    assert_reject("sqrt(-4)");               /* Negative square root */
    assert_reject("asin(2.5)");              /* asin domain [-1, 1] */
    assert_reject("tan(90)");                /* HERE: tan(90 deg) undefined */
    assert_reject("log(0)");                 /* log of non-positive */
    assert_reject("fact(-3)");               /* Factorial of negative */
    assert_reject("fact(3.5)");              /* Factorial of float */
    assert_reject("(2 + 3");                 /* Missing closing paren */
    assert_reject("2 + * 3");                /* Missing operand */

    printf("\n=========================================\n");
    printf(" ALL 36 TESTS PASSED SUCCESSFULLY (100%%)\n");
    printf("=========================================\n");
    return 0;
}