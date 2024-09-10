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

int existeEnLista(node *head, char *linea) {
    node *current = head;
    while (current != NULL) {
        if (strcmp(current->linea, linea) == 0) {
            return 1; // Ya existe
        }
        current = current->next;
    }
    return 0; // No existe
}

void push(node **head, int *n, char *linea) {
    if (existeEnLista(*head, linea)) {
        printf("El comando '%s' ya está en la lista, no se guardará de nuevo.\n", linea);
        return; // No se guarda si ya existe
    }
    (*n)++;
    if (*head == NULL) {
        *head = (node *)malloc(sizeof(node));
        (*head)->n = *n;
        (*head)->linea = strdup(linea);
        (*head)->next = NULL;
        return;
    }
    node * current = *head;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = (node *) malloc(sizeof(node));
    current->next->n = *n;
    current->next->linea = strdup(linea);
    current->next->next = NULL;
    
}

void print_list(node **head) {
    node *current = *head;
    printf("INICIO DE PRINTFS\n");
    while (current != NULL) {
        printf("#####\n");
        printf("%d\n", current->n);
        printf("%s\n", current->linea);
        printf("#####\n");
        current = current->next;
    }
}
void guardar(node **head, FILE *archivo) {
    node *current = *head;
    while (current != NULL) {
        
        fprintf(archivo,"%d " ,current->n);
        fprintf(archivo,"%s\n" ,current->linea);
        current = current->next;
    }
}
void cargar(node **head, FILE *archivo, int *n) {
    int numero;
    char linea[200];
    while (fgets(linea, sizeof(linea), archivo)) {
        if (sscanf(linea, "%d %[^\n]", &numero, linea) == 2) {
            *n = numero - 1;
            push(head, n, linea);
        }
    }
}

