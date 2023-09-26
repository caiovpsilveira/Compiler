/*
* Caio Vinicius Pereira Silveira
* Leonardo Gonçalves Grossi
* Mariana Gurgel Ferreira
*
* Compiler for a simple programming language
*
* September 2023
*/

#include "debug.h"
#include "lexical/lexical_analyzer.h"
#include "lexical/token.h"
#include "symbol_table/symbol_table.h"
#include "util/dstring.h"

#include <assert.h>
#include <glib.h> // GHashTable
#include <stdio.h>
#include <stdlib.h>

#define LA_INITIAL_LEX_CAPACITY 30

struct LexicalAnalyzer
{
    unsigned line;
    unsigned column;
    DString lex;
    FILE* file;

    GHashTable* reservedSymbols; // hashmap string (reserved symbols) -> TokenType
    // NOTE: symbol table does not recognize scope levels, as this language is a single scope
    // Otherwize, the key should be tuple <lex, level>, and should be managed by the syntatical
    GHashTable* symbolTable;
    GHashTable* literals; // Used as a set to existing literals, to avoid duplicating strings on memory
};

void _la_insertTokenTypeIntoHash(GHashTable* hash, char* key, TokenType tt)
{
    TokenType* ttPtr = (TokenType*) malloc(sizeof(TokenType));
    *ttPtr = tt;
    g_hash_table_insert(hash, key, ttPtr);

    DEBUG_PRINT("Inserted \"%s\" into hash.\n", token_type_toString(tt));
}

void _la_initReservedSymbols(GHashTable* reservedSymbols)
{
    _la_insertTokenTypeIntoHash(reservedSymbols, "class", TokenType_CLASS);
    _la_insertTokenTypeIntoHash(reservedSymbols, "int", TokenType_INT);
    _la_insertTokenTypeIntoHash(reservedSymbols, "string", TokenType_STRING);
    _la_insertTokenTypeIntoHash(reservedSymbols, "float", TokenType_FLOAT);

    _la_insertTokenTypeIntoHash(reservedSymbols, "if", TokenType_IF);
    _la_insertTokenTypeIntoHash(reservedSymbols, "else", TokenType_ELSE);

    _la_insertTokenTypeIntoHash(reservedSymbols, "do", TokenType_DO);
    _la_insertTokenTypeIntoHash(reservedSymbols, "while", TokenType_WHILE);

    _la_insertTokenTypeIntoHash(reservedSymbols, "read", TokenType_READ);
    _la_insertTokenTypeIntoHash(reservedSymbols, "write", TokenType_WRITE);

    _la_insertTokenTypeIntoHash(reservedSymbols, "{", TokenType_OPEN_CUR);
    _la_insertTokenTypeIntoHash(reservedSymbols, "}", TokenType_CLOSE_CUR);
    _la_insertTokenTypeIntoHash(reservedSymbols, "(", TokenType_OPEN_PAR);
    _la_insertTokenTypeIntoHash(reservedSymbols, ")", TokenType_CLOSE_PAR);
    _la_insertTokenTypeIntoHash(reservedSymbols, ";", TokenType_SEMICOLON);
    _la_insertTokenTypeIntoHash(reservedSymbols, ",", TokenType_COLON);
    _la_insertTokenTypeIntoHash(reservedSymbols, "=", TokenType_ASSIGN);

    _la_insertTokenTypeIntoHash(reservedSymbols, ">", TokenType_GREATER);
    _la_insertTokenTypeIntoHash(reservedSymbols, ">=", TokenType_GREATER_EQ);
    _la_insertTokenTypeIntoHash(reservedSymbols, "<", TokenType_LOWER);
    _la_insertTokenTypeIntoHash(reservedSymbols, "<=", TokenType_LOWER_EQ);
    _la_insertTokenTypeIntoHash(reservedSymbols, "==", TokenType_EQUALS);
    _la_insertTokenTypeIntoHash(reservedSymbols, "!", TokenType_NOT);
    _la_insertTokenTypeIntoHash(reservedSymbols, "!=", TokenType_NOT_EQUALS);
    _la_insertTokenTypeIntoHash(reservedSymbols, "&&", TokenType_AND);
    _la_insertTokenTypeIntoHash(reservedSymbols, "||", TokenType_OR);
    _la_insertTokenTypeIntoHash(reservedSymbols, "+", TokenType_ADD);
    _la_insertTokenTypeIntoHash(reservedSymbols, "-", TokenType_SUB);
    _la_insertTokenTypeIntoHash(reservedSymbols, "*", TokenType_MUL);
    _la_insertTokenTypeIntoHash(reservedSymbols, "/", TokenType_DIV);

    assert(g_hash_table_size(reservedSymbols) == TokenType_SIZE - 5); // All minus END_OF_FILE, ID, LITERAL, INTEGER and REAL
}

