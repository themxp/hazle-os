#include "../../userlib/hazle.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    puts("Hazle OS v1.0.0");
    puts("");
    puts("built-in commands:");
    puts("  cd <path>       - change directory");
    puts("  exit            - exit shell");
    puts("  reboot          - restart system");
    puts("  shutdown [time] - halt system (now/1s/1m/1h, default 1m)");
    puts("");
    puts("file commands:");
    puts("  ls [-a]         - list directory (-a for all)");
    puts("  cat <file>      - display file");
    puts("  mkdir <name>    - create directory");
    puts("  touch <file>    - create file");
    puts("  rm <path>       - delete file/directory");
    puts("  cp <src> <dst>  - copy file");
    puts("  mv <src> <dst>  - move/rename file");
    puts("  write <f> <txt> - write text to file");
    puts("  pwd             - print working directory");
    puts("");
    puts("system commands:");
    puts("  echo <text>     - print text");
    puts("  clear           - clear screen");
    puts("  info            - system info");
    puts("  mem             - memory info");
    puts("  disk            - disk info");
    puts("  time            - system ticks");
    puts("  date            - current date");
    puts("  uptime          - system uptime");
    puts("");
    puts("other commands:");
    puts("  calc <a> <op> <b> - calculator");
    puts("  color <fg> <bg> - change colors");
    puts("  beep [freq] [ms] - play beep");
    puts("  random [max]    - random number");
    
    return 0;
}
