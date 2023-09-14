/*
* Caio Vinicius Pereira Silveira
* Leonardo Gonçalves Grossi
* Mariana Gurgel Ferreira
*
* Compiler for a simple programming language
*
* September 2023
*/

/*
* This file includes functions that wrap around
* GHashTable to better organize SymbolTable key/value
* creation and destruction
*/

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <glib.h>

typedef struct SymbolTableKey
{
    // TODO: add level to be able to make multiple scopes
    char* lex;
} SymbolTableKey;

typedef enum DataType
{
    DataType_INTEGER,
    DataType_FLOAT,
    DataType_STRING
} DataType;

typedef struct SymbolTableEntry
{
    DataType type;
    // add more things...
} SymbolTableEntry;

SymbolTableKey* symbol_table_createKey(char* lex);
SymbolTableEntry* symbol_table_createEntry();

GHashTable* symbol_table_new();
void symbol_table_destroy(GHashTable* self);

void symbol_table_print(GHashTable* self);

#endif // SYMBOL_TABLE_H
