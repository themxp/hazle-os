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
    {"ls", "List directory contents", cmd_ls},
    {"cd", "Change directory", cmd_cd},
    {"pwd", "Print working directory", cmd_pwd},
    {"cat", "Display file contents", cmd_cat},
    {"mkdir", "Create directory", cmd_mkdir},
    {"touch", "Create empty file", cmd_touch},
    {"rm", "Delete file or directory", cmd_rm},
    {"cp", "Copy file (usage: cp <src> <dst>)", cmd_cp},
    {"mv", "Move/rename file (usage: mv <src> <dst>)", cmd_mv},
    {"disk", "Display disk information", cmd_disk},
    {"write", "Write text to file (usage: write <file> <text>)", cmd_write},
};

int command_count = sizeof(commands) / sizeof(command_t);

void register_commands(void) {
}