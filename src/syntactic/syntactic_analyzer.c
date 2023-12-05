/*
* Caio Vinicius Pereira Silveira
* Leonardo Gonçalves Grossi
* Mariana Gurgel Ferreira
*
* Compiler for a simple programming language
*
* October 2023
*/

#include "syntactic/syntactic_analyzer.h"

#include "lexical/lexical_analyzer.h"
#include "lexical/token.h"
#include "symbol_table/symbol_table.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct SyntacticAnalyzer
{
    GHashTable* symbolTable;
    LexicalAnalyzer* lexicalAnalyzer;
    Token curToken;
};

void _sa_showExpectedErrorAndExit(SyntacticAnalyzer* self, const char* expectedStr)
{
    unsigned line = lexical_analyzer_getLine(self->lexicalAnalyzer);
    unsigned column = lexical_analyzer_getColumn(self->lexicalAnalyzer);
    const char* gotTypeStr = token_type_toUserString(self->curToken.type);
    char* gotLexStr = token_lexemeToString(&self->curToken);

    fprintf(stderr, "Error at line %d column %d: expected \"%s\", got \"%s\"", line, column, expectedStr, gotTypeStr);
    if (gotLexStr)
    {
        fprintf(stderr, "(%s)", gotLexStr);
        token_destroyLexemeString(gotLexStr);
    }
    
    fprintf(stderr, ".\n");
    exit(-1);
}

void _sem_showAlreadyDeclaredIdentifierAndExit(const SyntacticAnalyzer* self, const char* identifierLex)
{
    unsigned line = lexical_analyzer_getLine(self->lexicalAnalyzer);
    unsigned column = lexical_analyzer_getColumn(self->lexicalAnalyzer);
    fprintf(stderr, "Error at line %d column %d: already declared identifier \"%s\".\n", line, column, identifierLex);
    exit(-1);
}

void _sem_showUndeclaredIdentifierAndExit(SyntacticAnalyzer* self, const char* identifierLex)
{
    unsigned line = lexical_analyzer_getLine(self->lexicalAnalyzer);
    unsigned column = lexical_analyzer_getColumn(self->lexicalAnalyzer);
    fprintf(stderr, "Error at line %d column %d: use of undeclared identifier \"%s\".\n", line, column, identifierLex);
    exit(-1);
}

void _sem_showMismatchedDataTypesAndExit(SyntacticAnalyzer* self, DataType dt1, DataType dt2)
{
    unsigned line = lexical_analyzer_getLine(self->lexicalAnalyzer);
    unsigned column = lexical_analyzer_getColumn(self->lexicalAnalyzer);
    const char* dt1Str = data_type_toUserString(dt1);
    const char* dt2Str = data_type_toUserString(dt2);
    fprintf(stderr, "Error at line %d column %d: DataTypes differs: \"%s\" and \"%s\".\n", line, column, dt1Str, dt2Str);
    exit(-1);
}

void _sem_showInvalidOperatorAndExit(SyntacticAnalyzer* self, DataType dt, TokenType tt)
{
    unsigned line = lexical_analyzer_getLine(self->lexicalAnalyzer);
    unsigned column = lexical_analyzer_getColumn(self->lexicalAnalyzer);
    const char* dtStr = data_type_toUserString(dt);
    const char* ttStr = token_type_toUserString(tt);
    fprintf(stderr, "Error at line %d column %d: DataTypes \"%s\" does not support operator \"%s\".\n", line, column, dtStr, ttStr);
    exit(-1);
}

void _sa_advance(SyntacticAnalyzer* self)
{
    self->curToken = lexical_analyzer_getToken(self->lexicalAnalyzer);
}

void _sa_eat(SyntacticAnalyzer* self, TokenType type)
{
    if (self->curToken.type == type)
    {
        _sa_advance(self);
    }
    else
    {
        const char* expectedStr = token_type_toUserString(type);
        _sa_showExpectedErrorAndExit(self, expectedStr);
    }
}

SyntacticAnalyzer* syntactic_analyzer_new(GHashTable* st, LexicalAnalyzer* la)
{
    SyntacticAnalyzer* sa = (SyntacticAnalyzer*) malloc(sizeof(SyntacticAnalyzer));
    sa->symbolTable = st;
    sa->lexicalAnalyzer = la;
    _sa_advance(sa); // init first token
    return sa;
}

void syntactic_analyzer_destroy(SyntacticAnalyzer* self)
{
    free(self);
}

