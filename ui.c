#define NCURSES_MOUSE_VERSION 2
#define PDC_NCMOUSE

#include "ui.h"
#include "evaluator.h"

#if defined(__has_include)
#if __has_include(<pdcurses.h>)
#include <pdcurses.h>
#elif __has_include(<pdcurses/curses.h>)
#include <pdcurses/curses.h>
#else
#include <curses.h>
#endif
#else
#include <pdcurses.h>
#endif

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define WIN_HEIGHT 22
#define WIN_WIDTH 74
#define GRID_ROWS 6
#define GRID_COLS 7

enum
{
    ACT_INSERT,
    ACT_DEL,
    ACT_CLEAR,
    ACT_ANS,
    ACT_EVAL,
    ACT_EXIT,
    ACT_MODE,
    ACT_HELP,
    ACT_SHIFT
};

typedef struct
{
    const char *label;
    const char *insert;
    const char *shift_label;
    const char *shift_insert;
    int action;
} calc_button;

static const calc_button button_grid[GRID_ROWS][GRID_COLS] = {
    {{" SHIFT", NULL, " SHIFT", NULL, ACT_SHIFT},
     {" sin  ", "sin(", " asin ", "asin(", ACT_INSERT},
     {" cos  ", "cos(", " acos ", "acos(", ACT_INSERT},
     {" tan  ", "tan(", " atan ", "atan(", ACT_INSERT},
     {" sqrt ", "sqrt(", " cbrt ", "cbrt(", ACT_INSERT},
     {"  ln  ", "ln(", " exp  ", "exp(", ACT_INSERT},
     {" DEG  ", NULL, " DEG  ", NULL, ACT_MODE}},
    {{" sinh ", "sinh(", " asinh", "asinh(", ACT_INSERT},
     {" cosh ", "cosh(", " acosh", "acosh(", ACT_INSERT},
     {" tanh ", "tanh(", " atanh", "atanh(", ACT_INSERT},
     {" log  ", "log(", " log2 ", "log2(", ACT_INSERT},
     {" abs  ", "abs(", " fact ", "fact(", ACT_INSERT},
     {" round", "round(", " trunc", "trunc(", ACT_INSERT},
     {" floor", "floor(", " ceil ", "ceil(", ACT_INSERT}},
    {{"  pi  ", "pi", "  pi  ", "pi", ACT_INSERT},
     {"  e   ", "e", "  e   ", "e", ACT_INSERT},
     {" CONST", NULL, " CONST", NULL, ACT_HELP},
     {"  ^   ", "^", "  ^   ", "^", ACT_INSERT},
     {"  (   ", "(", "  (   ", "(", ACT_INSERT},
     {"  )   ", ")", "  )   ", ")", ACT_INSERT},
     {" EXP  ", "e", " 1/x  ", "1/(", ACT_INSERT}},
    {{"  7   ", "7", "  7   ", "7", ACT_INSERT},
     {"  8   ", "8", "  8   ", "8", ACT_INSERT},
     {"  9   ", "9", "  9   ", "9", ACT_INSERT},
     {"  /   ", "/", "  /   ", "/", ACT_INSERT},
     {"  *   ", "*", "  *   ", "*", ACT_INSERT},
     {" DEL  ", NULL, " DEL  ", NULL, ACT_DEL},
     {" CLR  ", NULL, " CLR  ", NULL, ACT_CLEAR}},
    {{"  4   ", "4", "  4   ", "4", ACT_INSERT},
     {"  5   ", "5", "  5   ", "5", ACT_INSERT},
     {"  6   ", "6", "  6   ", "6", ACT_INSERT},
     {"  -   ", "-", "  -   ", "-", ACT_INSERT},
     {"  +   ", "+", "  +   ", "+", ACT_INSERT},
     {" Ans  ", NULL, " Ans  ", NULL, ACT_ANS},
     {" EXIT ", NULL, " EXIT ", NULL, ACT_EXIT}},
    {{"  1   ", "1", "  1   ", "1", ACT_INSERT},
     {"  2   ", "2", "  2   ", "2", ACT_INSERT},
     {"  3   ", "3", "  3   ", "3", ACT_INSERT},
     {"  0   ", "0", "  0   ", "0", ACT_INSERT},
     {"  .   ", ".", "  .   ", ".", ACT_INSERT},
     {"  %   ", "%", "  %   ", "%", ACT_INSERT},
     {"  =   ", NULL, "  =   ", NULL, ACT_EVAL}}};

