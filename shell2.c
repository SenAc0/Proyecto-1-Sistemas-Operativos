#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_ARGS 100
#define MAX_PIPES 100
#define MAX_CHAR 200



typedef struct node{
    int n;
    char *linea;
    struct node* next;
} node;

void push(node *head, int *n, char *linea) {
    (*n)++;
    node * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = (node *) malloc(sizeof(node));
    current->next->n = *n;
    current->next->linea = strdup(linea);
    current->next->next = NULL;
    
}

void print_list(node *head) {
    node *current = head;
    printf("INICIO DE PRINTFS\n");
    while (current != NULL) {
        printf("#####\n");
        printf("%d\n", current->n);
        printf("%s\n", current->linea);
        printf("#####\n");
        current = current->next;
    }
}
void guardar(node *head, FILE *archivo) {
    node *current = head;
    while (current != NULL) {
        
        fprintf(archivo,"%d " ,current->n);
        fprintf(archivo,"%s\n" ,current->linea);
        current = current->next;
    }
}



// Función para parsear un comando simple sin pipes
void parsearComando(const char *entrada, char *comando, char **argumentos) {
    int numArgs = 0;
    char copiaEntrada[1024];
    strcpy(copiaEntrada, entrada);

    char *token = strtok(copiaEntrada, " ");
    if (token != NULL) {
        strcpy(comando, token);
        argumentos[numArgs] = strdup(comando);  // Usa strdup para evitar problemas de memoria
        numArgs++;
    }

    while (token != NULL) {
        token = strtok(NULL, " ");
        if (token != NULL) {
            argumentos[numArgs] = strdup(token);  // Usa strdup para evitar problemas de memoria
            numArgs++;
        }
    }
    argumentos[numArgs] = NULL;
}

// Función para parsear comandos con pipes
void parsearComandoConPipes(const char *entrada, char **comandos, int *numComandos) {
    char copiaEntrada[1024];
    strcpy(copiaEntrada, entrada);

    char *token = strtok(copiaEntrada, "|");
    *numComandos = 0;

    while (token != NULL) {
        comandos[*numComandos] = strdup(token);  // Usa strdup para evitar problemas de memoria
        (*numComandos)++;
        token = strtok(NULL, "|");
    }
    comandos[*numComandos] = NULL;
}

// Función para ejecutar comandos con pipes
void ejecutarConPipes(char *comandos[], int numComandos) {
    int pipefd[2 * (numComandos - 1)];
    pid_t pid;
    int i;

    for (i = 0; i < (numComandos - 1); i++) {
        if (pipe(pipefd + i * 2) == -1) {
            perror("Error en pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < numComandos; i++) {
        pid = fork();
        if (pid == -1) {
            perror("Error al crear hijo");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            if (i > 0) {
                dup2(pipefd[(i - 1) * 2], STDIN_FILENO);
            }
            if (i < numComandos - 1) {
                dup2(pipefd[i * 2 + 1], STDOUT_FILENO);
            }

            for (int j = 0; j < 2 * (numComandos - 1); j++) {
                close(pipefd[j]);
            }

            char comando[50];
            char *argumentos[MAX_ARGS];
            parsearComando(comandos[i], comando, argumentos);
            execvp(comando, argumentos);
            perror("Error execvp");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < 2 * (numComandos - 1); i++) {
        close(pipefd[i]);
    }

    for (i = 0; i < numComandos; i++) {
        wait(NULL);
    }
}

// Función para ejecutar comandos sin pipes
void procesoconcurrente(char *comando, char **argumentos) {
    pid_t pid = fork();

    if (pid == 0) {
        execvp(comando, argumentos);
        perror("Error execvp");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        perror("Error al crear hijo");
    }
}

// Función principal
int main() {
    node *head = NULL;
    int cant_n=0; 
    char entrada[MAX_CHAR];
    char *comandos[MAX_PIPES + 1];
    int numComandos;

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
            perror("Error fgets");
            continue;
        }
        entrada[strcspn(entrada, "\n")] = '\0';


        if (entrada[0] == '\0') {
            continue;
        }

        if (strcmp(entrada, "exit") == 0) {
            break;
        }
        if (strncmp(entrada, "favs ", 5) == 0) { 
            if(strncmp(entrada+5, "crear ", 6) == 0){
                char *ruta = entrada + 11; // Obtener la ruta después del comando "favs " 

                FILE *archivo = fopen(ruta, "w"); 
                if (archivo != NULL) { 
                    fprintf(archivo, "Este es el archivo de tus comandos favoritos.\n"); 
                    fclose(archivo); 
                    printf("Archivo '%s' creado con éxito.\n", ruta); 
                } else { 
                perror("Error al crear el archivo"); 
                } 
                continue; // Saltar el resto de la iteración del loop 
            }
            //if(strncmp(entrada+5, "otros comandos ", 6) == 0) {}   
        }
        if(cant_n==0){
            head = (node *) malloc(sizeof(node));
            ++cant_n;
            head->n = cant_n;
            head->linea = strdup(entrada);
            head->next= NULL;
        }else push(head, &cant_n, entrada);
        printf("%d\n",cant_n);
        print_list(head);
        
        const char *ruta = "misfavoritos.txt";
        
        if(strcmp(entrada, "favs guardar")==0){
            FILE *archivo = fopen(ruta, "w");
            guardar(head, archivo);
            fclose(archivo);
        }
        
        
        parsearComandoConPipes(entrada, comandos, &numComandos);

        if (numComandos > 1) {
            ejecutarConPipes(comandos, numComandos);
        } else {
            char comando[50];
            char *argumentos[MAX_ARGS];
            parsearComando(entrada, comando, argumentos);
            procesoconcurrente(comando, argumentos);
        }
    }
    
    return 0;
}