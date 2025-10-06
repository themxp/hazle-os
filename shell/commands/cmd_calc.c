#include "commands.h"
#include "display.h"
#include "string.h"
#include "types.h"

void cmd_calc(int argc, char** argv) {
    if (argc != 4) {
        display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
        display_writeln("Usage: calc <num1> <operator> <num2>");
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
        display_writeln("Operators: + - * /");
        return;
    }
    
    int num1 = atoi(argv[1]);
    char op = argv[2][0];
    int num2 = atoi(argv[3]);
    int result = 0;
    bool valid = true;
    
    switch (op) {
        case '+': result = num1 + num2; break;
        case '-': result = num1 - num2; break;
        case '*': result = num1 * num2; break;
        case '/':
            if (num2 == 0) {
                display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
                display_writeln("Error: Division by zero!");
                return;
            }
            result = num1 / num2;
            break;
        default:
            valid = false;
            display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
            display_writeln("Error: Invalid operator!");
            return;
    }
    
    if (valid) {
        char buffer[32];
        display_set_color(DISPLAY_COLOR_LIGHT_GREEN, DISPLAY_COLOR_BLACK);
        display_write("Result: ");
        itoa(result, buffer, 10);
        display_writeln(buffer);
    }
}
