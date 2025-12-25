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
    printf("DEBUG parser_advance START: Current token type=%d, value='%s'\n",
           parser->current_token->type,
           parser->current_token->value ? parser->current_token->value : "(null)");
    if (parser->current_token->value != NULL) {
        free(parser->current_token->value);
        parser->current_token->value = NULL;
    }
    make_token(parser->lexer, parser->current_token);
    printf("DEBUG parser_advance END: New token type=%d, value='%s'\n",
           parser->current_token->type,
           parser->current_token->value ? parser->current_token->value : "(null)");
}

ASTNode* parse_factor(Parser* parser) {
    printf("=== parse_factor (FIXED) ===\n");
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
    else if (token->type == T_STRING) {
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_STRING;
        node->data.string.str_val = _strdup(token->value);
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
    printf("After parse_term, current token: type=%d, token_value=%s\n", 
           parser->current_token->type, 
           parser->current_token->value ? parser->current_token->value : "NULL");
    
    // Keep combining with + or - operators
    while (1) {
        printf("Loop check: current token type=%d, token_value=%s\n", parser->current_token->type, parser->current_token->value);
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
            printf("Debug: created %d\n", new_node->type);
            new_node->data.binary_op.left = result;
            printf("Debug: created %d\n", new_node->data.binary_op.left->type);
            new_node->data.binary_op.op = T_ADD;
            printf("Debug: created op %d\n", new_node->data.binary_op.op);
            new_node->data.binary_op.right = right;
            printf("Debug: created right %d\n", new_node->data.binary_op.right->type);
            
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
            printf("Debug: created %d\n", new_node->type);
            new_node->data.binary_op.left = result;
            printf("Debug: created %d\n", new_node->data.binary_op.left->type);
            new_node->data.binary_op.op = T_SUBTRACT;
            printf("Debug: created op %d\n", new_node->data.binary_op.op);
            new_node->data.binary_op.right = right;
            printf("Debug: created right %d\n", new_node->data.binary_op.right->type);
            
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

ASTNode* parse_typed_assignment(Parser* parser, char* var_name) {
    printf("=== parse_typed_assignment (FIXED) ===\n");
    printf("Parsing typed assignment for '%s'\n", var_name);

    // We're at: var_name ":" type "=" value
    // Current token is the identifier (var_name)

    // 1.eat colon ':'
    parser_advance(parser); // skip identifier
    if  (parser->current_token->type != T_COLON) {
        printf("Error: Expected ':' after variable name\n");
        exit(EXIT_FAILURE);
    }
    parser_advance(parser); // eat skip colon

    if (parser->current_token->type != T_STRING ||
        strcmp(parser->current_token->value, "str") != 0) {
        printf("Error: Expected 'str' type\n");
        printf("Found token type=%d, value='%s'\n",
               parser->current_token->type,
               parser->current_token->value ? parser->current_token->value : "(null)");
        exit(EXIT_FAILURE);
    }
    char* type_name = _strdup("str");
    parser_advance(parser); // eat 'str'

    if (parser->current_token->type != T_ASSIGN) {
        printf("Error: Expected '=' after type declaration\n");
        exit(EXIT_FAILURE);
    }
    parser_advance(parser); // eat '='

    ASTNode* value = parse_expression(parser);

    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_TYPED_ASSIGN;
    node->data.typed_assign.var_name = var_name;
    node->data.typed_assign.type_name = type_name;
    node->data.typed_assign.value = value;

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
        char* var_name = _strdup(parser->current_token->value);
        // Use parser_peek to check next token without consuming current
        TokenType next = parser_peek(parser);
        printf("Next token type: %d\n", next);
        
        if (next == T_COLON) {
            printf("It's a typed assignment!\n");
            return parse_assignment(parser);
        }
        else if (next == T_ASSIGN) {
            printf("It's a dynamic assignment!\n");
            free(var_name);
            return parse_assignment(parser);
        }
        free(var_name);
    }
    else if (parser->current_token->type == T_GROWL) {
        printf("It's a growl statement!\n");
        return parse_growl_statement(parser);
    }
    
    printf("Parsing as expression\n");
    return parse_expression(parser);
}