CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -O2 -DPDC_WIDE

# Detect Windows vs Unix/Linux/macOS
ifeq ($(OS),Windows_NT)
    CURSES_LIB = -lpdcurses
    TARGET = calc.exe
    TEST_TARGET = test_calc.exe
    RM = rm -f
else
    CURSES_LIB = -lncurses
    TARGET = calc
    TEST_TARGET = test_calc
    RM = rm -f
endif

# Math library
MATH_LIB = -lm

# Source files for core engine (shared by GUI and Tests)
CORE_SRC = tokenizer.c parser.c evaluator.c stack.c
CORE_OBJ = $(CORE_SRC:.c=.o)

# GUI sources
GUI_SRC = main.c ui.c
GUI_OBJ = $(GUI_SRC:.c=.o)

.PHONY: all test run clean

# Default target: builds the calculator application
all: $(TARGET)

$(TARGET): $(CORE_OBJ) $(GUI_OBJ)
	$(CC) $(CORE_OBJ) $(GUI_OBJ) -o $@ $(CURSES_LIB) $(MATH_LIB)

# Test target: compiles the test runner without curses and runs it
test: $(TEST_TARGET)
	@echo "--- Running automated unit tests ---"
	./$(TEST_TARGET)

$(TEST_TARGET): $(CORE_OBJ) test.o
	$(CC) $(CORE_OBJ) test.o -o $@ $(MATH_LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the calculator
run: $(TARGET)
	./$(TARGET)

# Clean compiled objects and binaries
clean:
	$(RM) $(CORE_OBJ) $(GUI_OBJ) test.o $(TARGET) $(TEST_TARGET)