#ifndef LEXER_H
#define LEXER_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "Token.h"
#include <sys/stat.h>

typedef struct Lexer {
    const char* source;
    int position;
    char current_char;
    int line;
    int column;
} Lexer;

// Function to initialize a Lexer
void lexer_init(Lexer* lexer, const char* source);
static void lex_string(Lexer* lexer, Token* token);
static void lex_number(Lexer* lexer, Token* token);
static void lex_identifier(Lexer* lexer, Token* token);
static void lex_controlflow(Lexer* lexer, Token* token);
static char peek(Lexer* lexer);
void make_token(Lexer* lexer, Token* token);
void advance(Lexer* lexer);
void skip_whitespace(Lexer* lexer);

#endif // LEXER_H