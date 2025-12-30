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

ASTNode* parse_comparison(Parser* parser) {
    printf("=== parse_comparison (FIXED) ===\n");
    ASTNode* left = parse_expression(parser);
    
    TokenType type = parser->current_token->type;
    if (type == T_EQUAL || type == T_NOTEQ ||
        type == T_LT || type == T_LTOREQ ||
        type == T_GT || type == T_GTOREQ) {
        printf("Found comparison operator '%s'\n", parser->current_token->value);
        
        TokenType op = parser->current_token->type;
        parser_advance(parser); // eat operator
        
        ASTNode* right = parse_expression(parser);
        
        ASTNode* node = malloc(sizeof(ASTNode));
        node->type = AST_COMPARE_OP;
        node->data.compare_op.left = left;
        node->data.compare_op.op = op;
        node->data.compare_op.right = right;
        
        return node;
    }
    
    return left; // No comparison, just return the expression
}

ASTNode* parse_statement(Parser* parser) {
    printf("=== parse_statement (FIXED) ===\n");
    printf("current_token: type=%d, value='%s'\n",
           parser->current_token->type,
           parser->current_token->value ? parser->current_token->value : "(null)");
    Token* token = parser->current_token;
    printf("Token type: %d\n", token->type);

    if (token->type == T_GROWL) {
        printf("Token Type: '%d', Token Value: '%s' (GROWL)", token->type, token->value);
        return parse_growl_statement(parser);
    }
    else if (token->type == T_IF) {
        printf("Token Type: '%d', Token Value: '%s' (IF)", token->type, token->value);
        return parse_if_statement(parser);
    }
    else if (token->type == T_IDENTIFIER) {
        printf("Token Type: '%d', Token Value: '%s' (IDENTIFIER)", token->type, token->value);
        char* var_name = _strdup(token->value);
        parser_advance(parser); // eat IDENT

        if (parser->current_token->type == T_COLON) {
            printf("Detected typed assignment for variable '%s'\n", var_name);
            return parse_typed_assignment(parser, var_name);
        }
        else if (parser->current_token->type == T_ASSIGN) {
            printf("Detected assignment for variable '%s'\n", var_name);
            return parse_assignment(parser);
        }
        else {
            printf("Error: Unexpected token after identifier '%s'\n", var_name);
            exit(EXIT_FAILURE);
        }
    }
    else {
        printf("Error: Unexpected token '%s' in statement\n", token->value);
        exit(EXIT_FAILURE);
    }
}

ASTNode* parse_growl_statement(Parser* parser) {
    printf("Parsing growl statement\n");
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_GROWL_STATEMENT;
    parser_advance(parser); // eat 'GROWL' token
    node->data.growl_stmt.expression = parse_comparison(parser);
    return node;
}

ASTNode** parse_block_until_honey(Parser* parser, int* count) {
    printf("=== parse_block_until_honey ===\n");
    ASTNode** statements = NULL;
    int capacity = 4;
    *count = 0;

    statements = malloc(sizeof(ASTNode*) * capacity);
    printf("Initialized statements array with capacity %d\n", capacity);

    while (parser->current_token->type != T_HONEY &&
           parser->current_token->type != T_EOF) {
        
        printf("DEBUG: Parsing statement (token type=%d, value='%s')\n",
               parser->current_token->type,
               parser->current_token->value ? parser->current_token->value : "(null)");
        
        // ✅ FIX: parse_statement returns ASTNode*, not ASTNode**
        ASTNode* stmt = parse_statement(parser);
        
        printf("Parsed statement type=%d, current token type=%d, value='%s'\n",
               stmt ? stmt->type : -1,
               parser->current_token->type,
               parser->current_token->value ? parser->current_token->value : "(null)");
        
        if (stmt) {
            // Check if we need to grow the array
            if (*count >= capacity) {
                capacity *= 2;
                statements = realloc(statements, sizeof(ASTNode*) * capacity);
                printf("DEBUG: Grew array to capacity %d\n", capacity);
            }
            
            printf("DEBUG: Adding statement type %d to block (index %d)\n", 
                   stmt->type, *count);
            statements[(*count)++] = stmt;
        }
    }
    
    printf("Finished parsing block, total statements: %d\n", *count);
    
    if (parser->current_token->type != T_HONEY) {
        printf("Error: Expected 'honey' to end block\n");
        exit(EXIT_FAILURE);
    }
    
    parser_advance(parser); // eat 'honey'
    return statements;
}

