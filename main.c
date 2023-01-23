#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
#define TABLE_MAX_PAGES 100
/*
    The column will be like:

    Columns:       Type:
    ID             Integer
    username       Varchar(32)
    email          Varchar(255)

*/

/*
    the relations among row, page and table:
    rows -> pages -> table
*/


// the table structure
typedef struct {
    uint32_t num_rows;
    void* pages[100];
} Table;

// the row structure
typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_buffer;
} InputBuffer;


/*
    Compilier 
*/

// execute result
typedef enum {
    EXECUTED_SUCCESS,
    EXECUTED_TABLE_FULL
} ExecutedResult;


// meta_command types
typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

// statement types
typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_SYNTAX_ERROR
} PrepareResult;

// sql types
typedef enum {
    STATEMENT_SELECT,
    STATEMENT_INSERT
} StatementTypes;

typedef struct {
    StatementTypes statement_type;
    Row row_to_insert; // only used by insert statement
} Statement;


/*
    Structure
*/

// the compact representation of a row
const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);

const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;

const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;


// Table structure consists of many pages usd to store rows
// rows -> pages -> table
const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROW_SIZE * ROWS_PER_PAGE;

void print_row(Row* row) {
    printf("(%d %s %s)\n", row->id, row->username, row->email);
}

// convert to or from a compact structure
void serialize_row(Row* source, void* destination) {
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}
void deserialize_row(void* source, Row* destination) {
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

// find the exact place to read or write
void* row_slot(Table* table, uint32_t rows_num) {
    uint32_t page_num = rows_num / ROWS_PER_PAGE;
    void* page = table->pages[page_num];
    // if this page does not exist yet, init one
    if (page == NULL) {
        page = table->pages[page_num] = malloc(sizeof(PAGE_SIZE));
    }

    uint32_t row_offset = rows_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;

    return page + byte_offset;
}

// construct and destruct table
Table* new_table() {
    Table* table = (Table*)malloc(sizeof(Table));
    table->num_rows = 0;
    for(uint32_t i = 0; i < 100; i++) {
        table->pages[i] = NULL;
    }

    return table;
}
void free_table(Table* table) {
    for(int i = 0; table->pages[i]; i++) {
        free(table->pages[i]);
    }
    free(table);
}


// construct and destruct input buffer
InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_buffer = 0;

    return input_buffer;
}
void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

// Statement handlers
MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table* table) {
    if(strcmp(input_buffer->buffer, ".exit") == 0) {
        close_input_buffer(input_buffer);
        free_table(table);
        exit(EXIT_SUCCESS);
    }
    else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if(strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->statement_type = STATEMENT_INSERT;
        int assigned = sscanf(input_buffer->buffer, "insert %d %s %s",
            &statement->row_to_insert.id,
            &statement->row_to_insert.username,
            &statement->row_to_insert.email
        );
        
        if(assigned < 3){
            return PREPARE_SYNTAX_ERROR;
        }

        return PREPARE_SUCCESS;
    }

    if(strncmp(input_buffer->buffer, "select", 6) == 0) {
        statement->statement_type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecutedResult execute_insert(Statement* statement, Table* table) {
    if(table->num_rows >= TABLE_MAX_ROWS) {
        return EXECUTED_TABLE_FULL;
    }

    Row* row = &(statement->row_to_insert);
    serialize_row(row, row_slot(table, table->num_rows));

    table->num_rows++;

    return EXECUTED_SUCCESS;
}

ExecutedResult execute_select(Statement* statement, Table* table) {
    Row row;
    for(uint32_t i = 0; i < table->num_rows; i++) {
        deserialize_row(row_slot(table, i), &row);
        // TODO -- print out the row
        print_row(&row);
    }

    return EXECUTED_SUCCESS;
}

ExecutedResult execute_statement(Statement* statement, Table* table) {
    switch(statement->statement_type) {
        case STATEMENT_INSERT:
            //printf("I will do insert here\n");

            return execute_insert(statement, table);

        case STATEMENT_SELECT:
            //printf("I will do select here\n");

            return execute_select(statement, table);
    }
}


/*
    main
*/

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


int main(int argc, char* argv[]) {
    InputBuffer* input_buffer = new_input_buffer();
    Table* table = new_table();
    printf("Start...\n");

    while(true) {
        print_prompt();
        read_input(input_buffer);

        if(input_buffer->buffer[0] == '.') {
            switch (do_meta_command(input_buffer, table)) {
                case META_COMMAND_SUCCESS:
                    continue;

                case META_COMMAND_UNRECOGNIZED_COMMAND:
                    printf("Unrecognized command '%s'.\n", input_buffer->buffer);
                    continue;
            }
        }
        
        Statement statement; // execute the statement, if success, get the statement type
        switch (prepare_statement(input_buffer, &statement)) {
            case PREPARE_SUCCESS:
                // To execute
                break;
            
            case PREPARE_UNRECOGNIZED_STATEMENT:
                printf("Unrecognized key word with start of %s\n", input_buffer->buffer);
                continue;

            case PREPARE_SYNTAX_ERROR:
                printf("Syntax Error\n");
                continue;
        }

        switch(execute_statement(&statement, table)) {
            case EXECUTED_TABLE_FULL:
                printf("Error: Table Full\n");
                break;

            case EXECUTED_SUCCESS:
                printf("Executed.\n");
                break;
        }


    }

    return 0;
}