#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "repl.h"
// Initialize a input buffer
InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_buffer = 0;

    return input_buffer;
}

// Prints a prompt to users
void print_prompt() { printf("xiaofeiDB > "); }

void read_input(InputBuffer* input_buffer) {
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0) {
        printf("Error reading input.");
        exit(EXIT_FAILURE);
    }

    // ignore the '\n'
    input_buffer->input_buffer = bytes_read-1;
    input_buffer->buffer[bytes_read - 1] = 0;
}

// destructs input buffer
void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}