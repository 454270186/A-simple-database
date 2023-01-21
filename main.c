#include "repl.h"
#include "compiler.h"


int main(int argc, char* argv[]) {
    InputBuffer* input_buffer = new_input_buffer();
    printf("Start...\n");

    while(true) {
        print_prompt();
        read_input(input_buffer);

        if(input_buffer->buffer[0] == '.') {
            switch(do_meta_command(input_buffer)) {
                case META_COMMAND_SUCCESS:
                    continue;

                case META_COMMAND_UNRECOGNIZED_COMMAND:
                    printf("Unrecognized command '%s'.\n", input_buffer->buffer);
                    continue;
            }
        }
        else {
            printf("Unrecognized command '%s'.\n", input_buffer->buffer);
        }
    }

    return 0;
}