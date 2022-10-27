#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../include/tree_skel.h"
#include "../include/network_server.h"
#include "../sdmessage.pb-c.h"

int sockfd;
struct sockaddr_in serverSocket;
int countBufLen;

/* Função para preparar uma socket de receção de pedidos de ligação
 * num determinado porto.
 * Retornar descritor do socket (OK) ou -1 (erro).
 */
int network_server_init(short port) {

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // SOCK_STREAM MEANS IT'S TCP
        printf("ERRO network_server_init(): Erro ao criar socket");
        return -1;
    }

    serverSocket.sin_family = AF_INET;
    serverSocket.sin_port = htons(port);
    serverSocket.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind assigns the adress specified by the descriptor
    if (bind(sockfd, (struct sockaddr *)&serverSocket, sizeof(serverSocket)) < 0) {
        printf("ERRO network_server_init() : Erro no bind");
        close(sockfd);
        return -1;
    }

    // listen marks the socket as a passive one that is waiting to accept a incoming connection
    if (listen(sockfd, 0) < 0) {
        printf("ERRO network_server_init() : Erro no listen");
        close(sockfd);
        return -1;
    };

    return sockfd;
}

/* Esta função deve:
 * - Aceitar uma conexão de um cliente;
 * - Receber uma mensagem usando a função network_receive;
 * - Entregar a mensagem de-serializada ao skeleton para ser processada;
 * - Esperar a resposta do skeleton;
 * - Enviar a resposta ao cliente usando a função network_send.
 */
int network_main_loop(int listening_socket) {

    int connsockfd; // descriptor of the socket connected with the client socket
    struct sockaddr_in client;
    socklen_t size_client;

    MessageT *message = NULL;

    // accept creates a connection with the first request in the listening socket
    while ((connsockfd = accept(sockfd, (struct sockaddr *)&client, &size_client)) != -1) {

        message = network_receive(connsockfd); // passar a client socket

        if (message == NULL)
            continue;

        if (invoke(message) == -1)
            continue;

        if (network_send(connsockfd, message) == -1)
            continue;
    }

    close(connsockfd);
    message_t__free_unpacked(message, NULL);
    return 0;
}

/* Esta função deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura message_t.
 */
MessageT *network_receive(int client_socket) {

    int sizeRead;
    char* buffer = malloc(sizeRead);

    if (read_all(client_socket, &sizeRead, sizeof(int)) <= 0) {
        printf("Erro network_receive() - Erro ao receber dados do cliente");
        free(buffer);
        close(client_socket);
        return NULL;
    }

    sizeRead = ntohl(sizeRead); // convert network-byte order to host-byte order

    MessageT *msg;

    message_t__init(msg);

    if (read_all(client_socket, buffer, sizeRead) <= 0) {
        printf("Erro network_receive() - Erro ao receber dados do cliente");
        free(buffer);
        close(client_socket);
        return NULL;
    }

    msg = message_t__unpack(NULL, sizeRead, buffer);

    return msg;
}

/* Esta função deve:
 * - Serializar a mensagem de resposta contida em msg;
 * - Libertar a memória ocupada por esta mensagem;
 * - Enviar a mensagem serializada, através do client_socket.
 */
int network_send(int client_socket, MessageT *msg) {

    int sizeRead = message_t__get_packed_size(msg);
    char* buffer = malloc(sizeRead);

    message_t__pack(msg, buffer);
    int sizeNetWork = htonl(sizeRead);

    if (write_all(client_socket, &sizeNetWork, sizeof(int)) <= 0){
        printf("Erro network_send() - Erro ao receber dados do cliente");
        close(client_socket);
        free(buffer);
        return NULL;
    }

    // Envia tamanho da string ao cliente pelo socket referente a conexão
    if (write_all(client_socket, buffer, sizeRead) <= 0){ // not sure about == sizeRead
        printf("ERRO network_send() - Erro ao enviar resposta ao cliente");
        close(client_socket);
        free(buffer);
        return -1;
    }

    free(buffer);
    return 0;
}

/* A função network_server_close() liberta os recursos alocados por
 * network_server_init().
 */
int network_server_close(){
    close(sockfd);
    return 0;
}
