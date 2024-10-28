#include <stdio.h>
#include <string.h>
#include "correos.h"
#include "usuarios.h"
#include <stdbool.h>


#define MAX_CORREOS 100
#define MAX_LINEA 256

int contadorCorreos = 0;

bool contieneSimbolo(const char *mensaje) {
    return strchr(mensaje, '|') != NULL;
}

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

void listarUsuarios() {
    FILE *archivo = fopen("usuarios.txt", "r");
    
    if (!archivo) {
        printf("No se pudo abrir el archivo de usuarios.\n");
        return;
    }

    char linea[MAX_LINEA];
    char nombre[50], correo[50], contrasena[50];

    printf("Correos de los usuarios disponibles:\n");
    
    // Leer el archivo línea por línea
    while (fgets(linea, sizeof(linea), archivo)) {
        // Verificar que la línea no esté vacía
        if (strlen(linea) > 1) {
            // Analizar cada línea en los campos nombre, correo y contraseña
            int camposLeidos = sscanf(linea, "%49[^|]|%49[^|]|%49s", nombre, correo, contrasena);
            
            // Verificar que se hayan leído correctamente los tres campos
            if (camposLeidos == 3) {
                // Mostrar solo el correo de cada usuario
                printf("%s\n", correo);
            } else {
                printf("Error al leer la línea: %s\n", linea);
            }
        }
    }

    fclose(archivo);
}


void enviarCorreo(const char *remitente) {

    FILE *archivo1 = fopen("correos.txt", "r");
    
    if (!archivo1) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }

    char linea[MAX_LINEA], destinatario[50], remitente1[50], mensaje[256], estado[10];
    int id = 0;  // Inicializar el id
    Correo nuevoCorreo;

    // Leer el archivo de correos para obtener el último ID usado
    while (fgets(linea, sizeof(linea), archivo1)) {
        sscanf(linea, "%d|%49[^|]|%49[^|]|%255[^|]|%9s", &id, remitente1, destinatario, mensaje, estado);
    }

    fclose(archivo1);
    
    // Verificar si el remitente es válido
    strcpy(nuevoCorreo.remitente, remitente);

    // Verificar si el remitente contiene el símbolo '|'
    if (contieneSimbolo(nuevoCorreo.remitente)) {
        printf("El remitente no puede contener el símbolo '|'.\n");
        return;
    }

    // Mostrar la lista de usuarios para que el remitente sepa a quién puede enviar correos
    listarUsuarios();  // Llama a la función que lista los correos de los usuarios

    // Solicitar el destinatario
    printf("\nIngrese el correo del destinatario: ");
    scanf("%s", nuevoCorreo.destinatario);

    // Verificar si el destinatario existe
    if (!validarDestinatario(nuevoCorreo.destinatario)) {
        printf("El destinatario no existe. Intente de nuevo.\n");
        return;
    }

    // Verificar si el destinatario contiene el símbolo '|'
    if (contieneSimbolo(nuevoCorreo.destinatario)) {
        printf("El destinatario no puede contener el símbolo '|'. Intente de nuevo.\n");
        return;
    }

    // Solicitar el mensaje
    printf("Escriba su mensaje: ");
    getchar();  // Limpiar el buffer
    fgets(nuevoCorreo.mensaje, sizeof(nuevoCorreo.mensaje), stdin);

    // Eliminar el carácter de nueva línea (\n) al final del mensaje si existe
    size_t len = strlen(nuevoCorreo.mensaje);
    if (len > 0 && nuevoCorreo.mensaje[len - 1] == '\n') {
        nuevoCorreo.mensaje[len - 1] = '\0';  // Reemplazar \n con \0
    }

    // Verificar si el mensaje contiene el símbolo '|'
    if (contieneSimbolo(nuevoCorreo.mensaje)) {
        printf("El mensaje no puede contener el símbolo '|'. Intente de nuevo.\n");
        return;  // Salir de la función si el símbolo está presente
    }

    strcpy(nuevoCorreo.estado, "pendiente");

    // Guardar el correo en el archivo
    FILE *archivo = fopen("correos.txt", "a");
    if (!archivo) {
        printf("No se pudo abrir el archivo para escribir.\n");
        return;
    }

    // Asignar un nuevo ID al correo
    nuevoCorreo.id = id + 1;
    fprintf(archivo, "%d|%s|%s|%s|%s\n", nuevoCorreo.id, nuevoCorreo.remitente, nuevoCorreo.destinatario, nuevoCorreo.mensaje, nuevoCorreo.estado);
    fclose(archivo);

    printf("Correo enviado exitosamente.\n");
}

