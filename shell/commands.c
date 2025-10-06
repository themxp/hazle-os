#include "shell.h"
#include "commands.h"


command_t commands[] = {
    {"help", "Display available commands", cmd_help},
    {"clear", "Clear the screen", cmd_clear},
    {"echo", "Print text to screen", cmd_echo},
    {"info", "Display system information", cmd_info},
    {"time", "Show system ticks", cmd_time},
    {"calc", "Simple calculator (usage: calc <num1> <op> <num2>)", cmd_calc},
    {"mem", "Display memory information", cmd_mem},
    {"reboot", "Restart the system", cmd_reboot},
    {"color", "Change text color (usage: color <fg> <bg>)", cmd_color},
    {"uptime", "Show system uptime", cmd_uptime},
    {"resolution", "Change screen resolution (usage: resolution <mode>)", cmd_resolution},
    {"date", "Show current date and time", cmd_date},
    {"beep", "Play a beep sound (usage: beep [frequency] [duration])", cmd_beep},
    {"random", "Generate random number (usage: random [max])", cmd_random},
    {"debug", "Display debug information", cmd_debug},
};

int command_count = sizeof(commands) / sizeof(command_t);

void register_commands(void) {
}