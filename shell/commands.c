#include "shell.h"
#include "display.h"
#include "string.h"
#include "kernel.h"
#include "timer.h"
#include "rtc.h"
#include "speaker.h"
#include "vesa.h"
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
static void cmd_resolution(int argc, char** argv);
static void cmd_date(int argc, char** argv);
static void cmd_beep(int argc, char** argv);
static void cmd_random(int argc, char** argv);

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
};

int command_count = sizeof(commands) / sizeof(command_t);

void register_commands(void) {
}

static void cmd_help(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    display_set_color(DISPLAY_COLOR_LIGHT_CYAN, DISPLAY_COLOR_BLACK);
    display_writeln("Available Commands:");
    display_writeln("==================");
    
    for (int i = 0; i < command_count; i++) {
        display_set_color(DISPLAY_COLOR_LIGHT_GREEN, DISPLAY_COLOR_BLACK);
        display_write("  ");
        display_write(commands[i].name);
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
        display_write(" - ");
        display_writeln(commands[i].description);
    }
}

static void cmd_clear(int argc, char** argv) {
    (void)argc;
    (void)argv;
    display_clear();
}

static void cmd_echo(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        display_write(argv[i]);
        if (i < argc - 1) {
            display_putchar(' ');
        }
    }
    display_putchar('\n');
}

static void cmd_info(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    display_set_color(DISPLAY_COLOR_LIGHT_CYAN, DISPLAY_COLOR_BLACK);
    display_writeln("System Information:");
    display_writeln("===================");
    
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    display_write("OS Name      : ");
    display_writeln(KERNEL_NAME);
    
    display_write("Version      : ");
    display_writeln(KERNEL_VERSION);
    
    display_write("Architecture : x86 (32-bit)");
    display_putchar('\n');
    
    display_write("Repository   : ");
    display_set_color(DISPLAY_COLOR_LIGHT_BLUE, DISPLAY_COLOR_BLACK);
    display_writeln("https://github.com/mateuphinxx/hazle-os");
    
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    display_write("Graphics Mode: ");
    display_writeln(vesa_get_mode_name(vesa_get_current_mode()));
    
    char buffer[32];
    display_write("Resolution   : ");
    itoa(vesa_get_width(), buffer, 10);
    display_write(buffer);
    display_write("x");
    itoa(vesa_get_height(), buffer, 10);
    display_writeln(buffer);
    
    display_write("Total RAM    : ");
    itoa(system_info.total_memory / (1024 * 1024), buffer, 10);
    display_write(buffer);
    display_writeln(" MB");
    
    display_write("Used RAM     : ");
    itoa(system_info.used_memory / (1024 * 1024), buffer, 10);
    display_write(buffer);
    display_writeln(" MB");
}

static void cmd_time(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    uint32_t ticks = timer_get_ticks();
    char buffer[32];
    
    display_write("System ticks: ");
    itoa(ticks, buffer, 10);
    display_writeln(buffer);
}