ASTNode* parse_if_statement(Parser* parser) {
    parser_advance(parser); // eat 'if'
    ASTNode* condition = parse_comparison(parser);

    int then_count;
    ASTNode** then_statements = parse_block_until_honey(parser, &then_count);

    ASTNode* elif_branch = NULL;
    ASTNode* else_branch = NULL;

    if (parser->current_token->type == T_ELIF) {
        elif_branch = parse_elif_statement(parser);
    }
    else if (parser->current_token->type == T_ELSE) {
        else_branch = parse_else_statement(parser);
    }

    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_IF_STATEMENT;

    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_statements = then_statements;

    node->data.if_stmt.then_count = then_count;
    node->data.if_stmt.elif_branch = elif_branch;

    node->data.if_stmt.else_branch = else_branch;
    return node;
}

ASTNode* parse_elif_statement(Parser* parser) {
    parser_advance(parser); // eat 'elif'
    ASTNode* condition = parse_comparison(parser);

    int then_count;
    ASTNode** then_statements = parse_block_until_honey(parser, &then_count);

    ASTNode* next_elif = NULL;
    ASTNode* else_branch = NULL;

    if (parser->current_token->type == T_ELIF) {
        next_elif = parse_elif_statement(parser);
    }
    else if (parser->current_token->type == T_ELSE) {
        else_branch = parse_else_statement(parser);
    }

    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_ELIF_STATEMENT;

    node->data.elif_stmt.condition = condition;
    node->data.elif_stmt.then_statements = then_statements;

    node->data.elif_stmt.count = then_count;
    node->data.elif_stmt.next_elif = next_elif;
    node->data.elif_stmt.else_branch = else_branch;
    return node;
}