static WINDOW *calc_win = NULL;
static char last_status[128] = "Ready. Select a function or type expression.";
static int last_is_error = 0;
static double last_result_val = 0.0;
static int has_last_result = 0;
static int is_shift_active = 0;

static void center_window(void)
{
    int term_h, term_w;
    getmaxyx(stdscr, term_h, term_w);

    int start_y = (term_h > WIN_HEIGHT) ? (term_h - WIN_HEIGHT) / 2 : 0;
    int start_x = (term_w > WIN_WIDTH) ? (term_w - WIN_WIDTH) / 2 : 0;

    if (calc_win != NULL)
    {
        mvwin(calc_win, start_y, start_x);
    }
}

static void show_constants_popup(void)
{
    int term_h, term_w;
    getmaxyx(stdscr, term_h, term_w);

    int pw = 52;
    int ph = 12;
    int py = (term_h > ph) ? (term_h - ph) / 2 : 2;
    int px = (term_w > pw) ? (term_w - pw) / 2 : 2;

    WINDOW *pop = newwin(ph, pw, py, px);
    if (!pop)
    {
        return;
    }

    keypad(pop, TRUE);

    if (has_colors())
    {
        wattron(pop, COLOR_PAIR(1));
    }
    box(pop, 0, 0);

    mvwprintw(pop, 1, (pw - 26) / 2, "PHYSICAL CONSTANTS GUIDE");
    mvwaddch(pop, 2, 0, ACS_LTEE);
    mvwhline(pop, 2, 1, ACS_HLINE, pw - 2);
    mvwaddch(pop, 2, pw - 1, ACS_RTEE);

    if (has_colors())
    {
        wattroff(pop, COLOR_PAIR(1));
        wattron(pop, COLOR_PAIR(6));
    }

    mvwprintw(pop, 3, 4, "c  : Speed of Light    299,792,458 m/s");
    mvwprintw(pop, 4, 4, "g  : Standard Gravity  9.80665 m/s^2");
    mvwprintw(pop, 5, 4, "h  : Planck Constant   6.62607e-34 J*s");
    mvwprintw(pop, 6, 4, "k  : Boltzmann Const   1.38065e-23 J/K");
    mvwprintw(pop, 7, 4, "na : Avogadro Number   6.02214e+23 1/mol");
    mvwprintw(pop, 8, 4, "qe : Elementary Charge 1.60218e-19 C");

    if (has_colors())
    {
        wattroff(pop, COLOR_PAIR(6));
        wattron(pop, COLOR_PAIR(1));
    }

    mvwaddch(pop, 9, 0, ACS_LTEE);
    mvwhline(pop, 9, 1, ACS_HLINE, pw - 2);
    mvwaddch(pop, 9, pw - 1, ACS_RTEE);

    if (has_colors())
    {
        wattroff(pop, COLOR_PAIR(1));
        wattron(pop, COLOR_PAIR(4));
    }

    mvwprintw(pop, 10, (pw - 26) / 2, "Type name | Any key to exit");

    if (has_colors())
    {
        wattroff(pop, COLOR_PAIR(4));
    }

    wrefresh(pop);
    wgetch(pop);
    delwin(pop);

    touchwin(calc_win);
    wrefresh(calc_win);
}

