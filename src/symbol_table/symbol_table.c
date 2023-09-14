/*
* Caio Vinicius Pereira Silveira
* Leonardo Gonçalves Grossi
* Mariana Gurgel Ferreira
*
* Compiler for a simple programming language
*
* September 2023
*/

#include "symbol_table/symbol_table.h"

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

unsigned _st_hash_func(const void* key)
{
    const SymbolTableKey* stKey = (const SymbolTableKey*) key;
    return g_str_hash(stKey->lex);
}

int _st_key_equal_func(const void* k1, const void* k2)
{
    const SymbolTableKey* stKey1 = (const SymbolTableKey*) k1;
    const SymbolTableKey* stKey2 = (const SymbolTableKey*) k2;
    return g_str_equal(stKey1->lex, stKey2->lex);
}

void _st_key_destroy_func(void* key)
{
    SymbolTableKey* stKey = (SymbolTableKey*) key;
    free(stKey->lex);
    free(stKey);
}

void _st_value_destroy_func(void* value)
{
    free(value);
}

SymbolTableKey* symbol_table_createKey(char* lex)
{
    SymbolTableKey* stKeyPtr = (SymbolTableKey*) malloc(sizeof(SymbolTableKey));
    stKeyPtr->lex = lex;
    return stKeyPtr;
}

SymbolTableEntry* symbol_table_createEntry()
{
    SymbolTableEntry* stEntryPtr = (SymbolTableEntry*) malloc(sizeof(SymbolTableEntry));
    return stEntryPtr;
}

GHashTable* symbol_table_new()
{
    GHashTable* st = g_hash_table_new_full(_st_hash_func, _st_key_equal_func, _st_key_destroy_func, _st_value_destroy_func);
    return st;
}

void symbol_table_destroy(GHashTable* self)
{
    g_hash_table_destroy(self);
}

void _st_hash_table_printf_foreach(void* key, void* value, void* userData)
{
    value = value; // remove warnings. this is intentional, as they will not be used but are required for the API
    userData = userData;
    SymbolTableKey* stKey = (SymbolTableKey*) key;
    printf("SymbolTable key: lex: %s\n", stKey->lex);
}

void symbol_table_print(GHashTable* self)
{
    g_hash_table_foreach(self, _st_hash_table_printf_foreach, NULL);
}
