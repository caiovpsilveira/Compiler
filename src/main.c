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
#include "syntactic/syntactic_analyzer.h"

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

    GHashTable* st = symbol_table_new();
    LexicalAnalyzer* la = lexical_analyzer_new(st, argv[1]);
    SyntacticAnalyzer* sa = syntactic_analyzer_new(st, la);

    syntactic_analyzer_start(sa);

    syntactic_analyzer_destroy(sa);
    lexical_analyzer_destroy(la);
    symbol_table_destroy(st);

    return 0;
}