ASTNode* parse_else_statement(Parser* parser) {
    parser_advance(parser); // eat 'else'

    int count;
    ASTNode** statements = parse_block_until_honey(parser, &count);

    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_ELSE_STATEMENT;

    node->data.else_stmt.then_statements = *statements;
    node->data.else_stmt.count = count;
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

    if ((parser->current_token->type != T_STRING ||
        strcmp(parser->current_token->value, "str") != 0) &&
        (parser->current_token->type != T_INTEGER ||
        strcmp(parser->current_token->value, "int") != 0) &&
        (parser->current_token->type != T_FLOAT ||
        strcmp(parser->current_token->value, "float") != 0)) {
        printf("Error: Expected 'str', 'int', or 'float' type after colon\n");
        printf("Found token type=%d, value='%s'\n",
               parser->current_token->type,
               parser->current_token->value ? parser->current_token->value : "(null)");
        exit(EXIT_FAILURE);
    }

    bool done = false;
    if (parser->current_token->type == T_STRING) {
        printf("Confirmed type token: '%s'\n", parser->current_token->value);
        char* type_name = _strdup("str");
        printf("Before eating type, current token type=%d, value='%s",
               parser->current_token->type,
               parser->current_token->value ? parser->current_token->value : "(null)");
        parser_advance(parser); // eat 'str'
        printf("After eating type, current token type=%d, value='%s'\n",
               parser->current_token->type,
               parser->current_token->value ? parser->current_token->value : "(null)");

        if (parser->current_token->type != T_ASSIGN) {
            printf("Error: Expected '=' after type declaration test\n");
            exit(EXIT_FAILURE);
        }
        parser_advance(parser); // eat '='

        ASTNode* value = parse_expression(parser);

        ASTNode* node = malloc(sizeof(ASTNode));
        node->type = AST_TYPED_ASSIGN;
        node->data.typed_assign.var_name = var_name;
        node->data.typed_assign.type_name = type_name;
        node->data.typed_assign.value = value;

        done = true;
        return node;
    }
    else if (parser->current_token->type == T_INTEGER && !done) {
        printf("Confirmed type token: '%s'\n", parser->current_token->value);
        char* type_name = _strdup("int");
        parser_advance(parser); // eat 'int'

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
    else if (parser->current_token->type == T_FLOAT && !done) {
        printf("Confirmed type token: '%s'\n", parser->current_token->value);
        char* type_name = _strdup("float");
        parser_advance(parser); // eat 'float'

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
    else {
        printf("Error: Unknown type in typed assignment\n");
        exit(EXIT_FAILURE);
    }
}

ASTNode* parse_immutable_assignment(Parser* parser, char* var_name) {
    // Implementation for immutable assignment parsing
    // This is a placeholder; actual implementation would be similar to parse_typed_assignment
    printf("=== parse_immutable_assignment (FIXED) ===\n");
    printf("Parsing immutable assignment for '%s'\n", var_name);
    printf("Before eating identifier, current token: type=%d, value=%s\n",
           parser->current_token->type,
           parser->current_token->value ? parser->current_token->value : "(null)");
    parser_advance(parser); // eat IDENT
    printf("After eating identifier and before eating '=', current token: type=%d, value=%s\n",
           parser->current_token->type,
           parser->current_token->value ? parser->current_token->value : "(null)");

    if (parser->current_token->type == T_COLON) {
        printf("it's a typed immutable assignment!\n");
        return parse_immutable_typed_dec(parser, var_name);
    }

    if (parser->current_token->type != T_ASSIGN) {
        printf("Error: Expected '=' after identifier in immutable assignment\n");
        printf("Found token type=%d, value='%s'\n",
               parser->current_token->type,
               parser->current_token->value ? parser->current_token->value : "(null)");
        exit(EXIT_FAILURE);
    }
    parser_advance(parser); // eat '='

    ASTNode* value = parse_expression(parser);

    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_IMMUTABLE_ASSIGN;
    node->data.immutable_assign.var_name = var_name;
    node->data.immutable_assign.value = value;
    return node;
}

ASTNode* parse_immutable_typed_dec(Parser* parser, char* var_name) {
    printf("=== parse_immutable_type_assignment (FIXED) ===\n");
    printf("Parsing immutable typed assignment for '%s'\n", var_name);
    parser_advance(parser); // eat ':'

    bool done = false;
    if (parser->current_token->type == T_INTEGER) {
        printf("Confirmed type token: '%s'\n", parser->current_token->value);
        char* type_name = _strdup("int");
        parser_advance(parser); // eat 'int'

        if (parser->current_token->type != T_ASSIGN) {
            printf("Error: Expected '=' after type declaration\n");
            exit(EXIT_FAILURE);
        }
        parser_advance(parser); // eat '='

        ASTNode* value = parse_expression(parser);

        ASTNode* node = malloc(sizeof(ASTNode));
        node->type = AST_IMMUTABLE_ASSIGN;
        node->data.immutable_assign.var_name = var_name;
        node->data.immutable_assign.type_name = type_name;
        node->data.immutable_assign.value = value;

        done = true;
        return node;
    }
    else if (parser->current_token->type == T_FLOAT && !done) {
        printf("Confirmed type token: '%s'\n", parser->current_token->value);
        char* type_name = _strdup("float");
        parser_advance(parser); // eat 'float'

        if (parser->current_token->type != T_ASSIGN) {
            printf("Error: Expected '=' after type declaration\n");
            exit(EXIT_FAILURE);
        }
        parser_advance(parser); // eat '='

        ASTNode* value = parse_expression(parser);

        ASTNode* node = malloc(sizeof(ASTNode));
        node->type = AST_IMMUTABLE_ASSIGN;
        node->data.immutable_assign.var_name = var_name;
        node->data.immutable_assign.type_name = type_name;
        node->data.immutable_assign.value = value;
        
        return node;
    }
    else if (parser->current_token->type == T_STRING && !done) {
        printf("Confirmed type token: '%s'\n", parser->current_token->value);
        char* type_name = _strdup("str");
        parser_advance(parser); // eat 'str'

        if (parser->current_token->type != T_ASSIGN) {
            printf("Error: Expected '=' after type declaration\n");
            exit(EXIT_FAILURE);
        }
        parser_advance(parser); // eat '='

        ASTNode* value = parse_expression(parser);

        ASTNode* node = malloc(sizeof(ASTNode));
        node->type = AST_IMMUTABLE_ASSIGN;
        node->data.immutable_assign.var_name = var_name;
        node->data.immutable_assign.type_name = type_name;
        node->data.immutable_assign.value = value;
        
        return node;
    }
    else {
         printf("Error: Unknown type in immutable typed assignment\n");
         exit(EXIT_FAILURE);
     }
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

    ASTNode* result = parse_statement(parser);
    if (parser->current_token->type != T_EOF) {
        printf("Error: Unexpected token after statement: '%s'\n",
               parser->current_token->value);
        exit(EXIT_FAILURE);
    }
    
    return result;
}