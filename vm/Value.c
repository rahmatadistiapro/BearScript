#include "D:\BearScript/include/Value.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

Value integer_value(long value) {
    Value v;
    v.type = VALUE_INTEGER;
    v.as.integer = value;
    return v;
}

Value float_value(double value) {
    Value v;
    v.type = VALUE_FLOAT;
    v.as.float_val = value;
    return v;
}

Value string_value(const char* value) {
    Value v;
    v.type = VALUE_STRING;
    v.as.string = _strdup(value);
    return v;
}

Value boolean_value(bool value) {
    Value v;
    v.type = VALUE_BOOLEAN;
    v.as.boolean = value;
    return v;
}

Value nil_value() {
    Value v;
    v.type = VALUE_NIL;
    return v;
}

Value error_value(const char *message) {
    Value v;
    v.type = VALUE_ERROR;
    v.as.error_message = _strdup(message);
    return v;
}

bool is_integer(Value value) { return value.type == VALUE_INTEGER; }
bool is_float(Value value) { return value.type == VALUE_FLOAT; }
bool is_number(Value value) { return is_integer(value) || is_float(value); }
bool is_string(Value value) { return value.type == VALUE_STRING; }
bool is_boolean(Value value) { return value.type == VALUE_BOOLEAN; }
bool is_nil(Value value) { return value.type == VALUE_NIL; }
bool is_immutable(Value value) { return value.type == VALUE_BOOLEAN; } // Placeholder
bool is_error(Value value) { return value.type == VALUE_ERROR; }

long as_integer(Value value) {
    if (is_integer(value)) {
        return value.as.integer;
    }
    else if (is_float(value)) {
        return (long)(value.as.float_val);
    }
    printf("Error: Value is not a number\n");
    exit(1);
}
double as_float(Value value) {
    if (is_float(value)) {
        return value.as.float_val;
    }
    else if (is_integer(value)) {
        return (double)(value.as.integer);
    }
    printf("Error: Value is not a number\n");
    exit(1);
}

char* as_string(Value value) {
    if (is_string(value)) {
        return value.as.string;
    }
    printf("Error: Value is not a string\n");
    exit(1);
}

bool as_boolean(Value value) {
    if (is_boolean(value)) {
        return value.as.boolean;
    }
    printf("Error: Value is not a boolean\n");
    exit(1);
}

void print_value(Value value) {
    switch (value.type) {
        case VALUE_FLOAT:
            printf("%g", value.as.float_val);
            break;
        case VALUE_INTEGER:
            printf("%ld", value.as.integer);
            break;
        case VALUE_STRING:
            printf("%s", value.as.string);
            break;
        case VALUE_BOOLEAN:
            printf(value.as.boolean ? "true" : "false");
            break;
        case VALUE_NIL:
            printf("nil");
            break;
        case VALUE_ERROR:
            printf("Error: %s", value.as.error_message);
            break;
    }
}

bool values_equal(Value a, Value b) {
    if (a.type != b.type) { return false; }
    switch (a.type) {
        case VALUE_FLOAT:
            return a.as.float_val == b.as.float_val;
        case VALUE_INTEGER:
            return a.as.integer == b.as.integer;
        case VALUE_STRING:
            return strcmp(a.as.string, b.as.string) == 0;
        case VALUE_BOOLEAN:
            return a.as.boolean == b.as.boolean;
        case VALUE_NIL:
            return true;
        case VALUE_ERROR:
            return strcmp(a.as.error_message, b.as.error_message) == 0;
    }
    return false;
}

void free_value(Value value) {
    if (is_string(value) || is_error(value)) {
        free(value.as.string);
    }
}

Value copy_value(Value original) {
    Value copy;
    copy.type = original.type;
    
    switch (original.type) {
        case VALUE_FLOAT:
            copy.as.float_val = original.as.float_val;
            break;
        case VALUE_INTEGER:
            copy.as.integer = original.as.integer;
            break;
        case VALUE_STRING:
        case VALUE_ERROR:
            // Deep copy: duplicate the string
            copy.as.string = (original.as.string == NULL) 
                ? NULL 
                : _strdup(original.as.string);
            break;
        case VALUE_BOOLEAN:
            copy.as.boolean = original.as.boolean;
            break;
        case VALUE_NIL:
            // Nothing to copy
            break;
    }
    return copy;
}