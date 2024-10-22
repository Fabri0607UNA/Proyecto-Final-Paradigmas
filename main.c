#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usuarios.h"
#include "correos.h"

#define MAX_LINEA 256

// Prototipo de la función de inicio de sesión
int iniciarSesion(char *correo, char *contrasena);

int main() {
    int opcionInicial, opcionMenu;
    char correo[50], contrasena[50];

    do {
        printf("\n--- Bienvenido al Sistema de Correos ---\n");
        printf("1. Iniciar Sesión\n");
        printf("2. Crear Nueva Cuenta\n");
        printf("3. Salir\n");
        printf("Seleccione una opción: ");

        if (scanf("%d", &opcionInicial) != 1) {
            printf("Entrada no válida. Por favor, ingrese un número.\n");
            while (getchar() != '\n'); // Limpiar el buffer de entrada
            continue;
        }

        switch(opcionInicial) {
        case 1:
            printf("Ingrese su correo: ");
            scanf("%s", correo);
            printf("Ingrese su contraseña: ");
            scanf("%s", contrasena);
            if (iniciarSesion(correo, contrasena)) {
                printf("¡Inicio de sesión exitoso!\n");
                do {
                    printf("\n--- Menú Principal ---\n");
                    printf("1. Enviar un correo nuevo\n");
                    printf("2. Ver listado de correos\n");
                    printf("3. Ver correos no leídos\n");
                    printf("4. Eliminar un correo\n");
                    printf("5. Responder a un correo\n");
                    printf("6. Cerrar sesión\n");
                    printf("Seleccione una opción: ");
                    scanf("%d", &opcionMenu);
                    switch(opcionMenu) {
                        case 1:
                            enviarCorreo(correo); // Utiliza el correo del usuario autenticado como remitente
                            break;
                        case 2:
                            listarCorreos(correo); // Listar correos del usuario autenticado
                            break;
                        case 3:
                            listarCorreosNoLeidos(correo); // Mostrar correos no leídos
                            break;
                        case 4:
                            eliminarCorreo(correo); // Eliminar un correo específico
                            break;
                        case 5:
                            responderCorreo(correo); // Responder a un correo recibido
                            break;
                        case 6:
                            printf("Cerrando sesión...\n");
                            break;
                        default:
                            printf("Opción no válida. Intente de nuevo.\n");
                    }
                } while(opcionMenu != 6);
            } else {
                printf("Correo o contraseña incorrectos. Inténtelo nuevamente.\n");
            }
            break;
        case 2:
            crearUsuario(); // Función que crea un nuevo usuario en el sistema
            break;
        case 3:
            printf("Saliendo del programa...\n");
            break;
        default:
            printf("Opción no válida. Intente de nuevo.\n");
        }
        
    } while(opcionInicial != 3);

    return 0;
}

// Función para validar el inicio de sesión
int iniciarSesion(char *correo, char *contrasena) {
    FILE *archivo = fopen("usuarios.txt", "r");
    if (!archivo) {
        printf("No se pudo abrir el archivo de usuarios.\n");
        return 0;
    }
    char linea[MAX_LINEA];
    Usuario usuario;
    while (fgets(linea, sizeof(linea), archivo)) {
        sscanf(linea, "%49[^|]|%49[^|]|%49[^\n]", usuario.nombre, usuario.correo, usuario.contrasena);
        if (strcmp(usuario.correo, correo) == 0 && strcmp(usuario.contrasena, contrasena) == 0) {
            fclose(archivo);
            return 1; // Inicio de sesión exitoso
        }
    }
    fclose(archivo);
    return 0; // Correo o contraseña incorrectos
}
