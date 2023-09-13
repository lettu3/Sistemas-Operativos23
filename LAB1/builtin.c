#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "builtin.h"
#include "command.h"
#include "strextra.h"

#include "tests/syscall_mock.h"

void use_cd (scommand cmd);
void use_help (void);
void use_exit (void);

bool builtin_is_internal(scommand cmd) {
    assert(cmd != NULL);
    // Toma la primer cadena de 'cmd' y verifica si es igual a las cadenas "cd", "help" o "exit"
    char *command = scommand_front(cmd);
    bool res = ((strcmp(command, "cd") == 0) ||
               ((strcmp(command, "help") == 0) ||
               ((strcmp(command, "exit") == 0))));
    return res; 
}

bool builtin_alone(pipeline p){
    assert(p != NULL);
    // Verficia si el largo del pipeline es 1
    bool res = pipeline_length(p) == 1;
    assert(res == (pipeline_length(p) == 1) && builtin_is_internal(pipeline_front(p)));
    return res;
}

void builtin_run(scommand cmd) {
    assert(builtin_is_internal(cmd));
    char *command = scommand_front(cmd);

    // Si el comando es interno, ejecuta el comando correspondiente
    if ((strcmp(command, "cd") == 0)) {
        use_cd(cmd);
    }
    else if ((strcmp(command, "help") == 0)) {
        use_help();
    }
    else if ((strcmp(command, "exit") == 0)) {
        use_exit();
    }
}


/* 
 * Ejecuta el comando interno "cd".
 * 
 * REQUIRES: cmd!=NULL
*/
void use_cd (scommand cmd) {
    assert(cmd!=NULL);

    // Solo reqiuere los argumentos "cd" y el directorio al que se desea cambiar
    if (scommand_length(cmd) > 2){
        fprintf(stderr, "-mybash: cd: too many arguments\n");
        return;
    }
    scommand_pop_front(cmd);

    int err = 0;
    char *directory = NULL;

    // Si no tiene ningun argumento va a el directorio principal
    if(scommand_is_empty(cmd)) { 
        directory = getenv("HOME");
    }
    else
    {
        // En caso de que haya mas argumentos, verifica diferentes opciones
        char *arg = scommand_front(cmd);

        // Cuando es '-' solo, envia al directorio al que se accedio anteriormente
        if (strcmp(arg, "-") == 0) { 
            directory = getenv("OLDPWD");
            printf("%s", directory);
        }
        // Si es '~' tambien lo envia al directorio principal
        else if (strcmp(arg, "~") == 0) {
            directory = getenv("HOME");
            // Puede extenderse la ruta para ir a un directorio desde el directorio principal
            if(arg[1] == '/') {
                directory = strmerge(directory, &arg[1]);
            } 
        }
        // Cualquier otro caso de directorios especificos dentro del actual
        else
        {
            directory = arg;    
        }
    }
    
    err = chdir(directory);

    // Si se intenta cambiar a un directorio que no existe tira error
    if (err!=0)
    {
        fprintf(stderr, "-mybash: cd: %s: No such file or directory\n", directory);
        return;
    }
    
}


/*
 * Ejecuta el comando interno "exit".
 * 
 * ENSURES: Salida del programa 'mybash'.
*/
void use_exit(void){
    close(STDIN_FILENO);
    exit(0);
}


/*
 * Ejecuta el comando interno "help".
*/
void use_help () {
    printf("┌──────────────────────────── •✧• ────────────────────────────┐\n"); 
  	printf("	    ___  ___     ______           _     \n");
  	printf("	    |  \\/  |     | ___ \\         | |    \n");
  	printf("	    | .  . |_   _| |_/ / __ _ ___| |__  \n");
  	printf("	    | |\\/| | | | | ___ \\/ _` / __| '_ \\ \n");
  	printf("	    | |  | | |_| | |_/ / (_| \\__ \\ | | |\n");
  	printf("	    \\_|  |_/\\__, \\____/ \\__,_|___/_| |_|\n");
   	printf("	            __/ |                      \n");
   	printf("	           |___/                       \n");
    printf("\n");
  	printf("  © 2023 FaMAF-UNC \n");
    printf("  Created By: Ernesto Agustin Alvarez Baquero\n");
	printf("	          Ignacio Gomez Barrios\n");
	printf("	          Arne José Müller Carrizo\n");
	printf("	          Alejo Valentin Saavedra\n");
    printf("\n");
    printf(" ────────────────────●Comandos Internos●────────────────────\n");
    printf(" cd   ➤ Changes the directory in which we are\n");
    printf(" help ➤ Information about the shell and its internal commands\n");
    printf(" exit ➤ Terminate the shell cleanly\n");
    printf("\n");
    printf("└──────────────────────────── •✧• ────────────────────────────┘ \n");
}