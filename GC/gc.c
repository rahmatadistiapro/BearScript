#include "D:\BearScript\include\AST.h"
#include "D:\BearScript\include\gc.h"
#include <stdlib.h> // For free()
#include <stdio.h> // For printf()

void gc_free_ast(ASTNode* node) {
    if (node == NULL) {
        return;
    }

    // First, recursively free all child nodes and their data
    switch (node->type) {
        case AST_GROWL_STATEMENT:
            gc_free_ast(node->data.growl_stmt.expression);
            break;

        case AST_STRING:
            // Free the heap-allocated string copy
            if (node->data.string.str_val != NULL) {
                free(node->data.string.str_val);
            }
            break;

        case AST_ASSIGN:
            // Free the variable name string and the value expression
            if (node->data.assign.var_name != NULL) {
                free(node->data.assign.var_name);
            }
            gc_free_ast(node->data.assign.value);
            break;

        case AST_TYPED_ASSIGN:
            // Free both name strings and the value expression
            if (node->data.typed_assign.var_name != NULL) {
                free(node->data.typed_assign.var_name);
            }
            if (node->data.typed_assign.type_name != NULL) {
                free(node->data.typed_assign.type_name);
            }
            gc_free_ast(node->data.typed_assign.value);
            break;

        case AST_IMMUTABLE_ASSIGN:
            // Free both name strings and the value expression
            if (node->data.immutable_assign.var_name != NULL) {
                free(node->data.immutable_assign.var_name);
            }
            if (node->data.immutable_assign.type_name != NULL) {
                free(node->data.immutable_assign.type_name);
            }
            gc_free_ast(node->data.immutable_assign.value);
            break;

        case AST_VARIABLE:
            // Free the variable name string
            if (node->data.variable.var_name != NULL) {
                free(node->data.variable.var_name);
            }
            break;

        case AST_BINARY_OP:
            // Recursively free the left and right child expressions
            gc_free_ast(node->data.binary_op.left);
            gc_free_ast(node->data.binary_op.right);
            break;

        case AST_INTEGER:
        case AST_FLOAT:
            // These nodes (types 3 & 4) hold their value directly in the union.
            // No extra heap memory to free here.
            break;

        default:
            // This is a safety net. If you add a new AST type, don't forget to handle it here!
            printf("Warning: gc_free_ast encountered unknown AST type: %d\n", node->type);
            break;
    }

    // Finally, free the memory for the ASTNode struct itself
    free(node);
}