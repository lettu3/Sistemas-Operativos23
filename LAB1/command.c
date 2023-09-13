#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <glib.h>
#include <assert.h>
#include <string.h>

#include "command.h"
#include "strextra.h"

#define STR_SIZE 257

// Estructura interna de los comandos simples
struct scommand_s {
    GList *cmd_with_args;
    char *input_redirect;
    char *output_redirect;
};

scommand scommand_new(void){
    scommand command = NULL;
    // Reserva espacio para la estructura y establece los campos a NULL
    command = malloc(sizeof(struct scommand_s));
    command->cmd_with_args = NULL;
    command->input_redirect = NULL;
    command->output_redirect = NULL;

    assert(command != NULL && scommand_is_empty (command) &&
                      scommand_get_redir_in (command) == NULL &&
                      scommand_get_redir_out (command) == NULL);

    return command;
}

scommand scommand_destroy(scommand self){
    assert(self != NULL);
    g_list_free_full(self->cmd_with_args, free);
    free(self->input_redirect);
    free(self->output_redirect);
    self->input_redirect=NULL;
    self->output_redirect=NULL;
    free(self);
    self = NULL;
    assert(self == NULL);
    return self;
}

void scommand_push_back(scommand self, char * argument) {
    assert(self!=NULL && argument!=NULL);
    
    // Añade al final de la lista de cadenas la nueva cadena
    self->cmd_with_args = g_list_append(self->cmd_with_args, argument);
    
    assert(!scommand_is_empty(self));
}

void scommand_pop_front(scommand self){
    assert(self!=NULL && !scommand_is_empty(self));
    
    // Elimina el primer elemento de la lista de cadenas y establece correctamente los punteros
    self->cmd_with_args = g_list_delete_link(self->cmd_with_args, g_list_first(self->cmd_with_args));
    
}

void scommand_set_redir_in(scommand self, char * filename) {
    assert(self!=NULL);
    if (self->input_redirect != NULL) {
        free(self->input_redirect);
    }
    // Establece el campo de redireccionamiento input del comando
    self->input_redirect = filename;
}

void scommand_set_redir_out(scommand self, char * filename){
    assert(self!=NULL);
    if (self->output_redirect != NULL) {
        free(self->output_redirect);
    }
    // Establece el campo de redireccionamiento output del comando
    self->output_redirect = filename;
}

bool scommand_is_empty(const scommand self){
    assert(self!=NULL);

    // Verifica si el largo de la lista de cadenas es 0
    return (g_list_length(self->cmd_with_args) == 0);
}

unsigned int scommand_length(const scommand self){
    assert(self!=NULL);

    // Devuelve el largo de la lista de cadenas del comando
    unsigned int res = g_list_length(self->cmd_with_args);
    assert((res==0) == scommand_is_empty(self));
    return res;
}

char * scommand_front(const scommand self){
    assert(self!=NULL && !scommand_is_empty(self));
    char * front;
    // Toma el elemento de la posicion 0 de la lista de cadenas, sin modificar la lista
    front = g_list_nth_data(self->cmd_with_args, 0u);
    assert(front!=NULL);
    return front;   
}

char * scommand_get_redir_in(const scommand self){
    assert(self!=NULL);
    // Devuelve el campo de redireccionamiento input
    char * redir_in = self->input_redirect;
    return redir_in;
}

char * scommand_get_redir_out(const scommand self){
    assert(self!=NULL);
    // Devuelve el campo de redireccionamiento output
    char * redir_out = self->output_redirect;
    return redir_out;
}

char * scommand_to_string(const scommand self){
	assert(self != NULL);
    // Toma el largo de la lista de cadenas
	unsigned int size = scommand_length(self);
	char * result = strdup("");
	if(!scommand_is_empty(self)){
        // Toma el primer elemento de la lista de cadenas
		result = str_merge_and_free(result, g_list_nth_data(self->cmd_with_args, 0));
        result = str_merge_and_free(result, " ");
		for (unsigned int i = 1; i < size; i++)
		{
            // Toma el elemento de la posicion i de la lista de cadenas y lo agrega a la cadena result
			result = str_merge_and_free(result, g_list_nth_data(self->cmd_with_args, i));
            result = str_merge_and_free(result, " ");
		}
		if (self->output_redirect!=NULL) 
		{
            // En caso de haber redireccionamiento output, lo agrega a la cadena result
			result = str_merge_and_free(result, " > ");
			result = str_merge_and_free(result, self->output_redirect);
		}
		if (self->input_redirect!=NULL) 
		{
            // En caso de haber redireccionamiento input, lo agrega a la cadena result
			result = str_merge_and_free(result, " < ");
			result = str_merge_and_free(result, self->input_redirect);
		}
	}
	assert(scommand_is_empty(self) ||
           scommand_get_redir_in(self)==NULL ||
           scommand_get_redir_out(self)==NULL ||
           strlen(result)>0);

	return result;
}

