#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "parsing.h"
#include "parser.h"
#include "command.h"



static scommand parse_scommand(Parser p) {
    assert(p != NULL && !parser_at_eof(p));
    scommand cmd = scommand_new();      //inicializo el scommand

    char * argument;                    //argumento a parsear
    arg_kind_t arg_type;                    //su tipo
   
    parser_skip_blanks(p);              //salto posibles espacios en blanco
    argument = parser_next_argument(p, &arg_type);      //parseo el argumento

    while (argument != NULL && !parser_at_eof(p)){ // si no se pudo leer el argumento, no entra/sale al ciclo
        if (arg_type == ARG_NORMAL){
            scommand_push_back(cmd, argument);          //argumento normal
        }
        else if(arg_type == ARG_INPUT){
            scommand_set_redir_in(cmd, argument);       //redireccion de entrada
        }
        else if(arg_type == ARG_OUTPUT){
            scommand_set_redir_out(cmd, argument);      //redireccion de salida
        }
        
        parser_skip_blanks(p);                      //espacios entre argumentos
        argument = parser_next_argument(p, &arg_type); //intento leer un siguiente argumento
    }

    if (scommand_is_empty(cmd)){
        cmd = scommand_destroy(cmd);  //si hubo error, destruyo la estructura
        cmd = NULL;
    }
    return cmd;
}

/*
 * Lee todo un pipeline de `parser' hasta llegar a un fin de línea (inclusive)
 * o de archivo.
 * Devuelve un nuevo pipeline (a liberar por el llamador), o NULL en caso
 * de error.
 * REQUIRES:
 *     parser != NULL
 *     ! parser_at_eof (parser)
 * ENSURES:
 *     No se consumió más entrada de la necesaria
 *     El parser esta detenido justo luego de un \n o en el fin de archivo.
 *     Si lo que se consumió es un pipeline valido, el resultado contiene la
 *     estructura correspondiente.
 */
pipeline parse_pipeline(Parser p) {
    assert((p != NULL) && !parser_at_eof(p));
    
    pipeline result = pipeline_new();
    scommand cmd = NULL;
    bool error = false, another_pipe=true;

    cmd = parse_scommand(p);

    error = (cmd==NULL); /* Comando inválido al empezar */

    while (another_pipe && !error) {
        pipeline_push_back(result, cmd);    //pusheo el scommand al pipeline si fue valido

        parser_op_pipe(p, &another_pipe); // intento consumir un "|"

        if (another_pipe){
            cmd = parse_scommand(p);
            error = (cmd == NULL);              // daria error si tengo algo de la forma "<place_holder> | \n" 
        }
    }


    /* Opcionalmente un OP_BACKGROUND al final */
    if (!error){
        bool was_op_background = false;
        parser_op_background(p, &was_op_background);
        if (was_op_background){                 //si se encontro un &, el shell no espera al comando a terminar
            pipeline_set_wait(result, false);   //simplemente lo ejecuta y sigue funcionando
        }
    }
    /* Tolerancia a espacios posteriores */
    parser_skip_blanks(p);
    /* Consumir todo lo que hay inclusive el \n */
    bool there_was_garbage = false;
    parser_garbage(p, &there_was_garbage);
    if (there_was_garbage){
        fprintf(stdout, "There was garbage: %s\n", parser_last_garbage(p));
    }
    /* Si hubo error, hacemos cleanup */
    if(error){
        result = pipeline_destroy(result);
        result = NULL;
        fprintf(stderr, "Syntax error!\n");
    }
    return result; // MODIFICAR
}