static void cmd_calc(int argc, char** argv) {
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

static void cmd_mem(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    display_set_color(DISPLAY_COLOR_LIGHT_CYAN, DISPLAY_COLOR_BLACK);
    display_writeln("Memory Information:");
    display_writeln("===================");
    
    char buffer[32];
    uint32_t free_mem = system_info.total_memory - system_info.used_memory;
    uint32_t usage_percent = (system_info.used_memory * 100) / system_info.total_memory;
    
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    display_write("Total Memory : ");
    itoa(system_info.total_memory / (1024 * 1024), buffer, 10);
    display_write(buffer);
    display_writeln(" MB");
    
    display_write("Used Memory  : ");
    itoa(system_info.used_memory / (1024 * 1024), buffer, 10);
    display_write(buffer);
    display_writeln(" MB");
    
    display_write("Free Memory  : ");
    itoa(free_mem / (1024 * 1024), buffer, 10);
    display_write(buffer);
    display_writeln(" MB");
    
    display_write("Usage        : ");
    itoa(usage_percent, buffer, 10);
    display_write(buffer);
    display_writeln("%");
}

static void cmd_reboot(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    display_set_color(DISPLAY_COLOR_YELLOW, DISPLAY_COLOR_BLACK);
    display_writeln("Rebooting system...");
    
    uint8_t temp = 0x02;
    while (temp & 0x02) {
        temp = inb(0x64);
    }
    outb(0x64, 0xFE);
    
    __asm__ volatile("hlt");
}

static void cmd_color(int argc, char** argv) {
    if (argc != 3) {
        display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
        display_writeln("Usage: color <fg> <bg>");
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
        display_writeln("Colors: 0-15 (0=black, 7=light grey, 15=white)");
        return;
    }
    
    int fg = atoi(argv[1]);
    int bg = atoi(argv[2]);
    
    if (fg < 0 || fg > 15 || bg < 0 || bg > 15) {
        display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
        display_writeln("Error: Color values must be between 0 and 15");
        return;
    }
    
    display_set_color((display_color_t)fg, (display_color_t)bg);
    display_writeln("Color scheme changed!");
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
    
    display_write("System uptime: ");
    itoa(hours, buffer, 10);
    display_write(buffer);
    display_write("h ");
    itoa(minutes, buffer, 10);
    display_write(buffer);
    display_write("m ");
    itoa(seconds, buffer, 10);
    display_write(buffer);
    display_writeln("s");
}

static void cmd_resolution(int argc, char** argv) {
    if (argc == 1) {
        display_write("Current resolution: ");
        display_writeln(vesa_get_mode_name(vesa_get_current_mode()));
        
        display_writeln("");
        display_writeln("Available modes:");
        
        int count = vesa_get_mode_count();
        for (int i = 0; i < count; i++) {
            const vesa_mode_entry_t* mode = vesa_get_mode_info(i);
            if (mode) {
                char buffer[32];
                display_write("  ");
                itoa(i, buffer, 10);
                display_write(buffer);
                display_write(" - ");
                display_writeln(mode->name);
            }
        }
        
        display_writeln("");
        display_writeln("Usage: resolution <mode>");
        display_writeln("");
        display_set_color(DISPLAY_COLOR_YELLOW, DISPLAY_COLOR_BLACK);
        display_writeln("Note: Resolution is set at compile time.");
        display_writeln("To change resolution:");
        display_writeln("  1. Edit boot/boot.asm (line 16-18)");
        display_writeln("  2. Edit Makefile (line 45)");
        display_writeln("  3. Run: make clean && make run");
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
        return;
    }
    
    display_set_color(DISPLAY_COLOR_YELLOW, DISPLAY_COLOR_BLACK);
    display_writeln("Resolution is set at compile time.");
    display_writeln("Use 'resolution' without arguments for details.");
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
}

static void cmd_date(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    rtc_time_t time;
    rtc_read_time(&time);
    
    char buffer[32];
    
    display_set_color(DISPLAY_COLOR_LIGHT_CYAN, DISPLAY_COLOR_BLACK);
    display_write("Current Date & Time:");
    display_putchar('\n');
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    
    itoa(time.year, buffer, 10);
    display_write(buffer);
    display_write("-");
    
    if (time.month < 10) display_write("0");
    itoa(time.month, buffer, 10);
    display_write(buffer);
    display_write("-");
    
    if (time.day < 10) display_write("0");
    itoa(time.day, buffer, 10);
    display_write(buffer);
    display_write("  ");
    
    if (time.hour < 10) display_write("0");
    itoa(time.hour, buffer, 10);
    display_write(buffer);
    display_write(":");
    
    if (time.minute < 10) display_write("0");
    itoa(time.minute, buffer, 10);
    display_write(buffer);
    display_write(":");
    
    if (time.second < 10) display_write("0");
    itoa(time.second, buffer, 10);
    display_writeln(buffer);
}

static void cmd_beep(int argc, char** argv) {
    uint32_t frequency = 440;
    uint32_t duration = 200;
    
    if (argc >= 2) {
        frequency = atoi(argv[1]);
        if (frequency < 20 || frequency > 20000) {
            display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
            display_writeln("Error: Frequency must be between 20 and 20000 Hz");
            display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
            return;
        }
    }
    
    if (argc >= 3) {
        duration = atoi(argv[2]);
        if (duration < 1 || duration > 5000) {
            display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
            display_writeln("Error: Duration must be between 1 and 5000 ms");
            display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
            return;
        }
    }
    
    speaker_beep(frequency, duration);
    
    display_set_color(DISPLAY_COLOR_LIGHT_GREEN, DISPLAY_COLOR_BLACK);
    display_writeln("Beep!");
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
}

static void cmd_random(int argc, char** argv) {
    uint32_t max = 100;
    
    if (argc >= 2) {
        max = atoi(argv[1]);
        if (max < 1) {
            display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
            display_writeln("Error: Max must be greater than 0");
            display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
            return;
        }
    }
    
    uint32_t random = timer_get_ticks() % max;
    
    char buffer[32];
    display_set_color(DISPLAY_COLOR_LIGHT_GREEN, DISPLAY_COLOR_BLACK);
    display_write("Random number: ");
    itoa(random, buffer, 10);
    display_writeln(buffer);
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
}
