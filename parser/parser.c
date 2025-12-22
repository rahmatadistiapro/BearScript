#include "D:/BearScript/include/parser.h"
#include "D:/BearScript/include/AST.h"
#include "D:/BearScript/include/Token.h"
#include <stdio.h>
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
    if (token->type == T_INTEGER) {
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_INTEGER;
        node->data.value.int_val = atoi(token->value);
        parser_advance(parser);
        return node;
    }
    else if (token->type == T_FLOAT) {
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_FLOAT;
        node->data.value.float_val = atof(token->value);
        parser_advance(parser);
        return node;
    }
    else if (token->type == T_IDENTIFIER) {
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_VARIABLE;
        node->data.variable.var_name = _strdup(token->value);
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
    printf("\n=== parse_term (FIXED) ===\n");
    
    // Parse first factor
    ASTNode* result = parse_factor(parser);
    
    // Keep combining with *, /, or % operators
    while (1) {
        if (parser->current_token->type == T_MAUL) {
            printf("Found * operator\n");
            parser_advance(parser);
            
            ASTNode* right = parse_factor(parser);
            
            ASTNode* new_node = malloc(sizeof(ASTNode));
            new_node->type = AST_BINARY_OP;
            new_node->data.binary_op.left = result;
            new_node->data.binary_op.op = T_MAUL;
            new_node->data.binary_op.right = right;
            
            result = new_node;
        }
        else if (parser->current_token->type == T_DIVIDE) {
            printf("Found / operator\n");
            parser_advance(parser);
            
            ASTNode* right = parse_factor(parser);
            
            ASTNode* new_node = malloc(sizeof(ASTNode));
            new_node->type = AST_BINARY_OP;
            new_node->data.binary_op.left = result;
            new_node->data.binary_op.op = T_DIVIDE;
            new_node->data.binary_op.right = right;
            
            result = new_node;
        }
        else if (parser->current_token->type == T_MODULO) {
            printf("Found %% operator\n");
            parser_advance(parser);
            
            ASTNode* right = parse_factor(parser);
            
            ASTNode* new_node = malloc(sizeof(ASTNode));
            new_node->type = AST_BINARY_OP;
            new_node->data.binary_op.left = result;
            new_node->data.binary_op.op = T_MODULO;
            new_node->data.binary_op.right = right;
            
            result = new_node;
        }
        else {
            // No more *, /, or % operators
            break;
        }
    }
    
    printf("Returning term node type: %d\n", result->type);
    return result;
}

ASTNode* parse_expression(Parser* parser) {
    printf("\n=== parse_expression (FIXED) ===\n");
    
    // Parse first term
    ASTNode* result = parse_term(parser);
    
    // Keep combining with + or - operators
    while (1) {
        // Check what operator we have (if any)
        if (parser->current_token->type == T_ADD) {
            printf("Found + operator\n");
            
            // Eat the '+'
            parser_advance(parser);
            
            // Parse the right side
            ASTNode* right = parse_term(parser);
            
            // Create new binary node: (left + right)
            ASTNode* new_node = malloc(sizeof(ASTNode));
            new_node->type = AST_BINARY_OP;
            new_node->data.binary_op.left = result;
            new_node->data.binary_op.op = T_ADD;
            new_node->data.binary_op.right = right;
            
            // This becomes our new result
            result = new_node;
            
            printf("Created + operation\n");
        }
        else if (parser->current_token->type == T_SUBTRACT) {
            printf("Found - operator\n");
            
            // Eat the '-'
            parser_advance(parser);
            
            // Parse the right side
            ASTNode* right = parse_term(parser);
            
            // Create new binary node: (left - right)
            ASTNode* new_node = malloc(sizeof(ASTNode));
            new_node->type = AST_BINARY_OP;
            new_node->data.binary_op.left = result;
            new_node->data.binary_op.op = T_SUBTRACT;
            new_node->data.binary_op.right = right;
            
            // This becomes our new result
            result = new_node;
            
            printf("Created - operation\n");
        }
        else {
            // No more + or - operators
            printf("No more + or - operators. Done.\n");
            break;
        }
    }
    
    printf("Returning expression node type: %d\n", result->type);
    return result;
}

ASTNode* parse_growl_statement(Parser* parser) {
    printf("Parsing growl statement\n");
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_GROWL_STATEMENT;
    parser_advance(parser); // eat 'GROWL' token
    node->data.growl_stmt.expression = parse_expression(parser);
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
    node->data.assign.var_name = var_name;
    node->data.assign.value = value;

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
    printf("\n=== PARSE_LINE ===\n");
    printf("Token: '%s' (type: %d)\n",
           parser->current_token->value, parser->current_token->type);
    
    // Check if it's an assignment: identifier followed by '='
    if (parser->current_token->type == T_IDENTIFIER) {
        // Use parser_peek to check next token without consuming current
        TokenType next = parser_peek(parser);
        printf("Next token type: %d\n", next);
        
        if (next == T_ASSIGN) {
            printf("It's an assignment!\n");
            return parse_assignment(parser);
        }
    }
    else if (parser->current_token->type == T_GROWL) {
        printf("It's a growl statement!\n");
        return parse_growl_statement(parser);
    }
    
    printf("Parsing as expression\n");
    return parse_expression(parser);
}