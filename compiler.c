#include <string.h>
#include "compiler.h"

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
    if(strcmp(input_buffer->buffer, ".exit") == 0) {
        close_input_buffer(input_buffer);
        exit(EXIT_SUCCESS);
    }
    else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if(strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->statement_type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if(strncmp(input_buffer->buffer, "select", 6) == 0) {
        statement->statement_type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_COMMAND;
}

void execute_statement(Statement* statement) {
    switch(statement->statement_type) {
        case STATEMENT_INSERT:
            printf("I will do insert here\n");
            break;

        case STATEMENT_SELECT:
            printf("I will do select here\n");
            break;
    }
}