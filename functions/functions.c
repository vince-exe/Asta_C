#include "functions.h"

int get_int(const int min, const int max) {
    char buffer[10];
    int n;
    
    do {
        fgets(buffer, 10, stdin);
        buffer[strlen(buffer) - 1] = '\0';
        n = atoi(buffer);
    } while(n < min || n > max);

    return n;
}