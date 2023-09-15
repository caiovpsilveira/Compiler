/*
* Caio Vinicius Pereira Silveira
* Leonardo Gonçalves Grossi
* Mariana Gurgel Ferreira
*
* Compiler for a simple programming language
*
* September 2023
*/

#include "util/dstring.h"

#include <stdlib.h>

#define DSTRING_GROWTH_FACTOR 2

void dstring_init(DString* dstring, unsigned initialCapacity)
{
    // capacity and length does not consider '\0', only 'useful' chars
    dstring->capacity = initialCapacity;
    dstring->length = 0;
    dstring->str = (char*) malloc((dstring->capacity + 1) * sizeof(char));
    dstring->str[0] = '\0';
}

void dstring_free(DString* dstring)
{
    free(dstring->str);
}

void dstring_clear(DString* dstring)
{
    dstring->str[0] = '\0';
    dstring->length = 0;
}

void dstring_appendChar(DString* dstring, char c)
{
    if (dstring->length == dstring->capacity)
    {
        dstring->capacity *= DSTRING_GROWTH_FACTOR;
        dstring->str = (char*) realloc(dstring->str, (dstring->capacity + 1) * sizeof(char));
    }

    dstring->str[dstring->length] = c;
    ++dstring->length;
    dstring->str[dstring->length] = '\0';
}

void dstring_shrinkToFit(DString* dstring)
{
    dstring->capacity = dstring->length;
    dstring->str = (char*) realloc(dstring->str, (dstring->capacity + 1) * sizeof(char));
}

char* dstring_steal(DString* dstring, unsigned newCapacity)
{
    char* str = dstring->str;
    dstring_init(dstring, newCapacity);
    return str;
}
