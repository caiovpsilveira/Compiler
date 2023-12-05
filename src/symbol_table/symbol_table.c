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

#include <assert.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

const char* data_type_toString(DataType dt)
{
    const char* str;
    switch (dt)
    {
    case DataType_INT:
        str = "DataType_INT";
        break;
    case DataType_FLOAT:
        str = "DataType_FLOAT";
        break;
    case DataType_STRING:
        str = "DataType_STRING";
        break;
    case DataType_BOOLEAN:
        str = "DataType_BOOLEAN";
        break;
    default:
        assert("Invalid DataType value" && 0);
        break;
    }
    return str;
}

const char* data_type_toUserString(DataType dt)
{
    const char* str;
    switch (dt)
    {
    case DataType_INT:
        str = "INT";
        break;
    case DataType_FLOAT:
        str = "FLOAT";
        break;
    case DataType_STRING:
        str = "STRING";
        break;
    case DataType_BOOLEAN:
        str = "BOOLEAN";
        break;
    default:
        assert("Invalid DataType value" && 0);
        break;
    }
    return str;
}

char* _st_keyToString(const SymbolTableKey* key)
{   
    return key->lex;
}

unsigned _st_hash_func(const void* key)
{
    const SymbolTableKey* stKey = (const SymbolTableKey*) key;
    char* keyStr = _st_keyToString(stKey);
    unsigned ret = g_str_hash(keyStr);
    // IF KEY IS MODIFIED TO BE OTHER THAN LEX, KEYS HAVE TO BE PROPERLY DELETED
    return ret;
}

int _st_key_equal_func(const void* k1, const void* k2)
{
    const SymbolTableKey* stKey1 = (const SymbolTableKey*) k1;
    const SymbolTableKey* stKey2 = (const SymbolTableKey*) k2;

    char* key1Str = _st_keyToString(stKey1);
    char* key2Str = _st_keyToString(stKey2);

    int res = g_str_equal(key1Str, key2Str);

    // IF KEY IS MODIFIED TO BE OTHER THAN LEX, KEYS HAVE TO BE PROPERLY DELETED

    return res;
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

SymbolTableEntry* symbol_table_createEntry(DataType dt)
{
    SymbolTableEntry* stEntryPtr = (SymbolTableEntry*) malloc(sizeof(SymbolTableEntry));
    stEntryPtr->dtype = dt;
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
