#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include <stddef.h>
#include <stdbool.h>

// Funcion para simular la accion de "Ping"
void ping (int rally, int semaphore_ping, int semaphore_pong) {
    int pid = fork();

    if (pid == 0) {
        for (unsigned int i = 0u; i<rally ; i++) { // Recordemos que el for se usa si sabemos la cantidad de iteraciones

            if(sem_down(semaphore_pong) == false) {
                printf("ERROR : in sem_down");
                exit(0); // Error
            }

            printf("Ping\n");

            if(sem_up(semaphore_ping) == false) {
                printf("ERROR : in sem_up");
                exit(0); // Error
            }

        }
        exit(1); // Exito
    }
    else if (pid == -1) {
        printf("ERROR : in fork \n");
        exit(0); // Error
    }
}

// Funcion para simular la accion de "Pong"
void pong (int rally, int semaphore_ping, int semaphore_pong) {
    
    int pid = fork();

    if (pid == 0) {
        for (unsigned int i = 0u; i<rally ; i++) {

            if(sem_down(semaphore_ping) == false) {
                printf("ERROR : in sem_down");
                exit(0); // Error
            }

            printf("Pong\n");

            if(sem_up(semaphore_pong) == false) {
                printf("ERROR : in sem_up");
                exit(0); // Error
            }

        }
        exit(1); // Exito
    }
    else if (pid == -1) {
        printf("Error in fork \n");
        exit(0); // Error
    }
}

void main(int argc, char *argv[]) {

    int rally = atoi(argv[1]); // atoi convierte una cadena a su valor numerico entero

    if(argc != 2) {
        printf("ERROR");
        exit(0);
    }
    if(rally < 1) {
        printf("ERROR : Insert ammount larger than 0 : 'pingpong #', # is the ammount \n");
        exit(0);
    }

    int semaphore_ping = 1;
    int semaphore_pong = 0;

    while (sem_open(semaphore_ping, 0) == false) { // Semaforo 0, se inicializa con 0 posibles procesos
        semaphore_ping++;
    }
    while (sem_open(semaphore_pong, 1) == false) { // Semaforo 1, se inicializa con 1 posible proceso
        semaphore_pong++;
    }

    ping(rally, semaphore_ping, semaphore_pong); // Modularizacion del apartado para 'Ping'
    pong(rally, semaphore_ping, semaphore_pong); // Modularizacion del apartado para 'Pong'

    wait(NULL);
    /* Este wait es necesario por que si no se genera un error en el cual el simbolo '$' 
    de la consola aparece en medio de la output de pingpong, pues no esperaria a que termine*/
    
    // Cerrar semaforos
    if (sem_close(semaphore_pong) == false) {
        printf("Error in sem_close");
        exit(0); // Error
    }
    if (sem_close(semaphore_ping) == false) {
        printf("Error in sem_close");
        exit(0); // Error
    }

    exit(1);
}

