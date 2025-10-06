#ifndef COMMANDS_H
#define COMMANDS_H

#include "shell.h"

// Command function declarations
void cmd_help(int argc, char** argv);
void cmd_clear(int argc, char** argv);
void cmd_echo(int argc, char** argv);
void cmd_info(int argc, char** argv);
void cmd_time(int argc, char** argv);
void cmd_calc(int argc, char** argv);
void cmd_mem(int argc, char** argv);
void cmd_reboot(int argc, char** argv);
void cmd_color(int argc, char** argv);
void cmd_uptime(int argc, char** argv);
void cmd_resolution(int argc, char** argv);
void cmd_date(int argc, char** argv);
void cmd_beep(int argc, char** argv);
void cmd_random(int argc, char** argv);
void cmd_debug(int argc, char** argv);

#endif
