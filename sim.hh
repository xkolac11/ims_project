#ifndef SIM_H
#define SIM_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <iostream>


using namespace std;

/** global varibles and functions **/

extern FILE *out;
extern bool out_to_close;

void SetOutput(const char *file);
void Print(const char *str);


#endif
