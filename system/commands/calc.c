#include "../../userlib/hazle.h"

int main(int argc, char **argv) {
    if (argc != 4) {
        puts("usage: calc <num1> <op> <num2>");
        return 2;
    }
    
    int a = atoi(argv[1]);
    char op = argv[2][0];
    int b = atoi(argv[3]);
    int result = 0;
    
    switch (op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/':
            if (b == 0) {
                puts("division by zero");
                return 1;
            }
            result = a / b;
            break;
        case '%':
            if (b == 0) {
                puts("division by zero");
                return 1;
            }
            result = a % b;
            break;
        default:
            puts("unknown operator");
            return 2;
    }
    
    char buf[32];
    itoa(result, buf, 10);
    puts(buf);
    
    return 0;
}
