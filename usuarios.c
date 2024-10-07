#include <stdio.h>
#include <string.h>
#include "usuarios.h"

#define MAX_USUARIOS 100
#define MAX_LINEA 256

typedef struct {
    char nombre[50];
    char correo[50];
    char contrasena[50];
} usuario;

// Validar si el correo existe en el archivo usuarios.txt
int validarCorreoExistente(const char *correo) {
    FILE *archivo = fopen("usuarios.txt", "r");
    if (!archivo) {
        printf("No se pudo abrir el archivo de usuarios.\n");
        return 0;
    }
    char linea[MAX_LINEA];
    while (fgets(linea, sizeof(linea), archivo)) {
        Usuario usuario;
        sscanf(linea, "%49[^,],%49[^,],%49[^\n]", usuario.nombre, usuario.correo, usuario.contrasena);
        if (strcmp(usuario.correo, correo) == 0) {
            fclose(archivo);
            return 1; // El correo ya existe
        }
    }
    fclose(archivo);
    return 0; // El correo no existe
}

// Crear un nuevo usuario y agregarlo al archivo
void crearUsuario() {
    Usuario nuevoUsuario;
    printf("Nombre: ");
    scanf("%s", nuevoUsuario.nombre);
    printf("Correo: ");
    scanf("%s", nuevoUsuario.correo);

    // Validar que el correo sea único
    if (validarCorreoExistente(nuevoUsuario.correo)) {
        printf("Error: El correo ya está registrado.\n");
        return;
    }

    printf("Contraseña: ");
    scanf("%s", nuevoUsuario.contrasena);

    // Guardar el usuario en el archivo
    FILE *archivo = fopen("usuarios.txt", "a");
    if (!archivo) {
        printf("No se pudo abrir el archivo para escribir.\n");
        return;
    }
    fprintf(archivo, "%s,%s,%s\n", nuevoUsuario.nombre, nuevoUsuario.correo, nuevoUsuario.contrasena);
    fclose(archivo);
    printf("Usuario creado exitosamente.\n");
}
