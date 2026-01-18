#include "../userlib/hazle.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    puts("Hazle OS v1.0.0");
    puts("");
    puts("available commands:");
    puts("  echo <text>     - print text");
    puts("  clear           - clear screen");
    puts("  pwd             - print working directory");
    puts("  ls [-h]         - list directory (-h for hidden)");
    puts("  cat <file>      - display file");
    puts("  mkdir <name>    - create directory");
    puts("  touch <file>    - create file");
    puts("  rm <path>       - delete file/directory");
    puts("  cp <src> <dst>  - copy file");
    puts("  mv <src> <dst>  - move/rename file");
    puts("  write <f> <txt> - write text to file");
    puts("  cd <path>       - change directory (built-in)");
    puts("  exit            - exit shell (built-in)");
    puts("  info            - system info");
    puts("  mem             - memory info");
    puts("  disk            - disk info");
    puts("  time            - system ticks");
    puts("  date            - current date");
    puts("  uptime          - system uptime");
    puts("  calc <a> <op> <b> - calculator");
    puts("  color <fg> <bg> - change colors");
    puts("  beep [freq] [ms] - play beep");
    puts("  random [max]    - random number");
    puts("  reboot          - restart system");
    
    return 0;
}
