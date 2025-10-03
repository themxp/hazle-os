#include "shell.h"
#include "vga.h"
#include "string.h"
#include "kernel.h"
#include "timer.h"
#include "io.h"

static void cmd_help(int argc, char** argv);
static void cmd_clear(int argc, char** argv);
static void cmd_echo(int argc, char** argv);
static void cmd_info(int argc, char** argv);
static void cmd_time(int argc, char** argv);
static void cmd_calc(int argc, char** argv);
static void cmd_mem(int argc, char** argv);
static void cmd_reboot(int argc, char** argv);
static void cmd_color(int argc, char** argv);
static void cmd_uptime(int argc, char** argv);

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
};

int command_count = sizeof(commands) / sizeof(command_t);

void register_commands(void) {
}

static void cmd_help(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_writeln("Available Commands:");
    vga_writeln("==================");
    
    for (int i = 0; i < command_count; i++) {
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_write("  ");
        vga_write(commands[i].name);
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        vga_write(" - ");
        vga_writeln(commands[i].description);
    }
}

static void cmd_clear(int argc, char** argv) {
    (void)argc;
    (void)argv;
    vga_clear();
}

static void cmd_echo(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        vga_write(argv[i]);
        if (i < argc - 1) {
            vga_putchar(' ');
        }
    }
    vga_putchar('\n');
}

static void cmd_info(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_writeln("System Information:");
    vga_writeln("===================");
    
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_write("OS Name    : ");
    vga_writeln(KERNEL_NAME);
    
    vga_write("Version    : ");
    vga_writeln(KERNEL_VERSION);
    
    vga_write("Architecture: x86 (32-bit)");
    vga_putchar('\n');
    
    vga_write("Total RAM  : ");
    char buffer[32];
    itoa(system_info.total_memory / (1024 * 1024), buffer, 10);
    vga_write(buffer);
    vga_writeln(" MB");
    
    vga_write("Used RAM   : ");
    itoa(system_info.used_memory / (1024 * 1024), buffer, 10);
    vga_write(buffer);
    vga_writeln(" MB");
}

static void cmd_time(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    uint32_t ticks = timer_get_ticks();
    char buffer[32];
    
    vga_write("System ticks: ");
    itoa(ticks, buffer, 10);
    vga_writeln(buffer);
}

static void cmd_calc(int argc, char** argv) {
    if (argc != 4) {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_writeln("Usage: calc <num1> <operator> <num2>");
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        vga_writeln("Operators: + - * /");
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
                vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
                vga_writeln("Error: Division by zero!");
                return;
            }
            result = num1 / num2;
            break;
        default:
            valid = false;
            vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            vga_writeln("Error: Invalid operator!");
            return;
    }
    
    if (valid) {
        char buffer[32];
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_write("Result: ");
        itoa(result, buffer, 10);
        vga_writeln(buffer);
    }
}

static void cmd_mem(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_writeln("Memory Information:");
    vga_writeln("===================");
    
    char buffer[32];
    uint32_t free_mem = system_info.total_memory - system_info.used_memory;
    uint32_t usage_percent = (system_info.used_memory * 100) / system_info.total_memory;
    
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_write("Total Memory : ");
    itoa(system_info.total_memory / (1024 * 1024), buffer, 10);
    vga_write(buffer);
    vga_writeln(" MB");
    
    vga_write("Used Memory  : ");
    itoa(system_info.used_memory / (1024 * 1024), buffer, 10);
    vga_write(buffer);
    vga_writeln(" MB");
    
    vga_write("Free Memory  : ");
    itoa(free_mem / (1024 * 1024), buffer, 10);
    vga_write(buffer);
    vga_writeln(" MB");
    
    vga_write("Usage        : ");
    itoa(usage_percent, buffer, 10);
    vga_write(buffer);
    vga_writeln("%");
}

static void cmd_reboot(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    vga_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga_writeln("Rebooting system...");
    
    uint8_t temp = 0x02;
    while (temp & 0x02) {
        temp = inb(0x64);
    }
    outb(0x64, 0xFE);
    
    __asm__ volatile("hlt");
}

static void cmd_color(int argc, char** argv) {
    if (argc != 3) {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_writeln("Usage: color <fg> <bg>");
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        vga_writeln("Colors: 0-15 (0=black, 7=light grey, 15=white)");
        return;
    }
    
    int fg = atoi(argv[1]);
    int bg = atoi(argv[2]);
    
    if (fg < 0 || fg > 15 || bg < 0 || bg > 15) {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_writeln("Error: Color values must be between 0 and 15");
        return;
    }
    
    vga_set_color((vga_color_t)fg, (vga_color_t)bg);
    vga_writeln("Color scheme changed!");
}

static void cmd_uptime(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    uint32_t seconds = system_info.uptime_seconds;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    
    seconds %= 60;
    minutes %= 60;
    
    char buffer[32];
    
    vga_write("System uptime: ");
    itoa(hours, buffer, 10);
    vga_write(buffer);
    vga_write("h ");
    itoa(minutes, buffer, 10);
    vga_write(buffer);
    vga_write("m ");
    itoa(seconds, buffer, 10);
    vga_write(buffer);
    vga_writeln("s");
}

