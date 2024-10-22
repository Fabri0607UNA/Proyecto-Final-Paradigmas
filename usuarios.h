#ifndef USUARIOS_H
#define USUARIOS_H

typedef struct {
    char nombre[50];
    char correo[50];
    char contrasena[50];
} Usuario;

int validarCorreoExistente(const char *correo);
void crearUsuario();

#endif