int ui_init(void)
{
    if (initscr() == NULL)
    {
        return 0;
    }

    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

    if (has_colors())
    {
        start_color();
        init_pair(1, COLOR_CYAN, COLOR_BLACK);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
        init_pair(3, COLOR_BLACK, COLOR_CYAN);
        init_pair(4, COLOR_GREEN, COLOR_BLACK);
        init_pair(5, COLOR_RED, COLOR_BLACK);
        init_pair(6, COLOR_YELLOW, COLOR_BLACK);
        init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
    }

    int term_h, term_w;
    getmaxyx(stdscr, term_h, term_w);
    int start_y = (term_h > WIN_HEIGHT) ? (term_h - WIN_HEIGHT) / 2 : 0;
    int start_x = (term_w > WIN_WIDTH) ? (term_w - WIN_WIDTH) / 2 : 0;

    calc_win = newwin(WIN_HEIGHT, WIN_WIDTH, start_y, start_x);
    if (calc_win == NULL)
    {
        endwin();
        return 0;
    }

    keypad(calc_win, TRUE);
    return 1;
}

static void draw_calculator(const char *buffer, int sel_row, int sel_col)
{
    werase(calc_win);

    if (has_colors())
    {
        wattron(calc_win, COLOR_PAIR(1));
    }
    box(calc_win, 0, 0);

    /* Header title */
    char header_title[64];
    snprintf(header_title, sizeof(header_title), "SCIENTIFIC CALCULATOR [%s]%s",
             get_angle_mode() ? "DEG" : "RAD",
             is_shift_active ? " [S]" : "");
    mvwprintw(calc_win, 1, (WIN_WIDTH - (int)strlen(header_title)) / 2, "%s", header_title);

    /* Dividers */
    mvwaddch(calc_win, 2, 0, ACS_LTEE);
    mvwhline(calc_win, 2, 1, ACS_HLINE, WIN_WIDTH - 2);
    mvwaddch(calc_win, 2, WIN_WIDTH - 1, ACS_RTEE);

    mvwaddch(calc_win, 5, 0, ACS_LTEE);
    mvwhline(calc_win, 5, 1, ACS_HLINE, WIN_WIDTH - 2);
    mvwaddch(calc_win, 5, WIN_WIDTH - 1, ACS_RTEE);

    mvwaddch(calc_win, 18, 0, ACS_LTEE);
    mvwhline(calc_win, 18, 1, ACS_HLINE, WIN_WIDTH - 2);
    mvwaddch(calc_win, 18, WIN_WIDTH - 1, ACS_RTEE);

    if (has_colors())
    {
        wattroff(calc_win, COLOR_PAIR(1));
    }

    /* Status / Result display */
    int status_color = last_is_error ? 5 : (has_last_result ? 4 : 1);
    if (has_colors())
    {
        wattron(calc_win, COLOR_PAIR(status_color) | A_BOLD);
    }
    mvwprintw(calc_win, 3, 3, "%-68.68s", last_status);
    if (has_colors())
    {
        wattroff(calc_win, COLOR_PAIR(status_color) | A_BOLD);
    }

    /* Active Expression input line */
    const char *disp_ptr = buffer;
    int len = (int)strlen(buffer);
    if (len > 60)
    {
        disp_ptr = buffer + (len - 60);
    }
    mvwprintw(calc_win, 4, 3, "Expr > %-60.60s", disp_ptr);

    /* Interactive keypad */
    for (int r = 0; r < GRID_ROWS; r++)
    {
        int by = 6 + r * 2;
        for (int c = 0; c < GRID_COLS; c++)
        {
            int bx = 3 + c * 10;
            const calc_button *btn = &button_grid[r][c];
            int is_selected = (r == sel_row && c == sel_col);

            const char *display_label = is_shift_active ? btn->shift_label : btn->label;
            if (btn->action == ACT_MODE)
            {
                display_label = get_angle_mode() ? " DEG  " : " RAD  ";
            }

            if (is_selected)
            {
                if (has_colors())
                {
                    wattron(calc_win, COLOR_PAIR(3) | A_BOLD);
                }
                else
                {
                    wattron(calc_win, A_REVERSE);
                }
            }
            else
            {
                int color = 2;
                if (btn->action == ACT_SHIFT && is_shift_active)
                {
                    color = 6; /* Highlight SHIFT button when active */
                }
                else if (r < 2 || (r == 2 && c == 6))
                {
                    color = is_shift_active ? 6 : 2; /* Functions and EXP/1/x light yellow in shift mode */
                }
                else if (btn->action == ACT_EVAL || btn->action == ACT_DEL ||
                         btn->action == ACT_CLEAR || btn->action == ACT_EXIT ||
                         btn->action == ACT_MODE || btn->action == ACT_HELP)
                {
                    color = 7; /* Action buttons in magenta */
                }

                if (has_colors())
                {
                    wattron(calc_win, COLOR_PAIR(color));
                }
            }

            mvwprintw(calc_win, by, bx, "[%s]", display_label);

            if (is_selected)
            {
                if (has_colors())
                {
                    wattroff(calc_win, COLOR_PAIR(3) | A_BOLD);
                }
                else
                {
                    wattroff(calc_win, A_REVERSE);
                }
            }
            else if (has_colors())
            {
                wattroff(calc_win, COLOR_PAIR(2) | COLOR_PAIR(6) | COLOR_PAIR(7));
            }
        }
    }

    /* Footer hints */
    mvwprintw(calc_win, 19, 3, "Arrows/Mouse: Select  | Enter/Space: Push | Del: Backspace");
    mvwprintw(calc_win, 20, 3, "Keyboard: Type keys   | 's': Toggle SHIFT | '?' : Const Guide");

    wrefresh(calc_win);
}

