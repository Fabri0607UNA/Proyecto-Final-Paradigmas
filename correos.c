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

// Listar los correos no leídos de un usuario
void listarCorreosNoLeidos(const char *usuario) {
    FILE *archivo = fopen("correos.txt", "r");
    if (!archivo) {
        printf("No se pudo abrir el archivo de correos.\n");
        return;
    }
    char linea[MAX_LINEA];
    printf("Correos no leídos de %s:\n", usuario);
    while (fgets(linea, sizeof(linea), archivo)) {
        char remitente[50], destinatario[50], mensaje[256], estado[10];
        sscanf(linea, "%49[^,],%49[^,],%255[^,],%9[^\n]", remitente, destinatario, mensaje, estado);
        if (strcmp(destinatario, usuario) == 0 && strcmp(estado, "no leido") == 0) {
            printf("De: %s | Mensaje: %s\n", remitente, mensaje);
        }
    }
    fclose(archivo);
}

// Eliminar un correo
void eliminarCorreo(const char *usuario) {
    FILE *archivo = fopen("correos.txt", "r");
    FILE *tempArchivo = fopen("temp.txt", "w");
    if (!archivo || !tempArchivo) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
    char linea[MAX_LINEA], destinatario[50], remitente[50], mensaje[256], estado[10];
    char correoAEliminar[50];
    
    printf("Ingrese el remitente del correo a eliminar: ");
    scanf("%s", correoAEliminar);

    while (fgets(linea, sizeof(linea), archivo)) {
        sscanf(linea, "%49[^,],%49[^,],%255[^,],%9[^\n]", remitente, destinatario, mensaje, estado);
        if (!(strcmp(destinatario, usuario) == 0 && strcmp(remitente, correoAEliminar) == 0)) {
            fprintf(tempArchivo, "%s", linea); // Copiar todo excepto el correo a eliminar
        }
    }

    fclose(archivo);
    fclose(tempArchivo);
    remove("correos.txt");
    rename("temp.txt", "correos.txt");
    printf("Correo eliminado exitosamente.\n");
}

// Responder un correo
void responderCorreo(const char *usuario) {
    FILE *archivo = fopen("correos.txt", "r");
    if (!archivo) {
        printf("No se pudo abrir el archivo de correos.\n");
        return;
    }

    int id, correoId;
    char linea[MAX_LINEA], remitente[50], destinatario[50], mensaje[256], estado[10];

    printf("Ingrese el ID del correo que desea responder: ");
    scanf("%d", &correoId);

    // Buscar el correo al que desea responder
    int i = 1;
    while (fgets(linea, sizeof(linea), archivo)) {
        if (i == correoId) {
            sscanf(linea, "%49[^,],%49[^,],%255[^,],%9[^\n]", remitente, destinatario, mensaje, estado);
            break;
        }
        i++;
    }
    fclose(archivo);

    // Enviar la respuesta
    if (strcmp(destinatario, usuario) == 0 || strcmp(remitente, usuario) == 0) {
        Correo respuesta;
        strcpy(respuesta.remitente, usuario);
        strcpy(respuesta.destinatario, remitente);  // La respuesta va al remitente del correo original
        printf("Escriba su respuesta: ");
        getchar();  // Para evitar problemas con el \n
        fgets(respuesta.mensaje, sizeof(respuesta.mensaje), stdin);
        strcpy(respuesta.estado, "no leido");

        // Guardar la respuesta en el archivo
        FILE *archivoRespuesta = fopen("correos.txt", "a");
        if (!archivoRespuesta) {
            printf("No se pudo abrir el archivo para escribir.\n");
            return;
        }
        fprintf(archivoRespuesta, "%s,%s,%s,%s\n", respuesta.remitente, respuesta.destinatario, respuesta.mensaje, respuesta.estado);
        fclose(archivoRespuesta);

        printf("Respuesta enviada exitosamente.\n");
    } else {
        printf("No tiene permiso para responder a este correo.\n");
    }
}