// Función para listar los correos de un usuario específico y actualizar los correos "pendiente" a "leído"
void listarCorreos(const char *usuario) {
    FILE *archivo = fopen("correos.txt", "r");
    FILE *tempArchivo = fopen("temp.txt", "w+");  // Archivo temporal para actualizar los estados

    if (!archivo || !tempArchivo) {
        printf("No se pudo abrir el archivo de correos.\n");
        return;
    }

    char linea[MAX_LINEA];
    int id;
    char remitente[50], destinatario[50], mensaje[256], estado[10];
    int listaCorreos = 0;

    printf("Correos de %s:\n", usuario);

    // Leer línea por línea y actualizar los estados de los correos pendientes
    while (fgets(linea, sizeof(linea), archivo)) {
        sscanf(linea, "%d|%49[^|]|%49[^|]|%255[^|]|%9s", &id, remitente, destinatario, mensaje, estado);

        // Mostrar el correo si el destinatario o remitente coincide con el usuario
        if (strcmp(destinatario, usuario) == 0 || strcmp(remitente, usuario) == 0) {
            printf("ID: %d | De: %s | Para: %s | Mensaje: %s | Estado: %s\n", id, remitente, destinatario, mensaje, estado);
        }

        // Verificar si el correo es para el usuario y está en estado "pendiente"
        if (strcmp(destinatario, usuario) == 0 && strcmp(estado, "pendiente") == 0) {
            strcpy(estado, "leido");  // Cambiar el estado a "leído"
            listaCorreos++;
        }

        // Escribir en el archivo temporal con el estado actualizado, si es necesario
        fprintf(tempArchivo, "%d|%s|%s|%s|%s\n", id, remitente, destinatario, mensaje, estado);
    }

    fclose(archivo);
    fclose(tempArchivo);

    // Reemplazar el archivo original con el archivo temporal actualizado
    remove("correos.txt");
    rename("temp.txt", "correos.txt");

    if (listaCorreos == 0) {
        printf("No tiene correos no leídos.\n");
    } else {
        printf("Se actualizaron %d correos a estado 'leído'.\n", listaCorreos);
    }
}

// Función para listar los correos no leídos de un usuario y actualizarlos como "leído"
void listarCorreosNoLeidos(const char *usuario) {
    FILE *archivo = fopen("correos.txt", "r");
    FILE *tempArchivo = fopen("temp.txt", "w+");  // Archivo temporal para actualizar los estados

    if (!archivo || !tempArchivo) {
        printf("No se pudo abrir el archivo de correos.\n");
        return;
    }

    char linea[MAX_LINEA];
    int id;
    char remitente[50], destinatario[50], mensaje[256], estado[10];
    int correosNoLeidos = 0;  // Contador de correos no leídos

    printf("Correos no leídos de %s:\n", usuario);

    // Leer línea por línea y actualizar los estados de los correos no leídos
    while (fgets(linea, sizeof(linea), archivo)) {
        sscanf(linea, "%d|%49[^|]|%49[^|]|%255[^|]|%9s", &id, remitente, destinatario, mensaje, estado);

        // Verificar si el correo es para el usuario y está en estado "no leido"
        if (strcmp(destinatario, usuario) == 0 && strcmp(estado, "pendiente") == 0) {
            printf("ID: %d | De: %s | Mensaje: %s\n", id, remitente, mensaje);
            strcpy(estado, "leido");  // Actualizar el estado a "leido"
            correosNoLeidos++;
        }

        // Escribir en el archivo temporal, con el estado actualizado si es necesario
        fprintf(tempArchivo, "%d|%s|%s|%s|%s\n", id, remitente, destinatario, mensaje, estado);
    }

    fclose(archivo);
    fclose(tempArchivo);

    // Reemplazar el archivo original con el archivo temporal actualizado
    remove("correos.txt");
    rename("temp.txt", "correos.txt");

    if (correosNoLeidos == 0) {
        printf("No tiene correos no leídos.\n");
    } else {
        printf("Se actualizaron %d correos a estado 'leído'.\n", correosNoLeidos);
    }
}


