/*
* Caio Vinicius Pereira Silveira
* Leonardo Gonçalves Grossi
* Mariana Gurgel Ferreira
*
* Compiler for a simple programming language
*
* October 2023
*/

#ifndef SYNTACTIC_ANALYZER_H
#define SYNTACTIC_ANALYZER_H

#include <glib.h>

typedef struct SyntacticAnalyzer SyntacticAnalyzer;
// Forward declarations
struct LexicalAnalyzer;

SyntacticAnalyzer* syntactic_analyzer_new(GHashTable* st, struct LexicalAnalyzer* la);
void syntactic_analyzer_destroy(SyntacticAnalyzer* self);

void syntactic_analyzer_start(SyntacticAnalyzer* self);

#endif // SYNTACTIC_ANALYZER_H
