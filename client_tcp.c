#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5001
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <IP_del_servidor>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Crear el socket TCP
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // Conectar al servidor
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al conectar con el servidor");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Leer la entrada del usuario
        printf("Ingrese una cadena (o vacío para salir): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        // Salir si la cadena es vacía
        if (strlen(buffer) == 0) break;

        // Enviar el mensaje al servidor
        send(sockfd, buffer, strlen(buffer), 0);

        // Recibir respuesta del servidor
        int n = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (n <= 0) break;

        buffer[n] = '\0';
        printf("Respuesta del servidor: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
