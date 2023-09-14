/*
* Caio Vinicius Pereira Silveira
* Leonardo Gonçalves Grossi
* Mariana Gurgel Ferreira
*
* Compiler for a simple programming language
*
* September 2023
*/

#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG // compile with -DDEBUG to enable
#include <stdio.h>
#include <string.h>
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define DEBUG_PRINT(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILENAME__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)
#else
#define DEBUG_PRINT(fmt, ...) do {} while (0)
#endif

#endif // DEBUG_H
