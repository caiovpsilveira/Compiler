/*
* Caio Vinicius Pereira Silveira
* Leonardo Gonçalves Grossi
* Mariana Gurgel Ferreira
*
* Compiler for a simple programming language
*
* September 2023
*/

#ifndef DSTRING_H
#define DSTRING_H

// Dynamically resized 'C str' style char array
typedef struct DString
{
    char* str;
    unsigned length;
    unsigned capacity;
} DString;

void dstring_init(DString* dstring, unsigned initialCapacity);
void dstring_free(DString* dstring);
void dstring_clear(DString* dstring);
void dstring_appendChar(DString* dstring, char c);
// Shrinks the buffer to fit to the stored string, including terminating '\0'
void dstring_shrinkToFit(DString* dstring);
// Returns the buffer and allocates a new one, with the provided capacity
char* dstring_steal(DString* dstring, unsigned newCapacity);

#endif // DSTRING_H