// Forward declaration of non terminal symbols rules
void _sa_proc_program(SyntacticAnalyzer* self);
void _sa_proc_decl_list(SyntacticAnalyzer* self);
void _sa_proc_body(SyntacticAnalyzer* self);
void _sa_proc_decl(SyntacticAnalyzer* self);
DataType _sa_proc_type(SyntacticAnalyzer* self);
void _sa_proc_stmt_list(SyntacticAnalyzer* self);
void _sa_proc_ident_list(SyntacticAnalyzer* self, DataType dt);
void _sa_proc_stmt(SyntacticAnalyzer* self);
void _sa_proc_assign_stmt(SyntacticAnalyzer* self);
void _sa_proc_if_stmt(SyntacticAnalyzer* self);
void _sa_proc_do_stmt(SyntacticAnalyzer* self);
void _sa_proc_read_stmt(SyntacticAnalyzer* self);
void _sa_proc_write_stmt(SyntacticAnalyzer* self);
DataType _sa_proc_simple_expr(SyntacticAnalyzer* self);
void _sa_proc_condition(SyntacticAnalyzer* self);
void _sa_proc_if_stmt_i(SyntacticAnalyzer* self);
void _sa_proc_do_suffix(SyntacticAnalyzer* self);
void _sa_proc_writable(SyntacticAnalyzer* self);
DataType _sa_proc_term(SyntacticAnalyzer* self);
DataType _sa_proc_simple_expr_i(SyntacticAnalyzer* self);
DataType _sa_proc_expression(SyntacticAnalyzer* self);
DataType _sa_proc_factor_a(SyntacticAnalyzer* self);
DataType _sa_proc_term_i(SyntacticAnalyzer* self);
void _sa_proc_addop(SyntacticAnalyzer* self);
DataType _sa_proc_expression_i(SyntacticAnalyzer* self);
DataType _sa_proc_factor(SyntacticAnalyzer* self);
void _sa_proc_mulop(SyntacticAnalyzer* self);
void _sa_proc_constant(SyntacticAnalyzer* self);

void syntactic_analyzer_start(SyntacticAnalyzer* self)
{
    _sa_proc_program(self);
    _sa_eat(self, TokenType_END_OF_FILE);
}

void _sa_proc_program(SyntacticAnalyzer* self)
{
    _sa_eat(self, TokenType_CLASS);
    _sa_eat(self, TokenType_ID);
    // First(decl-list)
    if (self->curToken.type == TokenType_INT ||
        self->curToken.type == TokenType_STRING ||
        self->curToken.type == TokenType_FLOAT)
    {
        _sa_proc_decl_list(self);
    }
    _sa_proc_body(self);
}

void _sa_proc_decl_list(SyntacticAnalyzer* self)
{
    _sa_proc_decl(self);
    _sa_eat(self, TokenType_SEMICOLON);
    // First(decl)
    while (self->curToken.type == TokenType_INT ||
           self->curToken.type == TokenType_STRING ||
           self->curToken.type == TokenType_FLOAT)
    {
        _sa_proc_decl(self);
        _sa_eat(self, TokenType_SEMICOLON);
    }
}

void _sa_proc_body(SyntacticAnalyzer* self)
{
    _sa_eat(self, TokenType_OPEN_CUR);
    _sa_proc_stmt_list(self);
    _sa_eat(self, TokenType_CLOSE_CUR);
}

void _sa_proc_decl(SyntacticAnalyzer* self)
{
    DataType dt = _sa_proc_type(self);
    _sa_proc_ident_list(self, dt);
}

DataType _sa_proc_type(SyntacticAnalyzer* self)
{
    DataType dt;
    if (self->curToken.type == TokenType_INT || 
        self->curToken.type == TokenType_STRING ||
        self->curToken.type == TokenType_FLOAT)
    {
        switch (self->curToken.type)
        {
        case TokenType_INT:
            dt = DataType_INT;
            break;
        case TokenType_STRING:
            dt = DataType_STRING;
            break;
        case TokenType_FLOAT:
            dt = DataType_FLOAT;
            break;
        default:
            assert("Invalid DataType value" && 0);
            break;
        }
        _sa_advance(self);
    }
    else
    {
        const char* expectedStr = "int, string or float";
        _sa_showExpectedErrorAndExit(self, expectedStr);
    }
    return dt;
}

void _sa_proc_stmt_list(SyntacticAnalyzer* self)
{
    _sa_proc_stmt(self);
    _sa_eat(self, TokenType_SEMICOLON);
    // First(stmt)
    while (self->curToken.type == TokenType_ID ||
           self->curToken.type == TokenType_IF ||
           self->curToken.type == TokenType_DO ||
           self->curToken.type == TokenType_READ ||
           self->curToken.type == TokenType_WRITE)
    {
        _sa_proc_stmt(self);
        _sa_eat(self, TokenType_SEMICOLON);
    }
}

