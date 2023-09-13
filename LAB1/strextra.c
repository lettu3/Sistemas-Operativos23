#include <stdlib.h>    /* calloc()...                        */
#include <string.h>    /* strlen(), strncat, strcopy()...    */
#include <assert.h>    /* assert()...                        */
#include "strextra.h"  /* Interfaz                           */

char * strmerge(char *s1, char *s2) {
    char *merge=NULL;
    size_t len_s1=strlen(s1);
    size_t len_s2=strlen(s2);
    assert(s1 != NULL && s2 != NULL);
    merge = calloc(len_s1 + len_s2 + 1, sizeof(char));
    strncpy(merge, s1, len_s1);
    merge = strncat(merge, s2, len_s2);
    assert(merge != NULL && strlen(merge) == strlen(s1) + strlen(s2));
    return merge;
}

char *str_merge_and_free(char *s1, const char *s2) {
    if (s1 == NULL || s2 == NULL) {
        return NULL;
    }

    const size_t s1_len = strlen(s1);
    const size_t s2_len = strlen(s2);
    const size_t new_len = s1_len + s2_len;

    // Reasignar memoria para la cadena resultante
    char *result = (char *)malloc(new_len + 1); // +1 para el carácter nulo

    if (result == NULL) {
        // Manejar el error de asignación de memoria
        free(s1);
        return NULL;
    }

    // Copiar s1 y s2 en la cadena resultante
    size_t i;
    for (i = 0; i < s1_len; i++) {
        result[i] = s1[i];
    }
    for (size_t j = 0; j < s2_len; j++, i++) {
        result[i] = s2[j];
    }

    // Agregar el carácter nulo al final
    result[new_len] = '\0';

    // Liberar la memoria de la cadena original (s1)
    free(s1);

    return result;
}
