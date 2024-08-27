#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void bienvenido(){
    printf("------------------------\n");
    printf("------------------------\n");
    printf("Shell en funcionamiento\n");
    printf("------------------------\n");
    printf("------------------------\n");

}

void parsearComando(const char *entrada, char *comando, char **argumentos, int *numArgs) {
    *numArgs = 0; // Inicializa el número de argumentos

    // Hacemos una copia de la entrada porque strtok modifica la cadena
    char copiaEntrada[1024];
    strcpy(copiaEntrada, entrada);

    // Extraemos el comando (primera palabra)
    char *token = strtok(copiaEntrada, " ");
    if (token != NULL) {
        strcpy(comando, token); // Guarda el comando
        argumentos[*numArgs] = comando; // El primer argumento es el comando en execvp
        (*numArgs)++;
    }

    // Extraemos los argumentos (resto de las palabras)
    while (token != NULL) {
        token = strtok(NULL, " ");
        if (token != NULL) {
            argumentos[*numArgs] = token; // Almacena el argumento
            (*numArgs)++;
        }
    }
    argumentos[*numArgs] = NULL;
}



int main(){
    char entrada[] = "";
    char comando[50];
    char *argumentos[10];
    int numArgs;
    bienvenido();
    printf(">");
    parsearComando(entrada, comando, argumentos, &numArgs);
    execvp(comando, argumentos);
    printf("Comando: %s\n", comando);
    printf("Argumentos:\n");
    for (int i = 0; i < numArgs; i++) {
        printf("Argumento %d: %s\n", i + 1, argumentos[i]);
    }


    return 0;
}