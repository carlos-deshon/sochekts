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
        if (isdigit(str[i])) {
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
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t client_len = sizeof(client_addr);

    // Crear el socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Vincular el socket al puerto
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al vincular");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor %s escuchando en el puerto %d\n", server_name, PORT);

    while (1) {
        // Recibir mensaje del cliente
        int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&client_addr, &client_len);
        buffer[n] = '\0';

        if (strlen(buffer) == 0) break;  // Terminar si la cadena es vacía

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
        sendto(sockfd, buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *)&client_addr, client_len);
        printf("Mensaje enviado: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}