void _sem_insertTokenInSymbolTable(SyntacticAnalyzer* self, DataType dt)
{
    if (self->curToken.type == TokenType_ID)
    {
        char* lex = self->curToken.lex;
        SymbolTableKey* stLookupKey = symbol_table_createKey(lex);
        SymbolTableEntry* curEntry = (SymbolTableEntry*) g_hash_table_lookup(self->symbolTable, stLookupKey);
        if (curEntry != NULL)
        {
            _sem_showAlreadyDeclaredIdentifierAndExit(self, lex);
        }
        else
        {
            SymbolTableEntry* entry = symbol_table_createEntry(dt);
            g_hash_table_insert(self->symbolTable, stLookupKey, entry);
        }
        _sa_advance(self); // TokenType_ID
    }
    else
    {
        const char* expectedStr = token_type_toUserString(TokenType_ID);
        _sa_showExpectedErrorAndExit(self, expectedStr);
    }
}

void _sa_proc_ident_list(SyntacticAnalyzer* self, DataType dt)
{
    _sem_insertTokenInSymbolTable(self, dt);
    while (self->curToken.type == TokenType_COLON)
    {
        _sa_advance(self); // TokenType_COLON
        _sem_insertTokenInSymbolTable(self, dt);
    }
}

void _sa_proc_stmt(SyntacticAnalyzer* self)
{
    // First(assign-stmt)
    if (self->curToken.type == TokenType_ID)
    {
        _sa_proc_assign_stmt(self);
    }
    // First(if-stmt)
    else if (self->curToken.type == TokenType_IF)
    {
        _sa_proc_if_stmt(self);
    }
    // First(do-stmt)
    else if (self->curToken.type == TokenType_DO)
    {
        _sa_proc_do_stmt(self);
    }
    // First(read-stmt)
    else if (self->curToken.type == TokenType_READ)
    {
        _sa_proc_read_stmt(self);
    }
    // First(write-stmt)
    else if (self->curToken.type == TokenType_WRITE)
    {
        _sa_proc_write_stmt(self);
    }
    else
    {
        const char* expectedStr = "identifier, if, do, read or write";
        _sa_showExpectedErrorAndExit(self, expectedStr);
    }
}

void _sa_proc_assign_stmt(SyntacticAnalyzer* self) // TODO
{
    DataType dt1;
    if (self->curToken.type == TokenType_ID)
    {
        char* lex = self->curToken.lex;
        SymbolTableKey* stLookupKey = symbol_table_createKey(lex);
        SymbolTableEntry* curEntry = (SymbolTableEntry*) g_hash_table_lookup(self->symbolTable, stLookupKey);
        if (curEntry != NULL)
        {
            dt1 = curEntry->dtype;
        }
        else
        {
            _sem_showUndeclaredIdentifierAndExit(self, lex);
        }
        _sa_advance(self); // TokenType_ID
    }
    else
    {
        const char* expectedStr = token_type_toUserString(TokenType_ID);
        _sa_showExpectedErrorAndExit(self, expectedStr);
    }

    _sa_eat(self, TokenType_ASSIGN);
    DataType dt2 = _sa_proc_simple_expr(self);

    if (dt1 != dt2)
    {
        _sem_showMismatchedDataTypesAndExit(self, dt1, dt2);
    }
}

void _sa_proc_if_stmt(SyntacticAnalyzer* self)
{
    _sa_eat(self, TokenType_IF);
    _sa_eat(self, TokenType_OPEN_PAR);
    _sa_proc_condition(self);
    _sa_eat(self, TokenType_CLOSE_PAR);
    _sa_eat(self, TokenType_OPEN_CUR);
    _sa_proc_stmt_list(self);
    _sa_eat(self, TokenType_CLOSE_CUR);
    _sa_proc_if_stmt_i(self);
}

void _sa_proc_do_stmt(SyntacticAnalyzer* self)
{
    _sa_eat(self, TokenType_DO);
    _sa_eat(self, TokenType_OPEN_CUR);
    _sa_proc_stmt_list(self);
    _sa_eat(self, TokenType_CLOSE_CUR);
    _sa_proc_do_suffix(self);
}

void _sa_proc_read_stmt(SyntacticAnalyzer* self)
{
    _sa_eat(self, TokenType_READ);
    _sa_eat(self, TokenType_OPEN_PAR);
    _sa_eat(self, TokenType_ID);
    _sa_eat(self, TokenType_CLOSE_PAR);
}

