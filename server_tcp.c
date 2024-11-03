#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

#define PORT 5001
#define BUFFER_SIZE 1024

void invertir_cadena(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; ++i) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

int contiene_numeros(const char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if (isdigit((unsigned char)str[i])) {
            return 1; // La cadena contiene números
        }
    }
    return 0; // La cadena no contiene números
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <nombre_del_servidor>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *server_name = argv[1];
    int server_fd, new_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t client_len = sizeof(client_addr);

    // Crear el socket TCP
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Vincular el socket al puerto
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al vincular");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones entrantes
    if (listen(server_fd, 3) < 0) {
        perror("Error al escuchar");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor %s escuchando en el puerto %d\n", server_name, PORT);

    while (1) {
        // Aceptar una conexión
        if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("Error al aceptar conexión");
            continue;
        }

        while (1) {
            // Recibir mensaje del cliente
            int n = recv(new_socket, buffer, BUFFER_SIZE, 0);
            if (n <= 0) {
                printf("Conexión cerrada o error de recepción\n");
                close(new_socket);
                break;
            }
            buffer[n] = '\0';

            // Verificar si la cadena está vacía después de eliminar espacios en blanco
            if (strlen(buffer) == 0 || buffer[0] == '\n') {
                printf("Cadena vacía recibida, cerrando conexión\n");
                close(new_socket);
                break;
            }

            // Verificar si la cadena contiene números
            if (contiene_numeros(buffer)) {
                strcpy(buffer, "Error: El mensaje no debe contener números. Intente nuevamente.");
            } else {
                // Invertir el mensaje y agregar el nombre del servidor
                invertir_cadena(buffer);
                strcat(buffer, " - ");
                strcat(buffer, server_name);
            }

            // Enviar respuesta al cliente
            send(new_socket, buffer, strlen(buffer), 0);
            printf("Mensaje enviado: %s\n", buffer);
        }
    }

    close(server_fd);
    return 0;
}

