#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"

static void show_prompt(void) {
    printf ("mybash> ");
    fflush (stdout);
}

int main(int argc, char *argv[]) {
    //pipeline pipe;ss
    Parser input;
    bool quit = false;

    input = parser_new(stdin);
    while (!quit) {
        show_prompt();
        pipeline apipe = parse_pipeline(input);
        execute_pipeline(apipe);
        apipe = pipeline_destroy(apipe);
        /* Hay que salir luego de ejecutar? */
        quit = parser_at_eof(input);
        /*
         * COMPLETAR
         *
         */
    }
    parser_destroy(input); input = NULL;
    return EXIT_SUCCESS;
}

