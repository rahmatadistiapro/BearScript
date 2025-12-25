#include "D:/BearScript/include/eval.h"
#include "D:/BearScript/include/AST.h"
#include "D:/BearScript/include/symbol_table.h"
#include "D:/BearScript/include/Value.h"
#include <stdio.h>
#include <string.h>

Value eval(ASTNode* node, SymbolTable* table) {
    switch (node->type) {
        case AST_INTEGER: {
            return number_value((double)node->data.value.int_val);
        }
        case AST_FLOAT: {
            return number_value(node->data.value.float_val);
        }
        case AST_STRING: {
            return string_value(node->data.string.str_val);
        }
        case AST_VARIABLE: {
            // TODO: Update get_variable to return Value
            double num_value;
            if (!get_variable(table, node->data.variable.var_name, &num_value)) {
                return error_value("Undefined variable");
            }
            return number_value(num_value);
        }
        case AST_ASSIGN: {
            char* var_name = node->data.assign.var_name;
            Value value = eval(node->data.assign.value, table);
            
            if (is_number(value)) {
                set_variable(table, var_name, as_number(value));
            }
            // TODO: Handle string assignments
            return value;
        }
        case AST_GROWL_STATEMENT: {
            Value value = eval(node->data.growl_stmt.expression, table);
            print_value(value);
            printf("\n");
            return value;
        }
        case AST_BINARY_OP: {
            Value left = eval(node->data.binary_op.left, table);
            Value right = eval(node->data.binary_op.right, table);
            
            // For now, only handle numbers in math operations
            if (!is_number(left) || !is_number(right)) {
                return error_value("Math operations only on numbers");
            }
            
            double l = as_number(left);
            double r = as_number(right);
            
            switch (node->data.binary_op.op) {
                case T_ADD:      return number_value(l + r);
                case T_SUBTRACT: return number_value(l - r);
                case T_MAUL:     return number_value(l * r);
                case T_DIVIDE:   
                    if (r == 0) return error_value("Division by zero");
                    return number_value(l / r);
                case T_MODULO:   
                    if ((int)r == 0) return error_value("Modulo by zero");
                    return number_value((int)l % (int)r);
                default:         return error_value("Unknown operator");
            }
        }
        case AST_TYPED_ASSIGN: {
            // NEW: Handle typed assignments
            char* var_name = node->data.typed_assign.var_name;
            char* type_name = node->data.typed_assign.type_name;
            Value value = eval(node->data.typed_assign.value, table);
            
            if (strcmp(type_name, "str") == 0) {
                if (!is_string(value)) {
                    return error_value("String variable requires string value");
                }
                // TODO: Store string in symbol table
                printf("Would store string: %s = %s\n", 
                       var_name, as_string(value));
                return value;
            } else {
                return error_value("Unknown type");
            }
        }
        default: {
            return error_value("Unknown AST node type");
        }
    }
}