LexicalAnalyzer* lexical_analyzer_new(GHashTable* st, char* filepath)
{
    LexicalAnalyzer* la = (LexicalAnalyzer*) malloc(sizeof(LexicalAnalyzer));
    la->file = fopen(filepath, "r");

    if (!la->file)
    {
        fprintf(stderr, "Error: cannot open file \"%s\" in read mode. Exiting.\n", filepath);
        exit(-1);
    }
    la->line = 1;
    la->column = 1;
    dstring_init(&la->lex, LA_INITIAL_LEX_CAPACITY);
    la->reservedSymbols = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free); // NULL because it's a literal
    _la_initReservedSymbols(la->reservedSymbols);
    la->symbolTable = st;
    la->literals = g_hash_table_new_full(g_str_hash, g_str_equal, free, NULL); // NULL because its a set (key = val)
    DEBUG_PRINT("Finished constructing Lexical Analyzer.\n");
    return la;
}

void lexical_analyzer_destroy(LexicalAnalyzer* self)
{
    if (self->file)
        fclose(self->file);
    dstring_free(&self->lex);
    g_hash_table_destroy(self->reservedSymbols);
    g_hash_table_destroy(self->literals);
    free(self);
}

void _la_showExpectedCharErrorAndExit(LexicalAnalyzer* self, char expectedChar, char gotChar)
{
    fprintf(stderr, "Error at line %d column %d: expected \"%c\", got \"%c\".\n", self->line, self->column, expectedChar, gotChar);
    exit(-1);
}

void _la_showExpectedSequenceErrorAndExit(LexicalAnalyzer* self, char* expectedSequence, char gotChar)
{
    fprintf(stderr, "Error at line %d column %d: expected \"%s\", got \"%c\".\n", self->line, self->column, expectedSequence, gotChar);
    exit(-1);
}

void _la_showMissingSequenceErrorAndExit(LexicalAnalyzer* self, char* missingSequence)
{
    fprintf(stderr, "Error at line %d column %d: missing \"%s\".\n", self->line, self->column, missingSequence);
    exit(-1);
}

void _la_showInvalidCharErrorAndExit(LexicalAnalyzer* self, char invalidChar)
{
    fprintf(stderr, "Error at line %d column %d: invalid char \"%c\".\n", self->line, self->column, invalidChar);
    exit(-1); 
}

int _la_isFinalState(unsigned state)
{
    return state == 51 ||
           state == 52 ||
           state == 53 ||
           state == 54;
}

int _la_isLetter(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z');
}

int _la_isDigit(char c)
{
    return c >= '0' && c <= '9';
}

int _la_isNonZeroDigit(char c)
{
  return c >= '1' && c <= '9';
}

