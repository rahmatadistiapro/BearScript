#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "Token.h"
#include "Lexer.h"
#include "symbol_table.h"
#include "AST.h"
#include <stdlib.h>

typedef struct {
    Lexer* lexer;
    Token* current_token;
    SymbolTable* symbol_table;
} Parser;

void parser_init(Parser* parser, Lexer* lexer, SymbolTable* table);
void parser_advance(Parser* parser);
TokenType parser_peek(Parser* parser);
ASTNode* parse_factor(Parser* parser);
ASTNode* parse_term(Parser* parser);
ASTNode* parse_expression(Parser* parser);
ASTNode* parse_assignment(Parser* parser);
ASTNode* parse_typed_assignment(Parser* parser, char* var_name);
ASTNode* parse_immutable_assignment(Parser* parser, char* var_name);
ASTNode* parse_immutable_typed_dec(Parser* parser, char* var_name);
ASTNode* parse_statement(Parser* parser);
ASTNode* parse_growl_statement(Parser* parser);
ASTNode* parse_if_statement(Parser* parser);
ASTNode* parse_elif_statement(Parser* parser);
ASTNode* parse_else_statement(Parser* parser);
ASTNode* parse_comparison(Parser* parser);
ASTNode** parse_block_until_honey(Parser* parser, int* count);
ASTNode* parse_line(Parser* parser); // entry point for a single line

#endif // PARSER_H