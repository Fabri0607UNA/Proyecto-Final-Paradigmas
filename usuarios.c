#include <stdio.h>
#include <string.h>
#include "usuarios.h"
#include <stdbool.h>

#define MAX_USUARIOS 100
#define MAX_LINEA 256

typedef struct {
    int id;
    char nombre[50];
    char correo[50];
    char contrasena[50];
} usuario;

bool contieneSimbolo1(const char *mensaje) {
    return strchr(mensaje, '|') != NULL;
}

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
        sscanf(linea, "%49[^|]|%49[^|]|%49[^\n]", usuario.nombre, usuario.correo, usuario.contrasena);
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
    usuario nuevoUsuario;

    // Pedir el nombre y validar que no contenga '|'
    printf("Nombre: ");
    scanf("%49s", nuevoUsuario.nombre);
    if (contieneSimbolo1(nuevoUsuario.nombre)) {
        printf("Error: El nombre no puede contener el símbolo '|'.\n");
        return;
    }

    // Pedir el correo y validar que no contenga '|'
    printf("Correo: ");
    scanf("%49s", nuevoUsuario.correo);
    if (contieneSimbolo1(nuevoUsuario.correo)) {
        printf("Error: El correo no puede contener el símbolo '|'.\n");
        return;
    }

    // Validar que el correo sea único
    if (validarCorreoExistente(nuevoUsuario.correo)) {
        printf("Error: El correo ya está registrado.\n");
        return;
    }

    // Pedir la contraseña y validar que no contenga '|'
    printf("Contraseña: ");
    scanf("%49s", nuevoUsuario.contrasena);
    if (contieneSimbolo1(nuevoUsuario.contrasena)) {
        printf("Error: La contraseña no puede contener el símbolo '|'.\n");
        return;
    }

    // Guardar el usuario en el archivo
    FILE *archivo = fopen("usuarios.txt", "a");
    if (!archivo) {
        printf("No se pudo abrir el archivo para escribir.\n");
        return;
    }

    // Escribir los datos del nuevo usuario en el archivo
    fprintf(archivo, "%s|%s|%s\n", nuevoUsuario.nombre, nuevoUsuario.correo, nuevoUsuario.contrasena);
    fclose(archivo);
    
    printf("Usuario creado exitosamente.\n");
}
