#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


void parsearComando(const char *entrada, char *comando, char **argumentos) {
    int numArgs = 0; // Inicializa el número de argumentos

    // Hacemos una copia de la entrada porque strtok modifica la cadena
    char copiaEntrada[1024];
    strcpy(copiaEntrada, entrada);

    // Extraemos el comando (primera palabra)
    char *token = strtok(copiaEntrada, " ");
    if (token != NULL) {
        strcpy(comando, token); // Guarda el comando
        argumentos[numArgs] = comando; // El primer argumento es el comando en execvp
        (numArgs)++;
    }

    // Extraemos los argumentos (resto de las palabras)
    while (token != NULL) {
        token = strtok(NULL, " ");
        if (token != NULL) {
            argumentos[numArgs] = token; // Almacena el argumento
            (numArgs)++;
        }
    }
    argumentos[numArgs] = NULL;
}

void procesoconcurrente(char *comando, char **argumentos){
    int pc_id = fork();
    
    if(pc_id==0){
        execvp(comando, argumentos);
    }else if(pc_id>0){
        wait(NULL);

    }else if(pc_id<0){
        printf("Error al crear el hijo");
    }
    
}

int main(){
    char entrada[200];
    char comando[50];
    char *argumentos[10];
    while (1){
        printf(">");
        fgets(entrada, sizeof(entrada), stdin);
        entrada[strcspn(entrada, "\n")] = '\0';
        if(strcmp(entrada, "exit") == 0) break;
        parsearComando(entrada, comando, argumentos);
        procesoconcurrente(comando, argumentos);
        /*//execvp(comando, argumentos);
        printf("Comando: %s\n", comando);
        printf("Argumentos:\n");
        for (int i = 0; i < numArgs; i++) {
            printf("Argumento %d: %s ", i + 1, argumentos[i]);
        }
        printf("\n");*/
    }
    


    return 0;
}