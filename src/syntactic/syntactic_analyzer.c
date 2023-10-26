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
void _sa_proc_type(SyntacticAnalyzer* self);
void _sa_proc_stmt_list(SyntacticAnalyzer* self);
void _sa_proc_ident_list(SyntacticAnalyzer* self);
void _sa_proc_stmt(SyntacticAnalyzer* self);
void _sa_proc_assign_stmt(SyntacticAnalyzer* self);
void _sa_proc_if_stmt(SyntacticAnalyzer* self);
void _sa_proc_do_stmt(SyntacticAnalyzer* self);
void _sa_proc_read_stmt(SyntacticAnalyzer* self);
void _sa_proc_write_stmt(SyntacticAnalyzer* self);
void _sa_proc_simple_expr(SyntacticAnalyzer* self);
void _sa_proc_condition(SyntacticAnalyzer* self);
void _sa_proc_if_stmt_i(SyntacticAnalyzer* self);
void _sa_proc_do_suffix(SyntacticAnalyzer* self);
void _sa_proc_writable(SyntacticAnalyzer* self);
void _sa_proc_term(SyntacticAnalyzer* self);
void _sa_proc_simple_expr_i(SyntacticAnalyzer* self);
void _sa_proc_expression(SyntacticAnalyzer* self);
void _sa_proc_factor_a(SyntacticAnalyzer* self);
void _sa_proc_term_i(SyntacticAnalyzer* self);
void _sa_proc_addop(SyntacticAnalyzer* self);
void _sa_proc_expression_i(SyntacticAnalyzer* self);
void _sa_proc_factor(SyntacticAnalyzer* self);
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
    _sa_proc_type(self);
    _sa_proc_ident_list(self);
}

void _sa_proc_type(SyntacticAnalyzer* self)
{
    if (self->curToken.type == TokenType_INT || 
        self->curToken.type == TokenType_STRING ||
        self->curToken.type == TokenType_FLOAT)
    {
        _sa_advance(self);
    }
    else
    {
        const char* expectedStr = "int, string or float";
        _sa_showExpectedErrorAndExit(self, expectedStr);
    }
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

void _sa_proc_ident_list(SyntacticAnalyzer* self)
{
    _sa_eat(self, TokenType_ID);
    while (self->curToken.type == TokenType_COLON)
    {
        _sa_advance(self);
        _sa_eat(self, TokenType_ID);
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

void _sa_proc_assign_stmt(SyntacticAnalyzer* self)
{
    _sa_eat(self, TokenType_ID);
    _sa_eat(self, TokenType_ASSIGN);
    _sa_proc_simple_expr(self);
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

void _sa_proc_simple_expr(SyntacticAnalyzer* self)
{
    _sa_proc_term(self);
    _sa_proc_simple_expr_i(self);
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

void _sa_proc_term(SyntacticAnalyzer* self)
{
    _sa_proc_factor_a(self);
    _sa_proc_term_i(self);
}

void _sa_proc_simple_expr_i(SyntacticAnalyzer* self)
{
    // First(addop)
    if (self->curToken.type == TokenType_ADD ||
        self->curToken.type == TokenType_SUB ||
        self->curToken.type == TokenType_OR)
    {
        _sa_proc_addop(self);
        _sa_proc_term(self);
        _sa_proc_simple_expr_i(self);
    }
}

void _sa_proc_expression(SyntacticAnalyzer* self)
{
    _sa_proc_simple_expr(self);
    _sa_proc_expression_i(self);
}

void _sa_proc_factor_a(SyntacticAnalyzer* self)
{
    if (self->curToken.type == TokenType_NOT)
    {
        _sa_advance(self);
        _sa_proc_factor(self);
    }
    else if (self->curToken.type == TokenType_SUB)
    {
        _sa_advance(self);
        _sa_proc_factor(self);
    }
    else
    {
        _sa_proc_factor(self);
    }
}

void _sa_proc_term_i(SyntacticAnalyzer* self)
{
    // First(mulop)
    if (self->curToken.type == TokenType_MUL ||
        self->curToken.type == TokenType_DIV ||
        self->curToken.type == TokenType_AND)
    {
        _sa_proc_mulop(self);
        _sa_proc_factor_a(self);
        _sa_proc_term_i(self);
    }
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

void _sa_proc_expression_i(SyntacticAnalyzer* self)
{
    // First(relop)
    if (self->curToken.type == TokenType_LOWER ||
        self->curToken.type == TokenType_LOWER_EQ ||
        self->curToken.type == TokenType_GREATER ||
        self->curToken.type == TokenType_GREATER_EQ ||
        self->curToken.type == TokenType_NOT_EQUALS ||
        self->curToken.type == TokenType_EQUALS)
    {
        _sa_advance(self);
        _sa_proc_simple_expr(self);
    }
}

void _sa_proc_factor(SyntacticAnalyzer* self)
{
    if (self->curToken.type == TokenType_ID)
    {
        _sa_advance(self);
    }
    // First(constant)
    else if (self->curToken.type == TokenType_INTEGER ||
             self->curToken.type == TokenType_LITERAL ||
             self->curToken.type == TokenType_REAL)
    {
        _sa_proc_constant(self);
    }
    else if (self->curToken.type == TokenType_OPEN_PAR)
    {
        _sa_advance(self);
        _sa_proc_expression(self);
        _sa_eat(self, TokenType_CLOSE_PAR);
    }
    else
    {
        const char* expectedStr = "identifier, integer const, literal, real const or (";
        _sa_showExpectedErrorAndExit(self, expectedStr);
    }
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