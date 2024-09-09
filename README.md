# Proyecto-1-Sistemas-Operativos
![C](https://img.shields.io/badge/language-C-orange.svg)

Es un intérprete de comandos (shell) simple escrito en C que soporta la ejecución de comandos con pipes, procesos concurrentes, permite la gestión de comandos favoritos y también la capacidad de crear recordatorios.

## Características

### Ejecución de Comandos
- **Soporte para múltiples argumentos:** Permite ejecutar comandos con uno o varios argumentos de forma sencilla, similar a una shell estándar.
- **Manejo de errores:** La shell maneja errores de entrada de manera adecuada, proporcionando mensajes claros cuando un comando no es reconocido o no puede ser ejecutado.

### Pipes
- **Comunicación entre procesos:** Ejecución de comandos con pipes (`|`), permitiendo la comunicación entre procesos.

### Gestión de Comandos Favoritos (`favs`)
- **Crear:** Permite crear un archivo donde se almacenan los comandos favoritos dado una ruta y nombre de archivo.
- **Mostrar:** Despliega en pantalla la lista de comandos favoritos con sus respectivos números asociados.
- **Eliminar:** Elimina comandos de la lista de favoritos utilizando sus números asociados.
- **Buscar:** Permite buscar un comando dentro de la lista de favoritos utilizando un substring, luego lo despliega en pantalla junto con su número asociado.
- **Borrar:** Borra completamente la lista de comandos favoritos.
- **Ejecutar:** Ejecuta directamente un comando favorito utilizando su número asociado en la lista.
- **Cargar:** Lee comandos desde un archivo de texto de favoritos y los carga en memoria.
- **Guardar:** Guarda los comandos actuales en un archivo de texto para que puedan ser reutilizados en sesiones futuras.

### Recordatorios (`set recordatorio`)
- **Configuración de recordatorios:** Permite al usuario establecer recordatorios con un mensaje específico que se activan después de unos segundos determinados. Estos recordatorios se ejecutan en segundo plano, permitiendo continuar usando la shell.

### Comando `exit`
- **Salida segura:** Proporciona una manera segura de salir de la shell, asegurando que todos los procesos en ejecución se manejen correctamente.

## Compilación

Para compilar, ejecuta el siguiente comando en la terminal:

```bash
gcc -o shell shell.c
```

## Ejecución

```bash
./shell
```

## Ejemplos de uso

```bash
favs crear /home/usuario/favoritos.txt
favs mostrar
favs eliminar 1,3
favs buscar ls
favs 2 ejecutar
set recordatorio 10 "ir a comprar"
exit
```
