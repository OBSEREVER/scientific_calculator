#ifndef UI_H
#define UI_H

/* Initializes the ncurses UI. Returns 1 on success, 0 on failure. */
int ui_init(void);

/* Displays the calculator UI and reads one full expression from the user.
   Returns 1 if an expression was entered, 0 if the user chose to quit. */
int ui_get_expression(char *buffer, int buffer_size);

/* Displays the computed result on the screen. */
void ui_show_result(double result);

/* Displays an error message on the screen without crashing the UI. */
void ui_show_error(const char *message);

/* Shuts down ncurses cleanly and restores the terminal. */
void ui_close(void);

#endif