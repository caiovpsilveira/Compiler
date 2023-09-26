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

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: \"%s source_filepath\".\n", argv[0]);
        exit(-1);
    }

    int lexical_print = 0;

    for (int i = 2; i < argc; ++i)
    {
        if (strcmp(argv[i], "-set") == 0)
        {
            ++i;
            if (i == argc)
            {
                fprintf(stderr, "Missing argument after \"-set\".\n");
                exit(-1);
            }
            else if (strcmp(argv[i], "lexical_print") == 0)
            {
                lexical_print = 1;
            }
            else
            {
                fprintf(stderr, "Invalid arguemnt after \"-set\": \"%s\".\n", argv[i]);
                exit (-1);
            }
        }
        else
        {
            fprintf(stderr, "Invalid option \"%s\".\n", argv[i]);
            exit(-1);
        }
    }

    GHashTable* st = symbol_table_new();
    LexicalAnalyzer* la = lexical_analyzer_new(st, argv[1]);

    Token t = lexical_analyzer_getToken(la);
    while (t.type != TokenType_END_OF_FILE)
    {
        if (lexical_print)
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
        }
        t = lexical_analyzer_getToken(la);
    }
    if (lexical_print)
        symbol_table_print(st);

    lexical_analyzer_destroy(la);
    symbol_table_destroy(st);

    return 0;
}