void borrar(node **head, int *n) {
    node *current = *head;
    node *next_node;

    while (current != NULL) {
        next_node = current->next;  // Guarda el siguiente nodo
        free(current->linea);       // Libera la memoria de la cadena
        free(current);              // Libera la memoria del nodo actual
        current = next_node;        // Avanza al siguiente nodo
    }
    *n = 0;
    *head = NULL;
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

void set_recordatorio(int seconds, const char *message)
{
    pid_t pid = fork();
    if (pid == 0)
    { // Proceso hijo
        sleep(seconds);
        printf("\a\n¡RECORDATORIO!: %s\n", message);
    }
    else if (pid > 0)
    {
        // Proceso padre no espera al hijo
        // Así la shell sigue funcionando
        printf("Recordatorio configurado en %d segundos: %s\n", seconds, message);
    }
    else
    {
        perror("Error al crear proceso hijo");
    }
}
int main() {
    node *head = NULL;
    int cant_n=0; 
    char entrada[MAX_CHAR];
    char *comandos[MAX_PIPES + 1];
    int numComandos;
    char *guardado = malloc(MAX_CHAR * sizeof(char));
    char *ruta = "ruta.txt";
    FILE *r_archivo = fopen(ruta,"r");
    fscanf(r_archivo, "%s",guardado);
    fclose(r_archivo);

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
        if (strncmp(entrada, "set recordatorio", 16) == 0)
        {
            char *resto = entrada + 16;
            int seconds;
            char message[MAX_CHAR];

            sscanf(resto, "%d \"%[^\"]\"", &seconds, message);

            set_recordatorio(seconds, message);
            continue;
        }
        if (strncmp(entrada, "favs eliminar ", 14) == 0) {
    char *buscar = entrada + 14;  // Donde empiezan los números a eliminar
    int pos = 0;
    int eliminados = 0;  // Llevar cuenta de cuántos nodos ya se eliminaron

    while (*buscar) {
        pos = 0;

        // Calcula el número del string
        while (*buscar >= '0' && *buscar <= '9') {
            pos = pos * 10 + (*buscar - '0');
            buscar++;
        }

        // Ajustar la posición a eliminar considerando cuántos nodos ya se eliminaron
        int posAjustada = pos - eliminados;

        // Eliminar el nodo en la posición 'posAjustada'
        if (posAjustada == 1) {
            // Si se quiere eliminar el primer nodo
            node *temp = head;
            if (temp != NULL) {
                head = head->next;
                free(temp->linea);
                free(temp);
                cant_n--;
                eliminados++;
            }
        } else {
            // Eliminar un nodo en una posición intermedia
            node *eliminar = head;
            for (int i = 1; eliminar != NULL && i < posAjustada - 1; i++) {
                eliminar = eliminar->next;
            }

            if (eliminar != NULL && eliminar->next != NULL) {
                node *nodo_a_eliminar = eliminar->next;
                eliminar->next = nodo_a_eliminar->next;
                free(nodo_a_eliminar->linea);
                free(nodo_a_eliminar);
                cant_n--;
                eliminados++;
            }
        }

        // Avanza sobre las comas y cualquier espacio en blanco después
        while (*buscar == ',' || *buscar == ' ') {
            buscar++;
        }
    }

    // Actualiza los números de los nodos
    node *temp = head;
    int nuevo_n = 1;
    while (temp != NULL) {
        temp->n = nuevo_n++;
        temp = temp->next;
    }
    continue;
}


        
        
        
        parsearComandoConPipes(entrada, comandos, &numComandos);

        if (numComandos > 1) {
            ejecutarConPipes(comandos, numComandos);
            push(&head, &cant_n, entrada);
        } else {
            char comando[50];
            char *argumentos[MAX_ARGS];
            parsearComando(entrada, comando, argumentos);
            
            if (strcmp(comando, "favs") == 0 && argumentos[1] != NULL) {
            if (argumentos[1] != NULL) {}
                if (strcmp(argumentos[1], "crear") == 0) {
                    printf("Comando: favs crear\n");
                    guardado = argumentos[2];
                    FILE *r_archivo = fopen(ruta, "w");
                    fprintf(r_archivo, "%s",guardado);
                    fclose(r_archivo);
                    FILE *archivo = fopen(guardado, "w"); 
                    if (archivo != NULL) { 
                        fprintf(archivo, "Este es el archivo de tus comandos favoritos.\n"); 
                        fclose(archivo); 
                        printf("Archivo '%s' creado con éxito.\n", guardado); 
                    } else { 
                        perror("Error al crear el archivo"); 
                        } 
                continue; // Saltar el resto de la iteración del loop 
                } else if (strcmp(argumentos[1], "guardar") == 0) {
                    printf("Comando: favs guardar\n");
                    FILE *archivo = fopen(guardado, "w");
                    guardar(&head, archivo);
                    fclose(archivo);
                    continue;
                } else if (strcmp(argumentos[1], "cargar") == 0) {
                    printf("Comando: favs cargar\n");
                    borrar(&head, &cant_n);
                    FILE *archivo = fopen(guardado, "r");
                    cargar(&head, archivo, &cant_n);
                    fclose(archivo);
                    continue;
                }else if (strcmp(argumentos[1], "mostrar") == 0) {
                    printf("Comando: favs mostrar\n");
                    printf("%d\n",cant_n);
                    print_list(&head);
                    continue;
                    
                }else if (strcmp(argumentos[1], "borrar") == 0) {
                    printf("Comando: favs borrar\n");
                    borrar(&head, &cant_n);
                    continue;
                }else if (strcmp(argumentos[1], "buscar")==0){
                    printf("Comando: favs buscar\n");
                    char *dato = argumentos[2];
                    int contador = 0;
                    node *buscar = head;
                    while (buscar != NULL) {
                        if(strstr(buscar->linea,dato) !=NULL){
                        printf("%d\n", buscar->n);
                        printf("%s\n", buscar->linea);
                        contador++;
                        }
                    buscar = buscar->next;           
                }
                    if(contador==0)printf("No hay comandos cmd\n");   
                    continue;
                }else if(argumentos[2] != NULL){
                    int num;    
                    num = atoi(argumentos[1]);
                    if(strcmp(argumentos[2], "ejecutar")==0){
                        node *buscar = head;
                        int contador = 1;

                        while(buscar != NULL && contador<num){
                            buscar = buscar->next;
                            contador++;
                        }
                        if(num==contador && buscar!=NULL){
                            strcpy(entrada,buscar->linea); //Como se ejecuto un codigo de agrego+1 a cant_n;
                            parsearComandoConPipes(entrada, comandos, &numComandos);
                            if (numComandos > 1) {
                                ejecutarConPipes(comandos, numComandos);
                            }else{
                                parsearComando(entrada, comando, argumentos);
                                procesoconcurrente(comando, argumentos);
                            }
                            continue;
                        }else {
                            printf("No hay comando %d \n",num);
                            continue;
                    }
                    }
                }

            }

            if(strstr(comando,"favs")==NULL){
                push(&head, &cant_n, entrada);
                procesoconcurrente(comando, argumentos);
            }
        }
    }
    return 0;
}
