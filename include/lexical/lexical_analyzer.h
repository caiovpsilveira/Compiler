/*
* Caio Vinicius Pereira Silveira
* Leonardo Gonçalves Grossi
* Mariana Gurgel Ferreira
*
* Compiler for a simple programming language
*
* September 2023
*/

#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include "token.h"

#include <glib.h>

typedef struct LexicalAnalyzer LexicalAnalyzer;

LexicalAnalyzer* lexical_analyzer_new(GHashTable* st, char* filepath);
void lexical_analyzer_destroy(LexicalAnalyzer* const self);

Token lexical_analyzer_getToken(LexicalAnalyzer* self);

#endif // LEXICAL_ANALYZER_H
