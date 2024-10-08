#ifndef CORREOS_H
#define CORREOS_H

typedef struct {
    int id;
    char remitente[50];
    char destinatario[50];
    char mensaje[256];
    char estado[10];
} Correo;

extern int contadorCorreos;


void enviarCorreo(const char *remitente);
void listarCorreos(const char *usuario);
void listarCorreosNoLeidos(const char *usuario);
void eliminarCorreo(const char *usuario);
void responderCorreo(const char *usuario);

#endif