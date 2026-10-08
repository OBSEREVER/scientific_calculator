#include "evaluator.h"
#include "stack.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

#define PI 3.14159265358979323846 /* HERE: High-precision pi for degree/radian conversion */

/* HERE: Angle mode state (1 = DEG, 0 = RAD, default to DEG like standard scientific calculators) */
static int is_degree_mode = 1;

/* HERE: Functions to get and toggle angle mode */
void toggle_angle_mode(void)
{
    is_degree_mode = !is_degree_mode;
}

int get_angle_mode(void)
{
    return is_degree_mode;
}

/* HERE: Degree/Radian-aware trigonometric wrapper functions */
static double safe_sin(double a)
{
    if (is_degree_mode)
    {
        a = a * (PI / 180.0);
    }
    return sin(a);
}

static double safe_cos(double a)
{
    if (is_degree_mode)
    {
        a = a * (PI / 180.0);
    }
    return cos(a);
}

static double safe_tan(double a)
{
    if (is_degree_mode)
    {
        /* In degree mode, check for undefined asymptotes like 90, 270 */
        double rem = fmod(fabs(a), 180.0);
        if (fabs(rem - 90.0) < 1e-9)
        {
            return NAN;
        }
        a = a * (PI / 180.0);
    }
    return tan(a);
}

static double safe_asin(double a)
{
    if (a < -1.0 || a > 1.0)
    {
        return NAN;
    }
    double res = asin(a);
    if (is_degree_mode)
    {
        res = res * (180.0 / PI);
    }
    return res;
}

static double safe_acos(double a)
{
    if (a < -1.0 || a > 1.0)
    {
        return NAN;
    }
    double res = acos(a);
    if (is_degree_mode)
    {
        res = res * (180.0 / PI);
    }
    return res;
}

static double safe_atan(double a)
{
    double res = atan(a);
    if (is_degree_mode)
    {
        res = res * (180.0 / PI);
    }
    return res;
}

/* Safe wrapper functions that enforce mathematical domains */
static double safe_sqrt(double a)  { return (a < 0.0) ? NAN : sqrt(a); }
static double safe_log(double a)   { return (a <= 0.0) ? NAN : log10(a); }
static double safe_ln(double a)    { return (a <= 0.0) ? NAN : log(a); }
static double safe_log2(double a)  { return (a <= 0.0) ? NAN : log2(a); }
static double safe_acosh(double a) { return (a < 1.0) ? NAN : acosh(a); }
static double safe_atanh(double a) { return (a <= -1.0 || a >= 1.0) ? NAN : atanh(a); }

/* Factorial via Gamma function: fact(n) = gamma(n + 1) */
static double safe_fact(double a)
{
    if (a < 0.0 || floor(a) != a || a > 170.0)
    {
        return NAN;
    }
    return tgamma(a + 1.0);
}

/* Function Pointer Struct: pairs a text name with a function address */
typedef struct
{
    const char *name;
    double (*func)(double);
} func_entry;

static const func_entry MATH_FUNCTIONS[] = {
    {"sin",   safe_sin},   /* HERE: Uses angle-aware wrapper */
    {"cos",   safe_cos},   /* HERE: Uses angle-aware wrapper */
    {"tan",   safe_tan},   /* HERE: Uses angle-aware wrapper */
    {"asin",  safe_asin},  /* HERE: Returns degrees when in DEG mode */
    {"acos",  safe_acos},  /* HERE: Returns degrees when in DEG mode */
    {"atan",  safe_atan},  /* HERE: Returns degrees when in DEG mode */
    {"sinh",  sinh},
    {"cosh",  cosh},
    {"tanh",  tanh},
    {"asinh", asinh},
    {"acosh", safe_acosh},
    {"atanh", safe_atanh},
    {"sqrt",  safe_sqrt},
    {"cbrt",  cbrt},
    {"log",   safe_log},
    {"ln",    safe_ln},
    {"log2",  safe_log2},
    {"exp",   exp},
    {"abs",   fabs},
    {"floor", floor},
    {"ceil",  ceil},
    {"round", round},
    {"trunc", trunc},
    {"fact",  safe_fact}
};

static int apply_operator(num_stack *s, char op)
{
    double a, b;
    if (!num_pop(s, &b))
    {
        return 0;
    }
    if (!num_pop(s, &a))
    {
        fprintf(stderr, "Error: missing operand for operator '%c'\n", op);
        return 0;
    }

    double res;
    switch (op)
    {
        case '+':
            res = a + b;
            break;
        case '-':
            res = a - b;
            break;
        case '*':
            res = a * b;
            break;
        case '/':
            if (b == 0.0)
            {
                fprintf(stderr, "Error: division by zero\n");
                return 0;
            }
            res = a / b;
            break;
        case '%':
            if (b == 0.0)
            {
                fprintf(stderr, "Error: modulo by zero\n");
                return 0;
            }
            res = fmod(a, b);
            break;
        case '^':
            res = pow(a, b);
            break;
        default:
            fprintf(stderr, "Error: unknown operator '%c'\n", op);
            return 0;
    }

    if (isnan(res) || isinf(res))
    {
        fprintf(stderr, "Error: result out of range for operator '%c'\n", op);
        return 0;
    }

    return num_push(s, res);
}

static int apply_function(num_stack *s, const char *name)
{
    double a;
    if (!num_pop(s, &a))
    {
        fprintf(stderr, "Error: missing operand for function '%s'\n", name);
        return 0;
    }

    double res = NAN;
    int matched = 0;
    size_t count = sizeof(MATH_FUNCTIONS) / sizeof(MATH_FUNCTIONS[0]);

    for (size_t i = 0; i < count; i++)
    {
        if (strcmp(name, MATH_FUNCTIONS[i].name) == 0)
        {
            res = MATH_FUNCTIONS[i].func(a);
            matched = 1;
            break;
        }
    }

    if (!matched)
    {
        fprintf(stderr, "Error: unknown function '%s'\n", name);
        return 0;
    }

    if (isnan(res) || isinf(res))
    {
        fprintf(stderr, "Error: invalid input or domain error for '%s'\n", name);
        return 0;
    }

    return num_push(s, res);
}

int evaluate_postfix(const postfix_list *postfix, double *result)
{
    num_stack nums;
    num_init(&nums);

    for (int i = 0; i < postfix->count; i++)
    {
        token t = postfix->tokens[i];

        if (t.type == TOKEN_NUMBER)
        {
            if (!num_push(&nums, t.value))
            {
                return 0;
            }
        }
        else if (t.type == TOKEN_OPERATOR)
        {
            if (!apply_operator(&nums, t.text[0]))
            {
                return 0;
            }
        }
        else if (t.type == TOKEN_FUNCTION)
        {
            if (!apply_function(&nums, t.text))
            {
                return 0;
            }
        }
        else
        {
            fprintf(stderr, "Error: unexpected token in postfix evaluation\n");
            return 0;
        }
    }

    double final;
    if (!num_pop(&nums, &final))
    {
        fprintf(stderr, "Error: empty expression, nothing to evaluate\n");
        return 0;
    }

    if (!num_is_empty(&nums))
    {
        fprintf(stderr, "Error: malformed expression, leftover operands\n");
        return 0;
    }

    *result = final;
    return 1;
}