// Estructura principal del pipeline
struct pipeline_s {
    GList *scommands;
    bool wait;
};

pipeline pipeline_new(void) {
    pipeline new_pipe = NULL;
    // Reserva memoria para el pipeline, estableciendo el campo 'scommands' en NULL y el campo 'wait' a true
    new_pipe = malloc(sizeof(struct pipeline_s));
    new_pipe->scommands = NULL;
    new_pipe->wait = true;
    assert(new_pipe!=NULL && pipeline_is_empty(new_pipe) && pipeline_get_wait(new_pipe));
    return new_pipe;
}

static void g_destroy_notify (void* cmd) {
    scommand_destroy(cmd);
}

pipeline pipeline_destroy(pipeline self) {
    assert(self != NULL);
    g_list_free_full(self->scommands, g_destroy_notify); 
    // Funcion con potencial muy alto, el segundo argumento se puede configurar para liberar de la forma en la que se quiera.
    free(self);
    self = NULL;
    assert(self == NULL);
    return self;
}

void pipeline_push_back(pipeline self, scommand sc) {
    assert(self!=NULL && sc!=NULL);

    // Agrega un comando al final de la lista de comandos
    self->scommands = g_list_append(self->scommands,sc);

    assert(!pipeline_is_empty(self));
}

void pipeline_pop_front(pipeline self) {
    assert(self != NULL && !pipeline_is_empty(self));

    // Elimina el primer comando de la lista de comandos
    GList *aux = self->scommands;
    scommand_destroy(aux->data);
    self->scommands = aux->next;
    g_list_free_1(aux);
}

void pipeline_set_wait(pipeline self, const bool w){
    assert(self != NULL);

    // Establece el campo 'wait'
    self->wait = w;
}

bool pipeline_is_empty(const pipeline self) {
    assert(self != NULL);

    // Verifica si el largo de la lista de comandos es 0
    return g_list_length(self->scommands) == 0;
}

unsigned int pipeline_length(const pipeline self) {
    assert(self != NULL);

    // Devuelve el largo de la lista de comandos
    unsigned int length = g_list_length(self->scommands);

    assert((length == 0) == pipeline_is_empty(self));
    return length;
}

scommand pipeline_front(const pipeline self) {
    assert(self!=NULL && !pipeline_is_empty(self));

    // Toma el elemento en la posicion 0 de la lista de comandos
    scommand tempscommand = g_list_nth_data(self->scommands,0u);

    assert(tempscommand!=NULL);
    return tempscommand;
}

bool pipeline_get_wait(const pipeline self) {
    assert(self != NULL);
    // Devuelve el estado del campo 'wait'
    return self->wait;
}

static char* append_scommand_to_string(char* string, const scommand self) {
    assert(string != NULL && self != NULL);

    // Agrega un comando como cadena a la cadena 'string'
    char* myChar = scommand_to_string(self);
    string = str_merge_and_free(string, myChar);
    free(myChar);
    myChar = NULL;

    return (string);
}

char* pipeline_to_string(const pipeline self) {
    assert(self != NULL);

    GList* commands = self->scommands;
    char* result = strdup("");

    if (commands != NULL) {
        // Agrega como cadena el primer comando de la lista a la cadena 'result'
        result = append_scommand_to_string(result, g_list_nth_data(commands, 0u));
        commands = g_list_next(commands);

        // Si todavia quedan comandos, itera para agregarlos a la cadena 'result'
        while (commands != NULL) {
            result = str_merge_and_free(result, " | ");
            result = append_scommand_to_string(result,
                                               g_list_nth_data(commands, 0u));
            commands = g_list_next(commands);
        }

        // Si el campo 'wait' es true, agrega "&" a la cadena 'result'
        if (!pipeline_get_wait(self)) {
            result = str_merge_and_free(result, " &");
        }
    }

    assert(result == NULL || pipeline_is_empty(self) ||
           pipeline_get_wait(self) || strlen(result) > 0);

    return result;
}