#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "tests/syscall_mock.h"
#include "builtin.h"
#include "command.h"
#include "strextra.h"
#include "execute.h"

// Función para ejecutar comandos externos
static void execute_external(scommand cmd) {
    char *redir_in = scommand_get_redir_in(cmd);
    char *redir_out = scommand_get_redir_out(cmd);

    // Manejar la redirección de entrada si está presente
    if (redir_in != NULL) {
        int file_in = open(redir_in, O_RDONLY, S_IRUSR);
        if (file_in == -1) {
            perror(redir_in);
            exit(EXIT_FAILURE);
        }

        if (dup2(file_in, STDIN_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        close(file_in);
    }

    // Manejar la redirección de salida si está presente
    if (redir_out != NULL) {
        int file_out = open(redir_out, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        if (file_out == -1) {
            perror(redir_out);
            exit(EXIT_FAILURE);
        }

        if (dup2(file_out, STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        close(file_out);
    }

    unsigned int length = scommand_length(cmd);
    char **argv = calloc(sizeof(char *), length + 1);

    // exec necesita un argv con un último elemento NULL, por eso el +1

    // Construir el arreglo de argumentos (argv)
    if (argv != NULL) {
        for (unsigned int i = 0; i < length; ++i) {
            char *arg = scommand_front(cmd);
            scommand_pop_front(cmd);
            argv[i] = arg;
        }
        argv[length] = NULL;
    } else {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    execvp(argv[0], argv);

    // Si llegamos a este punto, hubo un error en execvp
    perror("execvp");
    free(argv);
    exit(EXIT_FAILURE);
}

// Función para ejecutar un comando en la tubería
static void execute_command(pipeline apipe) {
    scommand cmd = pipeline_front(apipe);

    if (builtin_is_internal(cmd)) {
        builtin_run(cmd); // Ejecutar un comando interno
    } else {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Este es el proceso hijo que ejecuta un comando externo
            execute_external(cmd);
        }
        // El código a continuación pertenece al proceso padre
    }
}

// Función para ejecutar una serie de comandos en segundo plano
static void execute_commands(pipeline apipe) {
    unsigned int how_many_cmd = pipeline_length(apipe) - 1;
    // Para conectar varios comandos se necesitan "cantidad de comandos -1" tuberias
    // por que el ultimo elemento del pipe no se tiene que conectar con un elemento subsecuente.
    int *pipe_fds = malloc(sizeof(int) * how_many_cmd * 2);

    if (pipe_fds == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    for (unsigned int i = 0; i < how_many_cmd; i++) {
        if (pipe(pipe_fds + i * 2) < 0) {
            perror("pipe");
            free(pipe_fds);
            exit(EXIT_FAILURE);
        }
    }

    unsigned int i = 0;

    while (!pipeline_is_empty(apipe)) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            free(pipe_fds);
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            
            // Este es el proceso hijo que ejecuta un comando en una tubería
            // Configurar la redirección de tuberías y ejecutar el comando
            // El proceso hijo sale de la función después de ejecutar el comando

            // Configurar la redirección de tuberías
            if (pipeline_length(apipe) > 1) {
                int dup2_ = dup2(pipe_fds[i + 1], STDOUT_FILENO);
                if (dup2_ == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            if (i != 0) {
                int dup2_ = dup2(pipe_fds[i - 2], STDIN_FILENO);
                if (dup2_ == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Cerrar todos los descriptores de tubería
            for (unsigned int j = 0; j < how_many_cmd * 2; j++) {
                close(pipe_fds[j]);
            }

            scommand cmd = pipeline_front(apipe);

            if (builtin_is_internal(cmd)) {
                builtin_run(cmd);
            } else if (!scommand_is_empty(cmd)) {
                execute_external(cmd);
            }
        } else {
            i = i + 2;
            pipeline_pop_front(apipe);
        }
    }

    // Cerrar todos los descriptores de tubería en el proceso padre
    for (unsigned int j = 0; j < how_many_cmd * 2; j++) {
        close(pipe_fds[j]);
    }

    free(pipe_fds);
}

// Función para ejecutar una serie de comandos en segundo plano
static void execute_background(pipeline apipe) {
    unsigned int how_many_cmd = pipeline_length(apipe);

    if (how_many_cmd == 0) {
        fprintf(stderr, "No commands\n");
    } else if (how_many_cmd == 1) {
        execute_command(apipe);
    } else {
        execute_commands(apipe);
    }
}

// Función para ejecutar una serie de comandos en primer plano
static void execute_foreground(pipeline apipe) {
    unsigned int how_many_cmd = pipeline_length(apipe);
    execute_background(apipe);

    // Esperar a que terminen todos los procesos hijos
    while (how_many_cmd > 0) {
        wait(NULL);
        how_many_cmd--;
    }
}

// Función principal para ejecutar una tubería de comandos
void execute_pipeline(pipeline apipe) {
    assert(apipe != NULL);

    if (pipeline_get_wait(apipe)) { // Se tiene que esperar a que termine el proceso con todos los hijos (Foreground)
        execute_foreground(apipe);
    } else { // No hace falta esperar (Background)
        execute_background(apipe);
    }
}