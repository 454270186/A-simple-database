#ifndef _REPL_H
#define _REPL_H


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_buffer;
} InputBuffer;

// Initialize a input buffer
InputBuffer* new_input_buffer();

// Prints a prompt to users
void print_prompt();

// read user's input into buffer
void read_input(InputBuffer* input_buffer);

// destructs input buffer
void close_input_buffer(InputBuffer* input_buffer);



#endif