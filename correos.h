#ifndef CORREOS_H
#define CORREOS_H

typedef struct {
    int id;
} Correo;

void enviarCorreo(const char *remitente);
void listarCorreos(const char *usuario);
void listarCorreosNoLeidos(const char *usuario);
void eliminarCorreo(const char *usuario);
void responderCorreo(const char *usuario);

#endif
