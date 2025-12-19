#include <ctype.h>
#include <string.h>
#include "Token.h"
#include "Lexer.h"

static void lex_number(Lexer* lexer, Token* token);

static void lex_number(Lexer* lexer, Token* token) {
    int start_pos = lexer->position;
    int has_dot = 0;
    while (isdigit(lexer->current_char) || lexer->current_char == '.') {
        if (lexer->current_char == '.') {
            if (has_dot) {
                break;
            }
            has_dot = 1;
        }
        advance(lexer);
    }

    int length = lexer->position - start_pos;
    token->value = (char*)malloc(length + 1);
    memcpy(token->value, lexer->source + start_pos, length);
    token->value[length] = '\0';
    token->type = has_dot ? T_FLOAT : T_INTEGER;
}

void lexer_init(Lexer* lexer, const char* source) {
    lexer->source = source;
    lexer->position = 0;
    lexer->current_char = source[0];
    lexer->line = 1;
    lexer->column = 1;
};

void advance(Lexer* lexer) {
    if (lexer->current_char == '\n') {
        lexer->line++;
        lexer->column = 0;
    } else{
        lexer->column++;
    }
    lexer->position++;
    lexer->current_char = lexer->source[lexer->position];
};

void skip_whitespace(Lexer* lexer) {
    while (isspace(lexer->current_char)) {
        advance(lexer);
    }
};

void make_token(Lexer* lexer, Token* token) {
    skip_whitespace(lexer);

    if (lexer->current_char == '\0') {
        token->type = T_EOF;
        token->value = _strdup("EOF");
        return;
    }

    if (isdigit(lexer->current_char)) {
        lex_number(lexer, token);
        return;
    }
    char c = lexer->current_char;
    advance(lexer);

    token->value = (char*)malloc(2);
    token->value[0] = c;
    token->value[1] = '\0';

    switch (c) {
        case '+': token->type = T_ADD; break;
        case '-': token->type = T_SUBTRACT; break;
        case '*': token->type = T_MUL; break;
        case '/': token->type = T_DIVIDE; break;
        case '%': token->type = T_MODULO; break;
        case '(': token->type = T_LPAREN; break;
        case ')': token->type = T_RPAREN; break;
        default: token->type = T_UNKNOWN; break;
    }
};