void _sa_proc_write_stmt(SyntacticAnalyzer* self)
{
    _sa_eat(self, TokenType_WRITE);
    _sa_eat(self, TokenType_OPEN_PAR);
    _sa_proc_writable(self);
    _sa_eat(self, TokenType_CLOSE_PAR);
}

DataType _sa_proc_simple_expr(SyntacticAnalyzer* self)
{
    DataType dt1 = _sa_proc_term(self);
    DataType dt2 = _sa_proc_simple_expr_i(self);

    if ((int) dt2 != -1 && dt1 != dt2) // dt2 may be -1 if lambda
    {
        _sem_showMismatchedDataTypesAndExit(self, dt1, dt2);
    }

    return dt1;
}

void _sa_proc_condition(SyntacticAnalyzer* self)
{
    _sa_proc_expression(self);
}

void _sa_proc_if_stmt_i(SyntacticAnalyzer* self)
{
    if (self->curToken.type == TokenType_ELSE)
    {
        _sa_advance(self);
        _sa_eat(self, TokenType_OPEN_CUR);
        _sa_proc_stmt_list(self);
        _sa_eat(self, TokenType_CLOSE_CUR);
    }
}

void _sa_proc_do_suffix(SyntacticAnalyzer* self)
{
    _sa_eat(self, TokenType_WHILE);
    _sa_eat(self, TokenType_OPEN_PAR);
    _sa_proc_condition(self);
    _sa_eat(self, TokenType_CLOSE_PAR);
}

void _sa_proc_writable(SyntacticAnalyzer* self)
{
    _sa_proc_simple_expr(self);
}

DataType _sa_proc_term(SyntacticAnalyzer* self)
{
    DataType dt1 = _sa_proc_factor_a(self);
    TokenType tt = self->curToken.type;
    DataType dt2 = _sa_proc_term_i(self);

    if ((int) dt2 != -1 && dt1 != dt2) // dt2 may return -1 if lambda
    {
        _sem_showMismatchedDataTypesAndExit(self, dt1, dt2);
    }
    
    if (tt == TokenType_DIV)
        return DataType_FLOAT;

    return dt1;
}

DataType _sa_proc_simple_expr_i(SyntacticAnalyzer* self)
{
    DataType dt = -1;
    // First(addop)
    if (self->curToken.type == TokenType_ADD ||
        self->curToken.type == TokenType_SUB ||
        self->curToken.type == TokenType_OR)
    {
        TokenType tt1 = self->curToken.type;
        _sa_proc_addop(self);
        DataType dt1 = _sa_proc_term(self);

        if (dt1 == DataType_STRING && tt1 != TokenType_ADD)
            _sem_showInvalidOperatorAndExit(self, dt1, tt1);

        DataType dt2 = _sa_proc_simple_expr_i(self);

        if ((int) dt2 != -1 && dt1 != dt2) // dt2 may return -1 if lamda
        {
            _sem_showMismatchedDataTypesAndExit(self, dt1, dt2);
        }
            
        dt = dt1;
    }
    return dt;
}

DataType _sa_proc_expression(SyntacticAnalyzer* self)
{
    DataType dt1 = _sa_proc_simple_expr(self);
    DataType dt2 = _sa_proc_expression_i(self);

    if ((int) dt2 != -1 && dt1 != dt2) // dt2 may return -1 if lamda
    {
        _sem_showMismatchedDataTypesAndExit(self, dt1, dt2);
    }

    if ((int) dt2 != -1) // has Relop
        return DataType_BOOLEAN;

    return dt1;
}

DataType _sa_proc_factor_a(SyntacticAnalyzer* self)
{
    DataType dt;
    if (self->curToken.type == TokenType_NOT)
    {
        _sa_advance(self);
        dt = _sa_proc_factor(self);
        if (dt != DataType_BOOLEAN)
            _sem_showInvalidOperatorAndExit(self, dt, TokenType_NOT);
    }
    else if (self->curToken.type == TokenType_SUB)
    {
        _sa_advance(self);
        dt = _sa_proc_factor(self);
        if (dt != DataType_INT &&
            dt != DataType_FLOAT)
            _sem_showInvalidOperatorAndExit(self, dt, TokenType_SUB);
    }
    else
    {
        dt = _sa_proc_factor(self);
    }
    return dt;
}

