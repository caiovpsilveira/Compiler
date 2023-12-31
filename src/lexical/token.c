/*
* Caio Vinicius Pereira Silveira
* Leonardo Gonçalves Grossi
* Mariana Gurgel Ferreira
*
* Compiler for a simple programming language
*
* September 2023
*/

#include "lexical/token.h"
#include "symbol_table/symbol_table.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_LEXEME_MAX_CONVERSION_SIZE 50

const char* token_type_toString(TokenType tt)
{
    const char *str;
    switch (tt)
    {
    case TokenType_END_OF_FILE:
        str = "TokenType::END_OF_FILE";
        break;
    case TokenType_OPEN_CUR:
        str = "TokenType::OPEN_CUR";
        break;
    case TokenType_CLOSE_CUR:
        str = "TokenType::CLOSE_CUR";
        break;
    case TokenType_OPEN_PAR:
        str = "TokenType::OPEN_PAR";
        break;
    case TokenType_CLOSE_PAR:
        str = "TokenType::CLOSE_PAR";
        break;
    case TokenType_SEMICOLON:
        str = "TokenType::SEMICOLON";
        break;
    case TokenType_COLON:
        str = "TokenType::COLON";
        break;
    case TokenType_LOWER:
        str = "TokenType::LOWER";
        break;
    case TokenType_LOWER_EQ:
        str = "TokenType::LOWER_EQ";
        break;
    case TokenType_GREATER:
        str = "TokenType::GREATER";
        break;
    case TokenType_GREATER_EQ:
        str = "TokenType::GREATER_EQ";
        break;
    case TokenType_ASSIGN:
        str = "TokenType::ASSIGN";
        break;
    case TokenType_EQUALS:
        str = "TokenType::EQUALS";
        break;
    case TokenType_NOT_EQUALS:
        str = "TokenType::NOT_EQUALS";
        break;
    case TokenType_NOT:
        str = "TokenType::NOT";
        break;
    case TokenType_ADD:
        str = "TokenType::ADD";
        break;
    case TokenType_SUB:
        str = "TokenType::SUB";
        break;
    case TokenType_DIV:
        str = "TokenType::DIV";
        break;
    case TokenType_MUL:
        str = "TokenType::MUL";
        break;
    case TokenType_AND:
        str = "TokenType::AND";
        break;
    case TokenType_OR:
        str = "TokenType::OR";
        break;
    case TokenType_CLASS:
        str = "TokenType::CLASS";
        break;
    case TokenType_DO:
        str = "TokenType::DO";
        break;
    case TokenType_WHILE:
        str = "TokenType::WHILE";
        break;
    case TokenType_READ:
        str = "TokenType::READ";
        break;
    case TokenType_WRITE:
        str = "TokenType::WRITE";
        break;
    case TokenType_IF:
        str = "TokenType::IF";
        break;
    case TokenType_ELSE:
        str = "TokenType::ELSE";
        break;
    case TokenType_INT:
        str = "TokenType::INT";
        break;
    case TokenType_STRING:
        str = "TokenType::STRING";
        break;
    case TokenType_FLOAT:
        str = "TokenType::FLOAT";
        break;
    case TokenType_ID:
        str = "TokenType::ID";
        break;
    case TokenType_LITERAL:
        str = "TokenType::LITERAL";
        break;
    case TokenType_INTEGER:
        str = "TokenType::INTEGER";
        break;
    case TokenType_REAL:
        str = "TokenType::REAL";
        break;
    case TokenType_SIZE:
    default:
        assert("Invalid TokenType value" && 0);
        break;
    }
    return str;
}

const char* token_type_toUserString(TokenType tt)
{
    const char* str;
    switch (tt) {
    case TokenType_END_OF_FILE:
        str = "End of File";
        break;
    case TokenType_OPEN_CUR:
        str = "{";
        break;
    case TokenType_CLOSE_CUR:
        str = "}";
        break;
    case TokenType_OPEN_PAR:
        str = "(";
        break;
    case TokenType_CLOSE_PAR:
        str = ")";
        break;
    case TokenType_SEMICOLON:
        str = ";";
        break;
    case TokenType_COLON:
        str = ",";
        break;
    case TokenType_LOWER:
        str = "<";
        break;
    case TokenType_LOWER_EQ:
        str = "<=";
        break;
    case TokenType_GREATER:
        str = ">";
        break;
    case TokenType_GREATER_EQ:
        str = ">=";
        break;
    case TokenType_ASSIGN:
        str = "=";
        break;
    case TokenType_EQUALS:
        str = "==";
        break;
    case TokenType_NOT_EQUALS:
        str = "!=";
        break;
    case TokenType_NOT:
        str = "!";
        break;
    case TokenType_ADD:
        str = "+";
        break;
    case TokenType_SUB:
        str = "-";
        break;
    case TokenType_DIV:
        str = "/";
        break;
    case TokenType_MUL:
        str = "*";
        break;
    case TokenType_AND:
        str = "&&";
        break;
    case TokenType_OR:
        str = "||";
        break;
    case TokenType_CLASS:
        str = "class";
        break;
    case TokenType_DO:
        str = "do";
        break;
    case TokenType_WHILE:
        str = "while";
        break;
    case TokenType_READ:
        str = "read";
        break;
    case TokenType_WRITE:
        str = "write";
        break;
    case TokenType_IF:
        str = "if";
        break;
    case TokenType_ELSE:
        str = "else";
        break;
    case TokenType_INT:
        str = "int";
        break;
    case TokenType_STRING:
        str = "string";
        break;
    case TokenType_FLOAT:
        str = "float";
        break;
    case TokenType_ID:
        str = "identifier";
        break;
    case TokenType_LITERAL:
        str = "literal";
        break;
    case TokenType_INTEGER:
        str = "integer const";
        break;
    case TokenType_REAL:
        str = "real const";
        break;
    case TokenType_SIZE:
    default:
        assert("Invalid TokenType value" && 0);
        break;
    }
    return str;
}

char* token_lexemeToString(const Token* t)
{   
    char* str = NULL;

    // create copy so the token will not be modified
    if (t->type == TokenType_ID)
    {
        str = (char*) malloc((strlen(t->lex) + 1) * sizeof(char));
        strcpy(str, t->lex);
    }
    else if (t->type == TokenType_LITERAL)
    {
        str = (char*) malloc((strlen(t->literal) + 1) * sizeof(char));
        strcpy(str, t->literal);
    }
    else if (t->type == TokenType_INTEGER)
    {
        str = (char*) malloc((TOKEN_LEXEME_MAX_CONVERSION_SIZE + 1) * sizeof(char));
        snprintf(str, TOKEN_LEXEME_MAX_CONVERSION_SIZE, "%ld", t->longVal);
        str = (char*) realloc(str, (strlen(str) + 1) * sizeof(char)); // shrink to fit
    }
    else if (t->type == TokenType_REAL)
    {
        str = (char*) malloc((TOKEN_LEXEME_MAX_CONVERSION_SIZE + 1) * sizeof(char));
        snprintf(str, TOKEN_LEXEME_MAX_CONVERSION_SIZE, "%lf", t->doubleVal);
        str = (char*) realloc(str, (strlen(str) + 1) * sizeof(char)); // shrink to fit
    }

    return str;
}

void token_destroyLexemeString(char* str)
{
    assert(str);
    free(str);
}