// Función para eliminar un correo específico
void eliminarCorreo(const char *usuario) {
    FILE *archivo = fopen("correos.txt", "r");
    FILE *tempArchivo = fopen("temp.txt", "w");
    
    if (!archivo || !tempArchivo) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }

    char linea[MAX_LINEA], destinatario[50], remitente[50], mensaje[256], estado[10];
    int id;
    int idAEliminar;
    int eliminarCorreo = 0;

    // Mostrar correos del usuario antes de solicitar el ID a eliminar
    printf("Correos de %s:\n", usuario);
    while (fgets(linea, sizeof(linea), archivo)) {
        sscanf(linea, "%d|%49[^|]|%49[^|]|%255[^|]|%9s", &id, remitente, destinatario, mensaje, estado);

        // Mostrar solo los correos enviados o recibidos por el usuario
        if (strcmp(destinatario, usuario) == 0) {
            printf("ID: %d | De: %s | Para: %s | Mensaje: %s | Estado: %s\n", id, remitente, destinatario, mensaje, estado);
            eliminarCorreo++;
        }
    }

    if (eliminarCorreo == 0) {
        printf("El usuario no tiene correos.\n");
        return;
    }else{

         // Volver a abrir el archivo para leer desde el principio
        rewind(archivo);

        printf("\nIngrese el ID del correo que desea eliminar: ");
        scanf("%d", &idAEliminar);

        int correoEncontrado = 0; // Bandera para verificar si se encontró el correo a eliminar

        // Leer línea por línea y verificar si corresponde al correo a eliminar
        while (fgets(linea, sizeof(linea), archivo)) {
            sscanf(linea, "%d|%49[^|]|%49[^|]|%255[^|]|%9s", &id, remitente, destinatario, mensaje, estado);

            // Copiar al archivo temporal solo si no coincide con el correo a eliminar
            // y el destinatario es el usuario actual
            if (id != idAEliminar || strcmp(destinatario, usuario) != 0) {
                fprintf(tempArchivo, "%d|%s|%s|%s|%s\n", id, remitente, destinatario, mensaje, estado);
            } else {
                correoEncontrado = 1;  // Se encontró y se omitirá en el nuevo archivo
            }
        }

        fclose(archivo);
        fclose(tempArchivo);

        // Verificar si el correo se encontró y eliminó
        if (correoEncontrado) {
            remove("correos.txt");
            rename("temp.txt", "correos.txt");
            printf("Correo eliminado exitosamente.\n");
        } else {
            // Si no se encontró el correo, eliminar el archivo temporal
            remove("temp.txt");
            printf("No se encontró el correo con el ID proporcionado o no tiene permiso para eliminarlo.\n");
        }
            
    }

}


