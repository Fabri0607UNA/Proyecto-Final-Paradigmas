#include <stdio.h>
#include <string.h>
#include "correos.h"
#include "usuarios.h"

#define MAX_CORREOS 100
#define MAX_LINEA 256

int contadorCorreos = 0;

typedef struct {
    int id;
    char remitente[50];
    char destinatario[50];
    char mensaje[256];
    char estado[10];
} correo;

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
        sscanf(linea, "%49[^|]|%49[^|]|%49[^\n]", usuario.nombre, usuario.correo, usuario.contrasena);
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
    nuevoCorreo.id = ++contadorCorreos;  // Asigna un ID único
    fprintf(archivo, "%d|%s|%s|%s|%s\n", nuevoCorreo.id, nuevoCorreo.remitente, nuevoCorreo.destinatario, nuevoCorreo.mensaje, nuevoCorreo.estado);
    fclose(archivo);
    printf("Correo enviado exitosamente.\n");
}

// Listar los correos de un usuario específico
void listarCorreos(const char *usuario) {
    FILE *archivo = fopen("correos.txt", "r+");  // Abrir en modo lectura y escritura
    if (!archivo) {
        printf("No se pudo abrir el archivo de correos.\n");
        return;
    }
    char linea[MAX_LINEA];
    
    printf("Correos de %s:\n", usuario);
    while (fgets(linea, sizeof(linea), archivo)) {
        int id;
        char remitente[50], destinatario[50], mensaje[256], estado[10];
        long posicion = ftell(archivo);  // Guarda la posición actual

        // Analiza la línea con sscanf, incluyendo el id al principio
        sscanf(linea, "%d|%49[^|]|%49[^|]|%255[^|]|%9s", &id, remitente, destinatario, mensaje, estado);

        if (strcmp(destinatario, usuario) == 0 || strcmp(remitente, usuario) == 0) {
            printf("ID: %d | De: %s | Para: %s | Mensaje: %s | Estado: %s\n", id, remitente, destinatario, mensaje, estado);

            // Si el estado es "no leido", cambiarlo a "leido"
            if (strcmp(estado, "no leido") == 0) {
                // Calcular la posición del estado en la línea actual
                fseek(archivo, posicion - strlen(linea) + strlen(linea) - strlen(estado) - 1, SEEK_SET);  // Mover el puntero al inicio del estado
                fprintf(archivo, "leido");  // Sobrescribir el estado con "leido"
                fflush(archivo);  // Asegurarse de que los cambios se escriban en el archivo
            }
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
        int id;
        char remitente[50], destinatario[50], mensaje[256], estado[10];
        
        // Leer el id y los otros campos
        sscanf(linea, "%d|%49[^|]|%49[^|]|%255[^|]|%9s", &id, remitente, destinatario, mensaje, estado);

        // Filtrar por destinatario y estado "no leido"
        if (strcmp(destinatario, usuario) == 0 && strcmp(estado, "no leido") == 0) {
            printf("ID: %d | De: %s | Mensaje: %s\n", id, remitente, mensaje);
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
    int id;
    char correoAEliminar[50];
    
    printf("Ingrese el remitente del correo a eliminar: ");
    scanf("%s", correoAEliminar);

    // Leer línea por línea y verificar si corresponde al correo a eliminar
    while (fgets(linea, sizeof(linea), archivo)) {
        sscanf(linea, "%d|%49[^|]|%49[^|]|%255[^|]|%9s", &id, remitente, destinatario, mensaje, estado);
        
        // Copiar al archivo temporal solo si no coincide con el correo a eliminar
        if (!(strcmp(destinatario, usuario) == 0 && strcmp(remitente, correoAEliminar) == 0)) {
            fprintf(tempArchivo, "%d|%s|%s|%s|%s\n", id, remitente, destinatario, mensaje, estado);
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

    // Buscar el correo por ID
    while (fgets(linea, sizeof(linea), archivo)) {
        sscanf(linea, "%d|%49[^|]|%49[^|]|%255[^|]|%9s", &id, remitente, destinatario, mensaje, estado);

        if (id == correoId) {
            break;  // Correo encontrado
        }
    }
    fclose(archivo);

    // Verificar si el correo fue dirigido o enviado por el usuario
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
        // Se debe asignar un nuevo ID al correo
        static int nuevoID = 1;  // O manejarlo de alguna otra forma si el ID es global
        fprintf(archivoRespuesta, "%d|%s|%s|%s|%s\n", nuevoID++, respuesta.remitente, respuesta.destinatario, respuesta.mensaje, respuesta.estado);
        fclose(archivoRespuesta);

        printf("Respuesta enviada exitosamente.\n");
    } else {
        printf("No tiene permiso para responder a este correo.\n");
    }
}
