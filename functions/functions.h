#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MIN_PORT 1024
#define MAX_PORT 49151

#define NICK_ALREADY_IN_USE "NICK_ALREADY_IN_USE"

#define CLEAR_SCREEN() system("cls")

int get_int(const int min, const int max);

#endif