// Función para responder a un correo
void responderCorreo(const char *usuario) {
    FILE *archivo = fopen("correos.txt", "r");
    if (!archivo) {
        printf("No se pudo abrir el archivo de correos.\n");
        return;
    }

    int id, correoId;
    char linea[MAX_LINEA], remitente[50], destinatario[50], mensaje[256], estado[10];
    int responderCorreo = 0;  // Contador de correos a los cuales se puede responder

    // Mostrar lista de correos del usuario antes de pedir el ID
    printf("Correos de %s:\n", usuario);
    while (fgets(linea, sizeof(linea), archivo)) {
        sscanf(linea, "%d|%49[^|]|%49[^|]|%255[^|]|%9s", &id, remitente, destinatario, mensaje, estado);

        // Mostrar solo los correos enviados o recibidos por el usuario
        if (strcmp(destinatario, usuario) == 0 || strcmp(remitente, usuario) == 0) {
            printf("ID: %d | De: %s | Para: %s | Mensaje: %s | Estado: %s\n", id, remitente, destinatario, mensaje, estado);
            responderCorreo++;
        }
    }

    // Verificar si el usuario tiene correos a los cuales puede responder
    if (responderCorreo == 0) {
        printf("No tiene correos a los cuales pueda responder.\n");
        fclose(archivo);
        return;
    }

    // Volver al inicio del archivo para poder buscar el correo a responder
    rewind(archivo);

    printf("\nIngrese el ID del correo que desea responder: ");
    scanf("%d", &correoId);

    // Buscar el correo por ID
    int correoEncontrado = 0; // Bandera para verificar si se encontró el correo
    while (fgets(linea, sizeof(linea), archivo)) {
        sscanf(linea, "%d|%49[^|]|%49[^|]|%255[^|]|%9s", &id, remitente, destinatario, mensaje, estado);

        if (id == correoId) {
            correoEncontrado = 1;  // Correo encontrado
            break;
        }
    }
    fclose(archivo);

    // Verificar si el correo fue encontrado
    if (!correoEncontrado) {
        printf("No se encontró el correo con el ID proporcionado.\n");
        return;
    }

    // Verificar si el correo fue dirigido o enviado por el usuario
    if (strcmp(destinatario, usuario) == 0 || strcmp(remitente, usuario) == 0) {
        Correo respuesta;
        strcpy(respuesta.remitente, usuario);

        // Verificar si el remitente contiene el símbolo '|'
        if (contieneSimbolo(respuesta.remitente)) {
            printf("El remitente no puede contener el símbolo '|'.\n");
            return;
        }

        strcpy(respuesta.destinatario, remitente);  // La respuesta va al remitente del correo original

        // Verificar si el destinatario contiene el símbolo '|'
        if (contieneSimbolo(respuesta.destinatario)) {
            printf("El destinatario no puede contener el símbolo '|'. Intente de nuevo.\n");
            return;
        }

        printf("Escriba su respuesta: ");
        getchar();  // Para evitar problemas con el \n sobrante
        fgets(respuesta.mensaje, sizeof(respuesta.mensaje), stdin);

        // Eliminar el carácter de nueva línea (\n) al final del mensaje si existe
        size_t len = strlen(respuesta.mensaje);
        if (len > 0 && respuesta.mensaje[len - 1] == '\n') {
            respuesta.mensaje[len - 1] = '\0';  // Reemplazar \n con \0
        }

        // Verificar si el mensaje contiene el símbolo '|'
        if (contieneSimbolo(respuesta.mensaje)) {
            printf("El mensaje no puede contener el símbolo '|'. Intente de nuevo.\n");
            return;
        }

        FILE *archivo1 = fopen("correos.txt", "r");
    
        if (!archivo1) {
            printf("No se pudo abrir el archivo.\n");
            return;
        }

        // Leer el archivo de correos para obtener el último ID usado
        while (fgets(linea, sizeof(linea), archivo1)) {
            sscanf(linea, "%d|%49[^|]|%49[^|]|%255[^|]|%9s", &id, remitente, destinatario, mensaje, estado);
        }

        fclose(archivo1);

        // Guardar la respuesta en el archivo
        FILE *archivoRespuesta = fopen("correos.txt", "a");
        if (!archivoRespuesta) {
            printf("No se pudo abrir el archivo para escribir.\n");
            return;
        }

        // Asignar un nuevo ID al correo, incrementando el ID del último correo
        respuesta.id = id + 1;
        strcpy(respuesta.estado, "pendiente");
        fprintf(archivoRespuesta, "%d|%s|%s|%s|%s\n", respuesta.id, respuesta.remitente, respuesta.destinatario, respuesta.mensaje, respuesta.estado);
        fclose(archivoRespuesta);

        printf("Respuesta enviada exitosamente.\n");
    } else {
        printf("No tiene permiso para responder a este correo.\n");
    }
}
