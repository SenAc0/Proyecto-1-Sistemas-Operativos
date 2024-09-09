# Proyecto-1-Sistemas-Operativos
![Shell](https://img.shields.io/badge/shell-linux-blue.svg) ![C](https://img.shields.io/badge/language-C-orange.svg)

MiShell es un intérprete de comandos (shell) simple escrito en C que soporta la ejecución de comandos con pipes, procesos concurrentes y permite la gestión de comandos favoritos.

## Características

- **Ejecución de comandos**: Soporte para ejecutar comandos y programas con múltiples argumentos.
- **Pipes**: Ejecución de comandos con pipes (`|`), permitiendo la comunicación entre procesos.
- **Favoritos (`favs`)**: Gestión de comandos favoritos con opciones para crear, guardar, cargar, mostrar, buscar y eliminar comandos.
- **Recordatorios (`set recordatorio`)**: Configuración de recordatorios que se activan después de un tiempo determinado.
- **Manejo de errores**: Respuesta adecuada ante comandos no válidos.
- **Comando `exit`**: Salida segura de la shell.

## Compilación

Para compilar el proyecto, ejecuta el siguiente comando en la terminal:

```bash
gcc -o mishell mishell.c