static void remove_last_element(char *buffer, int *expr_len)
{
    const char *funcs[] = {
        "asinh(", "acosh(", "atanh(", "asin(", "acos(", "atan(",
        "sinh(", "cosh(", "tanh(", "round(", "floor(", "ceil(", "trunc(",
        "sqrt(", "cbrt(", "log2(", "fact(", "log(", "ln(", "exp(", "abs(", "1/(",
        "sin(", "cos(", "tan("};
    int count = (int)(sizeof(funcs) / sizeof(funcs[0]));

    for (int i = 0; i < count; i++)
    {
        int flen = (int)strlen(funcs[i]);
        if (*expr_len >= flen && strcmp(buffer + *expr_len - flen, funcs[i]) == 0)
        {
            *expr_len -= flen;
            buffer[*expr_len] = '\0';
            return;
        }
    }

    if (*expr_len > 0)
    {
        (*expr_len)--;
        buffer[*expr_len] = '\0';
    }
}

int ui_get_expression(char *buffer, int buffer_size)
{
    int expr_len = 0;
    buffer[0] = '\0';

    int sel_row = 0;
    int sel_col = 0;

    while (1)
    {
        draw_calculator(buffer, sel_row, sel_col);
        int ch = wgetch(calc_win);

        if (ch == KEY_RESIZE)
        {
            center_window();
            continue;
        }

        if (ch == KEY_MOUSE)
        {
            MEVENT event;
            if (getmouse(&event) == OK)
            {
                if (event.bstate & (BUTTON1_CLICKED | BUTTON1_PRESSED))
                {
                    int win_y, win_x;
                    getbegyx(calc_win, win_y, win_x);
                    int my = event.y - win_y;
                    int mx = event.x - win_x;

                    for (int r = 0; r < GRID_ROWS; r++)
                    {
                        int by = 6 + r * 2;
                        if (my == by)
                        {
                            for (int c = 0; c < GRID_COLS; c++)
                            {
                                int bx = 3 + c * 10;
                                if (mx >= bx && mx < bx + 8)
                                {
                                    sel_row = r;
                                    sel_col = c;
                                    ch = '\n';
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (ch == KEY_UP)
        {
            sel_row = (sel_row + GRID_ROWS - 1) % GRID_ROWS;
            continue;
        }
        if (ch == KEY_DOWN)
        {
            sel_row = (sel_row + 1) % GRID_ROWS;
            continue;
        }
        if (ch == KEY_LEFT)
        {
            sel_col = (sel_col + GRID_COLS - 1) % GRID_COLS;
            continue;
        }
        if (ch == KEY_RIGHT)
        {
            sel_col = (sel_col + 1) % GRID_COLS;
            continue;
        }

        /* Execute focused button */
        if (ch == '\n' || ch == '\r' || ch == KEY_ENTER || ch == ' ')
        {
            const calc_button *btn = &button_grid[sel_row][sel_col];

            if (btn->action == ACT_SHIFT)
            {
                is_shift_active = !is_shift_active;
                snprintf(last_status, sizeof(last_status), "SHIFT mode %s.",
                         is_shift_active ? "ON" : "OFF");
                continue;
            }

            if (btn->action == ACT_INSERT)
            {
                const char *to_insert = is_shift_active ? btn->shift_insert : btn->insert;
                int ins_len = (int)strlen(to_insert);
                if (expr_len + ins_len < buffer_size - 1)
                {
                    memcpy(buffer + expr_len, to_insert, ins_len);
                    expr_len += ins_len;
                    buffer[expr_len] = '\0';
                }
                if (is_shift_active)
                {
                    is_shift_active = 0;
                }
            }
            else if (btn->action == ACT_MODE)
            {
                toggle_angle_mode();
                snprintf(last_status, sizeof(last_status), "Mode changed to %s.",
                         get_angle_mode() ? "DEG (Degrees)" : "RAD (Radians)");
            }
            else if (btn->action == ACT_HELP)
            {
                show_constants_popup();
            }
            else if (btn->action == ACT_DEL)
            {
                remove_last_element(buffer, &expr_len);
            }
            else if (btn->action == ACT_CLEAR)
            {
                expr_len = 0;
                buffer[0] = '\0';
            }
            else if (btn->action == ACT_ANS)
            {
                if (has_last_result)
                {
                    char ans_buf[32];
                    snprintf(ans_buf, sizeof(ans_buf), "%g", last_result_val);
                    int alen = (int)strlen(ans_buf);
                    if (expr_len + alen < buffer_size - 1)
                    {
                        memcpy(buffer + expr_len, ans_buf, alen);
                        expr_len += alen;
                        buffer[expr_len] = '\0';
                    }
                }
            }
            else if (btn->action == ACT_EVAL)
            {
                if (expr_len > 0)
                {
                    return 1;
                }
            }
            else if (btn->action == ACT_EXIT)
            {
                return 0;
            }
            continue;
        }

        if (ch == 's' || ch == 'S')
        {
            is_shift_active = !is_shift_active;
            snprintf(last_status, sizeof(last_status), "SHIFT mode %s.",
                     is_shift_active ? "ON" : "OFF");
            continue;
        }

        if (ch == '?')
        {
            show_constants_popup();
            continue;
        }

        if (ch == '=')
        {
            if (expr_len > 0)
            {
                return 1;
            }
            continue;
        }

        if (ch == 'q' || ch == 'Q')
        {
            if (expr_len == 0)
            {
                return 0;
            }
        }

        if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b')
        {
            remove_last_element(buffer, &expr_len);
            continue;
        }

        if (ch == 27)
        {
            expr_len = 0;
            buffer[0] = '\0';
            continue;
        }

        if (isprint((unsigned char)ch))
        {
            if (expr_len < buffer_size - 1)
            {
                buffer[expr_len++] = (char)ch;
                buffer[expr_len] = '\0';
            }
        }
    }
}

void ui_show_result(double result)
{
    has_last_result = 1;
    last_result_val = result;
    last_is_error = 0;
    snprintf(last_status, sizeof(last_status), "Result: %g", result);
}

void ui_show_error(const char *message)
{
    last_is_error = 1;
    snprintf(last_status, sizeof(last_status), "Error: %s", message);
}

void ui_close(void)
{
    if (calc_win != NULL)
    {
        delwin(calc_win);
        calc_win = NULL;
    }
    endwin();
}