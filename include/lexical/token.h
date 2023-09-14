/*
* Caio Vinicius Pereira Silveira
* Leonardo Gonçalves Grossi
* Mariana Gurgel Ferreira
*
* Compiler for a simple programming language
*
* September 2023
*/

#ifndef TOKEN_H
#define TOKEN_H

// Forward declarations
struct SymbolTableKey;

typedef enum TokenType
{
    // Specials
    TokenType_END_OF_FILE,

    // Symbols
    TokenType_OPEN_CUR,
    TokenType_CLOSE_CUR,
    TokenType_OPEN_PAR,
    TokenType_CLOSE_PAR,
    TokenType_SEMICOLON,

    // Operators
    TokenType_LOWER,
    TokenType_LOWER_EQ,
    TokenType_GREATER,
    TokenType_GREATER_EQ,
    TokenType_ASSIGN,
    TokenType_EQUALS,
    TokenType_NOT_EQUALS,
    TokenType_NOT,
    TokenType_ADD,
    TokenType_SUB,
    TokenType_DIV,
    TokenType_MUL,
    TokenType_AND,
    TokenType_OR,

    // Keywords
    TokenType_CLASS,
    TokenType_DO,
    TokenType_WHILE,
    TokenType_READ,
    TokenType_WRITE,
    TokenType_IF,
    TokenType_ELSE,
    TokenType_INT,
    TokenType_STRING,
    TokenType_FLOAT,

    // Others
    TokenType_ID,
    TokenType_LITERAL,
    TokenType_INTEGER,
    TokenType_REAL,

    // Not to be used, only to get how many token types are
    TokenType_SIZE
} TokenType;

typedef struct Token
{
    TokenType type;
    union
    {
        long longVal;
        double doubleVal;
        const char* literal;
        const struct SymbolTableKey* st_key;
    };
} Token;

int token_type_isType(TokenType tt);
int token_type_isRelOp(TokenType tt);
int token_type_isAddOp(TokenType tt);
int token_type_isMulOp(TokenType tt);
int token_type_isConstant(TokenType tt);

const char* token_type_toString(TokenType tt);
char* token_lexemeToString(const Token* t);
void token_destroyLexemeString(char* str);

#endif // TOKEN_H
