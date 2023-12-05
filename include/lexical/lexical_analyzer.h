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

LexicalAnalyzer* lexical_analyzer_new(char* filepath);
void lexical_analyzer_destroy(LexicalAnalyzer* self);

unsigned lexical_analyzer_getLine(const LexicalAnalyzer* self);
unsigned lexical_analyzer_getColumn(const LexicalAnalyzer* self);

Token lexical_analyzer_getToken(LexicalAnalyzer* self);

#endif // LEXICAL_ANALYZER_H
