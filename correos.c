#include <stdio.h>
#include <string.h>
#include "correos.h"
#include "usuarios.h"

#define MAX_CORREOS 100
#define MAX_LINEA 256

typedef struct {
    char remitente[50];
    char destinatario[50];
    char mensaje[256];
    char estado[10];
} Correo;

// Validar si el destinatario existe en el archivo de usuarios
int validarDestinatario(const char *destinatario) {
    FILE *archivo = fopen("usuarios.txt", "r");
    if (!archivo) {
        printf("No se pudo abrir el archivo de usuarios.\n");
        return 0;
    }
    char linea[MAX_LINEA];
    while (fgets(linea, sizeof(linea), archivo)) {
        Usuario usuario;
        sscanf(linea, "%49[^,],%49[^,],%49[^\n]", usuario.nombre, usuario.correo, usuario.contrasena);
        if (strcmp(usuario.correo, destinatario) == 0) {
            fclose(archivo);
            return 1; // El destinatario existe
        }
    }
    fclose(archivo);
    return 0; // El destinatario no existe
}

// Enviar un nuevo correo
void enviarCorreo(const char *remitente) {
    Correo nuevoCorreo;
    strcpy(nuevoCorreo.remitente, remitente);
    printf("Destinatario: ");
    scanf("%s", nuevoCorreo.destinatario);

    // Validar que el destinatario exista
    if (!validarDestinatario(nuevoCorreo.destinatario)) {
        printf("Error: El destinatario no existe.\n");
        return;
    }

    printf("Mensaje: ");
    getchar(); // Para capturar el \n
    fgets(nuevoCorreo.mensaje, sizeof(nuevoCorreo.mensaje), stdin);
    strcpy(nuevoCorreo.estado, "no leido");

    // Guardar el correo en el archivo
    FILE *archivo = fopen("correos.txt", "a");
    if (!archivo) {
        printf("No se pudo abrir el archivo para escribir.\n");
        return;
    }
    fprintf(archivo, "%s,%s,%s,%s\n", nuevoCorreo.remitente, nuevoCorreo.destinatario, nuevoCorreo.mensaje, nuevoCorreo.estado);
    fclose(archivo);
    printf("Correo enviado exitosamente.\n");
}

// Listar los correos de un usuario específico
void listarCorreos(const char *usuario) {
    FILE *archivo = fopen("correos.txt", "r");
    if (!archivo) {
        printf("No se pudo abrir el archivo de correos.\n");
        return;
    }
    char linea[MAX_LINEA];
    printf("Correos de %s:\n", usuario);
    while (fgets(linea, sizeof(linea), archivo)) {
        char remitente[50], destinatario[50], mensaje[256], estado[10];
        sscanf(linea, "%49[^,],%49[^,],%255[^,],%9[^\n]", remitente, destinatario, mensaje, estado);
        if (strcmp(destinatario, usuario) == 0 || strcmp(remitente, usuario) == 0) {
            printf("De: %s | Para: %s | Mensaje: %s | Estado: %s\n", remitente, destinatario, mensaje, estado);
        }
    }
    fclose(archivo);
}