DataType _sa_proc_term_i(SyntacticAnalyzer* self)
{
    DataType dt = -1;
    // First(mulop)
    if (self->curToken.type == TokenType_MUL ||
        self->curToken.type == TokenType_DIV ||
        self->curToken.type == TokenType_AND)
    {
        TokenType tt = self->curToken.type;
        _sa_proc_mulop(self);
        DataType dt1 = _sa_proc_factor_a(self);

        if ((tt == TokenType_AND && dt1 != DataType_BOOLEAN)
            || ((tt == TokenType_MUL || tt == TokenType_DIV) && (dt1 != DataType_INT && dt1 != DataType_FLOAT)))
        {
            _sem_showInvalidOperatorAndExit(self, dt1, tt);
        }


        DataType dt2 = _sa_proc_term_i(self);

        if ((int) dt2 != -1 && dt1 != dt2) // dt2 may return -1 if lambda
            _sem_showMismatchedDataTypesAndExit(self, dt1, dt2);

        dt = dt1;
    }
    return dt;
}

void _sa_proc_addop(SyntacticAnalyzer* self)
{
    // First(addop)
    if (self->curToken.type == TokenType_ADD ||
        self->curToken.type == TokenType_SUB ||
        self->curToken.type == TokenType_OR)
    {
        _sa_advance(self);
    }
    else
    {
        const char* expectedStr = "+, - or ||";
        _sa_showExpectedErrorAndExit(self, expectedStr);
    }
}

DataType _sa_proc_expression_i(SyntacticAnalyzer* self)
{
    DataType dt = -1;
    // First(relop)
    if (self->curToken.type == TokenType_LOWER ||
        self->curToken.type == TokenType_LOWER_EQ ||
        self->curToken.type == TokenType_GREATER ||
        self->curToken.type == TokenType_GREATER_EQ ||
        self->curToken.type == TokenType_NOT_EQUALS ||
        self->curToken.type == TokenType_EQUALS)
    {
        TokenType tt = self->curToken.type;
        _sa_advance(self);
        dt = _sa_proc_simple_expr(self);
        switch (dt)
        {
        case DataType_INT:
        case DataType_FLOAT:
            // Int e float aceitam todas as comparacoes
            break;
        case DataType_STRING:
            if (tt != TokenType_NOT_EQUALS &&
                tt != TokenType_EQUALS)
            {
                _sem_showInvalidOperatorAndExit(self, dt, tt);
            }
            break;
        default:
            assert("Invalid DataType value" && 0);
            break;
        }
    }
    return dt;
}

DataType _sa_proc_factor(SyntacticAnalyzer* self)
{
    DataType dt;
    if (self->curToken.type == TokenType_ID)
    {
        char* lex = self->curToken.lex;
        SymbolTableKey* stLookupKey = symbol_table_createKey(lex);
        SymbolTableEntry* stEntry = g_hash_table_lookup(self->symbolTable, stLookupKey);
        if (stEntry == NULL)
        {
            _sem_showUndeclaredIdentifierAndExit(self, lex);
        }
        dt = stEntry->dtype;
        _sa_advance(self);
    }
    // First(constant)
    else if (self->curToken.type == TokenType_INTEGER ||
             self->curToken.type == TokenType_LITERAL ||
             self->curToken.type == TokenType_REAL)
    {
        switch (self->curToken.type)
        {
        case TokenType_INTEGER:
            dt = DataType_INT;
            break;
        case TokenType_LITERAL:
            dt = DataType_STRING;
            break;
        case TokenType_REAL:
            dt = DataType_FLOAT;
            break;
        default:
            assert("Invalid DataType value" && 0);
            break;
        }
        _sa_proc_constant(self);
    }
    else if (self->curToken.type == TokenType_OPEN_PAR)
    {
        _sa_advance(self);
        dt = _sa_proc_expression(self);
        _sa_eat(self, TokenType_CLOSE_PAR);
    }
    else
    {
        const char* expectedStr = "identifier, integer const, literal, real const or (";
        _sa_showExpectedErrorAndExit(self, expectedStr);
    }
    return dt;
}

void _sa_proc_mulop(SyntacticAnalyzer* self)
{
    // First(mulop)
    if  (self->curToken.type == TokenType_MUL ||
         self->curToken.type == TokenType_DIV ||
         self->curToken.type == TokenType_AND)
    {
        _sa_advance(self);
    }
    else
    {
        const char* expectedStr = "*, / or &&";
        _sa_showExpectedErrorAndExit(self, expectedStr);
    }
}

void _sa_proc_constant(SyntacticAnalyzer* self)
{
    if (self->curToken.type == TokenType_INTEGER ||
        self->curToken.type == TokenType_LITERAL ||
        self->curToken.type == TokenType_REAL)
    {
        _sa_advance(self);
    }
    else
    {
        const char* expectedStr = "integer const, literal or real const";
        _sa_showExpectedErrorAndExit(self, expectedStr);
    }
}