Token lexical_analyzer_getToken(LexicalAnalyzer* self)
{
    int i;
    char c;
    unsigned state = 0;

    while (!_la_isFinalState(state))
    {
        i = fgetc(self->file);

        if (i == EOF)
        {
            break;
        }

        c = (char) i;
        switch (state) {
        case 0:
            if (c == '\n')
            {
                ++self->line;
                self->column = 0;
                state = 0;
            }
            else if (c == '\t' ||
                     c == '\r' ||
                     c == ' ')
            {
                state = 0;
            }
            else if (c == '<' ||
                     c == '>' ||
                     c == '=' ||
                     c == '!')
            {
                dstring_appendChar(&self->lex, c);
                state = 1;
            }
            else if (c == '|')
            {
                dstring_appendChar(&self->lex, c);
                state = 2;
            }
            else if (c == '&')
            {
                dstring_appendChar(&self->lex, c);
                state = 3;
            }
            else if (_la_isLetter(c))
            {
                dstring_appendChar(&self->lex, c);
                state = 4;
            }
            else if (c == '{' ||
                     c == '}' ||
                     c == '(' ||
                     c == ')' ||
                     c == ';' ||
                     c == ',' ||
                     c == '+' ||
                     c == '-' ||
                     c == '*')
            {
                dstring_appendChar(&self->lex, c);
                state = 53;
            }
            else if (c == '/')
            {
                dstring_appendChar(&self->lex, c); // this has to be removed later if it's a comment
                state = 5;
            }
            else if (c == '\"')
            {
                state = 6;
            }
            else if (_la_isNonZeroDigit(c))
            {
                dstring_appendChar(&self->lex, c);
                state = 7;
            }
            else if (c == '0')
            {
                dstring_appendChar(&self->lex, c);
                state = 8;
            }
            else
            {
                _la_showInvalidCharErrorAndExit(self, c); // invalid starting character
            }
            break;
        case 1:
            if (c == '=')
            {
                dstring_appendChar(&self->lex, c);
                state = 53;
            }
            else
            {
                ungetc(c, self->file);
                state = 53;
            }
            break;
        case 2:
            if (c == '|')
            {
                dstring_appendChar(&self->lex, c);
                state = 53;
            }
            else
            {
                // invalid character different from | after |
                _la_showExpectedCharErrorAndExit(self, '|', c);
            }
            break;
        case 3:
            if (c == '&')
            {
                dstring_appendChar(&self->lex, c);
                state = 53;
            }
            else
            {
                // invalid character different from & after &
                _la_showExpectedCharErrorAndExit(self, '&', c);
            }
            break;
        case 4:
            if (_la_isLetter(c) ||
                _la_isDigit(c) ||
                c == '_')
            {
                dstring_appendChar(&self->lex, c);
                state = 4;
            }
            else
            {
                ungetc(c, self->file);
                state = 53;
            }
            break;
        case 5:
            if (c == '/')
            {
                dstring_clear(&self->lex); // remove previously added '/' into lex
                state = 11;
            }
            else if (c == '*')
            {
                dstring_clear(&self->lex); // remove previously added '/' into lex
                state = 12;
            }
            else
            {
                ungetc(c, self->file);
                state = 53;
            }
            break;
        case 6:
            if (c == '\"')
            {
                state = 54;
            }
            else if (c == '\n')
            {
                // invalid character "\n" inside literal quotes
                _la_showMissingSequenceErrorAndExit(self, "\"");
            }
            else
            {
                dstring_appendChar(&self->lex, c);
                state = 6;
            }
            break;
        case 7:
            if (_la_isDigit(c))
            {
                dstring_appendChar(&self->lex, c);
                state = 7;
            }
            else if (c == '.')
            {
                dstring_appendChar(&self->lex, c);
                state = 9;
            }
            else
            {
                ungetc(c, self->file);
                state = 51;
            }
            break;
        case 8:
            if (c == '.')
            {
                dstring_appendChar(&self->lex, c);
                state = 9;
            }
            else
            {
                // NOTE: 0 will always retorn a token 0
                // If there is a sequence of 010, it will output TokenInteger(0), TokenInteger(10)
                ungetc(c, self->file);
                state = 51;
            }
            break;
        case 9:
            if (_la_isDigit(c))
            {
                dstring_appendChar(&self->lex, c);
                state = 10;
            }
            else
            {
                // invalid char different from digit after (digit)+'.'
                _la_showExpectedSequenceErrorAndExit(self, "digit", c);
            }
            break;
        case 10:
            if (_la_isDigit(c))
            {
                dstring_appendChar(&self->lex, c);
                state = 10;
            }
            else
            {
                ungetc(c, self->file);
                state = 52;
            }
            break;
        case 11:
            if (c == '\n')
            {
                ++self->line;
                self->column = 0;
                state = 0;
            }
            else
            {
                state = 11;
            }
            break;
        case 12:
            if (c == '*')
            {
                state = 13;
            }
            else if (c == '\n')
            {
                ++self->line;
                self->column = 0;
                state = 12;
            }
            else
            {
                state = 12;
            }
            break;
        case 13:
            if (c == '/')
            {
                state = 0;
            }
            else if (c == '*')
            {
                state = 13;
            }
            else if (c == '\n')
            {
                ++self->line;
                self->column = 0;
                state = 12;
            }
            else
            {
                state = 12;
            }
            break;
        default:
            assert("Invalid state value in getToken loop." && 0);
            break;
        }
        ++self->column;
    }

    if (!_la_isFinalState(state) && state != 0)
    {
        // unexpected EOF
        switch (state)
        {
        case 2:
            // unexpected EOF after |
            _la_showMissingSequenceErrorAndExit(self, "|");
            break;
        case 3:
            // unexpected EOF after &
            _la_showMissingSequenceErrorAndExit(self, "&");
            break;
        case 6:
            // unexpected EOF after literal quote start
            _la_showMissingSequenceErrorAndExit(self, "\"");
            break;
        case 12:
            // comment /* not finished
            _la_showMissingSequenceErrorAndExit(self, "*/");
            break;
        case 13:
            // comment /* not finished, but last char was *
            _la_showMissingSequenceErrorAndExit(self, "/");
            break;
        default:
            assert("Unexpected EOF error state not handled." && 0);
            break;
        }
    }

    Token t;
    char* endptr;
    void* curKey;
    void* curValue;

    switch (state)
    {
    case 0:
        t.type = TokenType_END_OF_FILE;
        break;
    case 51:
        t.type = TokenType_INTEGER;
        t.longVal = strtol(self->lex.str, &endptr, 10);
        assert(self->lex.str + self->lex.length == endptr); // lex contains only convertible chars
        dstring_clear(&self->lex);
        break;
    case 52:
        t.type = TokenType_REAL;
        t.doubleVal = strtod(self->lex.str, &endptr);
        assert(self->lex.str + self->lex.length == endptr); // lex contains only convertible chars
        dstring_clear(&self->lex);
        break;
    case 53:
        curValue = g_hash_table_lookup(self->reservedSymbols, self->lex.str);
        if (curValue == NULL) // is not a reserved word or symbol -> identifier
        {
            t.type = TokenType_ID;
            SymbolTableKey lookupKey = {self->lex.str};
            if (g_hash_table_lookup_extended(self->symbolTable, &lookupKey, &curKey, &curValue))
            {
                // entry already exists. clear lex and assign token st_key to existing one
                DEBUG_PRINT("SymbolTableEntry for lex \"%s\" already exists in table."
                    " Assigning to already existing key ptr %p.\n", self->lex.str, curKey);
                t.st_key = (SymbolTableKey*) curKey;
                dstring_clear(&self->lex);
            }
            else // entry does not exist
            {
                dstring_shrinkToFit(&self->lex);
                char* stKeyLex = dstring_steal(&self->lex, LA_INITIAL_LEX_CAPACITY);
                SymbolTableKey* stKeyPtr = symbol_table_createKey(stKeyLex);
                t.st_key = stKeyPtr; // bind token to symbol table
                SymbolTableEntry* stEntryPtr = symbol_table_createEntry();
                g_hash_table_insert(self->symbolTable, stKeyPtr, stEntryPtr);
                DEBUG_PRINT("Token ID with lex \"%s\" inserted into symbol table. Key ptr is %p.\n", stKeyLex, (void*) stKeyPtr);
            }
        }
        else // it is a reserved word or symbol
        {
            t.type = *((const TokenType*) curValue);
            dstring_clear(&self->lex);
        }
        break;
    case 54:
        t.type = TokenType_LITERAL;
        if (g_hash_table_lookup_extended(self->literals, self->lex.str, &curKey, &curValue))
        {
            assert(curKey == curValue); // NOTE: key == value because we are using as a set
            // entry already exists. clear lex and assign token literal to existing one
            DEBUG_PRINT("Literal \"%s\" already exists in table."
                " Assigning to already existing key ptr %p.\n", self->lex.str, curKey);
            t.literal = (char*) curKey;
            dstring_clear(&self->lex);
        }
        else
        {
            dstring_shrinkToFit(&self->lex);
            g_hash_table_add(self->literals, self->lex.str);
            DEBUG_PRINT("New literal \"%s\" inserted into literal table. Key ptr is %p.\n", self->lex.str, self->lex.str);
            t.literal = dstring_steal(&self->lex, LA_INITIAL_LEX_CAPACITY);
        }
        break;
    default:
        assert("Invalid final state reached." && 0);
        break;
    }

    return t;
}
