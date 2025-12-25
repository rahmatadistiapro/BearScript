#include "D:\BearScript/include/Value.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Value number_value(double value) {
    Value v;
    v.type = VALUE_NUMBER;
    v.as.number = value;
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

bool is_number(Value value) { return value.type == VALUE_NUMBER; }
bool is_string(Value value) { return value.type == VALUE_STRING; }
bool is_boolean(Value value) { return value.type == VALUE_BOOLEAN; }
bool is_nil(Value value) { return value.type == VALUE_NIL; }
bool is_error(Value value) { return value.type == VALUE_ERROR; }

double as_number(Value value) {
    if (is_number(value)) {
        return value.as.number;
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
        case VALUE_NUMBER:
            printf("%g", value.as.number);
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
        case VALUE_NUMBER:
            return a.as.number == b.as.number;
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