#include "parser.h"
#include "AST.h"
#include "Token.h"
#include <stdlib.h>
#include <string.h>

void parser_init(Parser* parser, Lexer* lexer, SymbolTable* table) {
    parser->lexer = lexer;
    parser->symbol_table = table;
    parser->current_token = (Token*)malloc(sizeof(Token));
    parser->current_token->type = 0;
    parser->current_token->value = NULL;
    parser_advance(parser);
    return;
}

void parser_advance(Parser *parser) {
    if (parser->current_token->value != NULL) {
        free(parser->current_token->value);
    }
    make_token(parser->lexer, parser->current_token);
}

ASTNode* parse_factor(Parser* parser) {
    Token* token = parser->current_token;
    if (token->type == T_INTEGER ||
        token->type == T_FLOAT) {
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = (token->type == T_INTEGER) ? AST_INTEGER : AST_FLOAT;
        node->number = atof(token->value);
        parser_advance(parser);
        return node;
    }
    else if (token->type == T_IDENTIFIER) {
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_VARIABLE;
        node->var_name = _strdup(token->value);
        parser_advance(parser);
        return node;
    }
    else if (token->type == T_LPAREN) {
        parser_advance(parser);
        ASTNode* node = parse_expression(parser);
        if (parser->current_token->type == T_RPAREN) {
            parser_advance(parser);
            return node;
        } else {
            printf("Error: Expected ')'\n");
            exit(EXIT_FAILURE);
        }
    }
    else {
        printf("Error: Unexpected token '%s'\n", token->value);
        exit(EXIT_FAILURE);
    }
}

ASTNode* parse_term(Parser* parser) {
    ASTNode* node = parse_factor(parser);
    while (parser->current_token->type == T_MUL ||
           parser->current_token->type == T_DIVIDE ||
           parser->current_token->type == T_MODULO) {
            Token* token = parser->current_token;
            parser_advance(parser);
            if (token->type == T_MUL) {
                ASTNode* new_node = (ASTNode*)malloc(sizeof(ASTNode));
                new_node->type = AST_BINARY_OP;
                new_node->binary_op.left = node;
                new_node->binary_op.op = T_MUL;
                new_node->binary_op.right = parse_factor(parser);
                node = new_node;
            }
            else if (token->type == T_DIVIDE) {
                ASTNode* new_node = (ASTNode*)malloc(sizeof(ASTNode));
                new_node->type = AST_BINARY_OP;
                new_node->binary_op.left = node;
                new_node->binary_op.op = T_DIVIDE;
                new_node->binary_op.right = parse_factor(parser);
                node = new_node;
            }
            else if (token->type == T_MODULO) {
                ASTNode* new_node = (ASTNode*)malloc(sizeof(ASTNode));
                new_node->type = AST_BINARY_OP;
                new_node->binary_op.left = node;
                new_node->binary_op.op = T_MODULO;
                new_node->binary_op.right = parse_factor(parser);
                node = new_node;
            }
    }
    return node;
}

ASTNode* parse_expression(Parser* parser) {
    ASTNode* node = parse_term(parser);
    while (parser->current_token->type == T_ADD ||
           parser->current_token->type == T_SUBTRACT) {
            Token* token = parser->current_token;
            parser_advance(parser);
            if (token->type == T_ADD) {
                ASTNode* new_node = (ASTNode*)malloc(sizeof(ASTNode));
                new_node->type = AST_BINARY_OP;
                new_node->binary_op.left = node;
                new_node->binary_op.op = T_ADD;
                new_node->binary_op.right = parse_term(parser);
                node = new_node;
            }
            else if (token->type == T_SUBTRACT) {
                ASTNode* new_node = (ASTNode*)malloc(sizeof(ASTNode));
                new_node->type = AST_BINARY_OP;
                new_node->binary_op.left = node;
                new_node->binary_op.op = T_SUBTRACT;
                new_node->binary_op.right = parse_term(parser);
                node = new_node;
            }
    }
    return node;
}

ASTNode* parse_assignment(Parser* parser) {
    // IDENT already confirmed
    char* var_name = _strdup(parser->current_token->value);
    parser_advance(parser); // eat IDENT

    parser_advance(parser); // eat '='

    ASTNode* value = parse_expression(parser);

    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_ASSIGN;
    node->assign.var_name = var_name;
    node->assign.value = value;

    return node;
}

TokenType parser_peek(Parser* parser) {
    // save lexer state
    int pos = parser->lexer->position;
    char ch = parser->lexer->current_char;
    int line = parser->lexer->line;
    int col = parser->lexer->column;

    Token tmp;
    tmp.value = NULL;
    make_token(parser->lexer, &tmp);

    TokenType type = tmp.type;
    if (tmp.value) free(tmp.value);

    // restore lexer state
    parser->lexer->position = pos;
    parser->lexer->current_char = ch;
    parser->lexer->line = line;
    parser->lexer->column = col;

    return type;
}


ASTNode* parse_line(Parser* parser) {
    if (parser->current_token->type == T_IDENTIFIER) {
        char* name = _strdup(parser->current_token->value);
        parser_advance(parser);

        if (parser->current_token->type == T_ASSIGN) {
            // assignment
            parser_advance(parser);
            ASTNode* value = parse_expression(parser);

            ASTNode* node = malloc(sizeof(ASTNode));
            node->type = AST_ASSIGN;
            node->assign.var_name = name;
            node->assign.value = value;
            return node;
        }

        // Not assignment → treat as expression starting with variable
        ASTNode* left = malloc(sizeof(ASTNode));
        left->type = AST_VARIABLE;
        left->var_name = name;

        return parse_expression(parser); // expression continues
    }

    return parse_expression(parser);
}
