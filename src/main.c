/*
* Caio Vinicius Pereira Silveira
* Leonardo Gonçalves Grossi
* Mariana Gurgel Ferreira
*
* Compiler for a simple programming language
*
* September 2023
*/

#include "lexical/lexical_analyzer.h"
#include "lexical/token.h"
#include "symbol_table/symbol_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: \"%s source_filepath\".\n", argv[0]);
        exit(-1);
    }

    GHashTable* st = symbol_table_new();
    LexicalAnalyzer* la = lexical_analyzer_new(st, argv[1]);

    Token t = lexical_analyzer_getToken(la);
    while (t.type != TokenType_END_OF_FILE)
    {
        const char* tokenTypeStr = token_type_toString(t.type);
        char* tokenLexemeStr = token_lexemeToString(&t);
        if (!tokenLexemeStr)
        {
            printf("%s\n", tokenTypeStr);
        }
        else
        {
            printf("%s, lex: %s\n", tokenTypeStr, tokenLexemeStr);
            token_destroyLexemeString(tokenLexemeStr);
        }
        t = lexical_analyzer_getToken(la);
    }
    symbol_table_print(st);

    lexical_analyzer_destroy(la);
    symbol_table_destroy(st);

    return 0;
}
