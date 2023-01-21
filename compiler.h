#ifndef _COMPILER_H
#define _COMPILER_H


#include "repl.h"

// compiler will deal with "meta_command" and "SQL Statement"

// meta_command types
typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

// statement types
typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_COMMAND
} PrepareResult;

// sql types
typedef enum {
    STATEMENT_SELECT,
    STATEMENT_INSERT
} StatementTypes;

typedef struct {
    StatementTypes statement_type;
} Statement;

// deal with meta_command
MetaCommandResult do_meta_command(InputBuffer* input_buffer);

#endif