#include "D:/BearScript/include/eval.h"
#include "D:/BearScript/include/Value.h"
#include "D:/BearScript/include/symbol_table.h"
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
            return string_value(_strdup(node->data.string.str_val));
        }
        case AST_VARIABLE: {
            Value value;
            if (!get_variable(table, node->data.variable.var_name, &value)) {
                return error_value(_strdup("Undefined variable, cannot find variable"));
            }
            return value;
        }
        case AST_ASSIGN: {
            char* var_name = node->data.assign.var_name;
            Value value = eval(node->data.assign.value, table);
            if (!assign_variable(table, var_name, value)) {
                return error_value(_strdup("Cannot reassign to immutable variable"));
            }
            return value;
        }
        case AST_TYPED_ASSIGN: {
            char* var_name = node->data.typed_assign.var_name;
            char* type_name = node->data.typed_assign.type_name;
            Value value = eval(node->data.typed_assign.value, table);
            
            if (strcmp(type_name, "str") == 0) {
                if (!is_string(value)) {
                    return error_value(_strdup("String variable requires to have a string value"));
                }
                assign_variable(table, var_name, value);
                return value;
            } 
            else if (strcmp(type_name, "int") == 0 || strcmp(type_name, "float") == 0) {
                if (!is_number(value)) {
                    return error_value(_strdup("Number variable requires to have a number value"));
                }
                assign_variable(table, var_name, value);
                return value;
            }
            else if (strcmp(type_name, "float") == 0) {
                if (!is_number(value)) {
                    return error_value(_strdup("Float variable requires to have a float value"));
                }
                assign_variable(table, var_name, value);
                return value;
            }
            else {
                return error_value(_strdup("Unknown type, did you mean: 'str', 'int', or 'float'?"));
            }
        }
        case AST_IMMUTABLE_ASSIGN: {
            char* var_name = node->data.immutable_assign.var_name;
            char* type_name = node->data.immutable_assign.type_name;
            Value value = eval(node->data.immutable_assign.value, table);
            if (!define_variable(table, var_name, value, true)) {
                return error_value(_strdup("cannot defined a variable that is already defined variable"));
            }
            if (type_name == NULL) {
                return value;
            }
            if (strcmp(type_name, "str") == 0) {
                if (!is_string(value)) {
                    return error_value(_strdup("String variable requires to have a string value"));
                }
                return value;
            } 
            else if (strcmp(type_name, "int") == 0 || strcmp(type_name, "float") == 0) {
                if (!is_number(value)) {
                    return error_value(_strdup("Number variable requires to have a number value"));
                }
                return value;
            }
            else if (strcmp(type_name, "float") == 0) {
                if (!is_number(value)) {
                    return error_value(_strdup("Float variable requires to have a float value"));
                }
                return value;
            }
            else {
                return error_value(_strdup("Unknown type, did you mean: 'str', 'int', or 'float'?"));
            }
        }
        case AST_GROWL_STATEMENT: {
            Value value = eval(node->data.growl_stmt.expression, table);
            return value;
        }
        case AST_BINARY_OP: {
            Value left = eval(node->data.binary_op.left, table);
            Value right = eval(node->data.binary_op.right, table);
            
            // Handle number operations
            if (is_number(left) && is_number(right)) {
                double l = as_number(left);
                double r = as_number(right);
                
                switch (node->data.binary_op.op) {
                    case T_ADD:   { return number_value(l + r); }
                    case T_SUBTRACT: { return number_value(l - r); }
                    case T_MAUL:     { return number_value(l * r); }
                    case T_DIVIDE: {
                        if (r == 0) { return error_value(_strdup("cannot do Division by zero")); }
                        return number_value(l / r);
                    }
                    case T_MODULO: {
                        if ((int)r == 0) { return error_value(_strdup("cannot do Modulo by zero")); }
                        return number_value((int)l % (int)r);
                    }
                    default:         return error_value(_strdup("Unknown operator, did you mean: '+', '-', '*', '/', '%'?"));
                }
            }
            // Handle string concatenation (future)
            else if (is_string(left) && is_string(right) && node->data.binary_op.op == T_ADD) {
                // TODO: Implement string concatenation
                return error_value(_strdup("String concatenation not implemented yet"));
            }
            else {
                return error_value(_strdup("Type mismatch in operation"));
            }
        }
        default: {
            return error_value(_strdup("Unknown keyword"));
        }
    }
}