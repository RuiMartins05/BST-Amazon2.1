#include <stdio.h>
#include <stdlib.h>

#include "../include/client_stub.h"
#include "../include/client_stub-private.h"
#include"../include/message-private.h"

#include "sdmessage.pb-c.h"

/* Esta função deve:
 * - Obter o enrdeeço do servidor (struct sockaddr_in) a base da
 *   informação guardada na estrutura rtree;
 * - Estabelecer a ligação com o servidor;
 * - Guardar toda a informação necessária (e.g., descritor do socket)
 *   na estrutura rtree;
 * - Retornar 0 (OK) ou -1 (erro).
 */
int network_connect(struct rtree_t *rtree) {

    if(rtree == NULL) {
        printf("rtree == NULL");
        return -1;
    }

    rtree->descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (rtree->descriptor == -1) {
        printf("descriptor == -1");
        return -1;
    }

    if (connect(rtree->descriptor, (struct sockaddr*) &rtree->socket, sizeof(rtree->socket)) == -1) {
        printf("connnect == -1");
        close(rtree->descriptor);
        return -1;
    }

    printf("Conexão Estabelecida");

    //SGINAL SIGPIPE, temos de criar uma funcao que trata do exit e notificacao

    return 0;
}

/* Esta função deve:
 * - Obter o descritor da ligação (socket) da estrutura rtree_t;
 * - Serializar a mensagem contida em msg;
 * - Enviar a mensagem serializada para o servidor;
 * - Esperar a resposta do servidor;
 * - De-serializar a mensagem de resposta;
 * - Retornar a mensagem de-serializada ou NULL em caso de erro.
 */
struct MessageT *network_send_receive(struct rtree_t * rtree, struct MessageT *msg) {

    if(rtree == NULL)
        printf("rtree == NULL");
    
    if(msg == NULL)
        printf("msg == NULL");

    int descriptor = rtree->descriptor;

    int sizeRead = message_t__get_packed_size(&msg);
    int sizeNetWork = htonl(sizeRead); //converter host bytes to network bytes

    char* buff = malloc(sizeRead);
    if(buff == NULL)
        printf("buff == NULL");

    message_t__pack(msg, buff);

    if(write_all(descriptor, buff, sizeof(int)) <= 0){
        printf("write_all(descriptor, &sizeRead, sizeof(int)) < 0");
        free(buff);
        return NULL;
    }

    if(write_all(descriptor, buff, sizeRead) <= 0){
        printf("write_all(descriptor, buff, len) < 0");
        free(buff);
        return NULL;
    }

    if (read_all(descriptor, buff, sizeof(int)) <= 0) {
        printf("Erro network_receive() - Erro ao receber dados do cliente");
        free(buff);
        close(descriptor);
        return NULL;
    }

    sizeRead = ntohl(sizeRead);

    if (read_all(descriptor, buff, sizeRead) <= 0) {
        printf("Erro network_receive() - Erro ao receber dados do cliente");
        free(buff);
        close(descriptor);
        return NULL;
    }

    MessageT *retMsg = message_t__unpack(NULL, sizeRead, buff);

    // message_t__free_unpacked(msg, NULL);
    free(buff);

    return retMsg;

}

/* A função network_close() fecha a ligação estabelecida por
 * network_connect().
 */
int network_close(struct rtree_t * rtree) {
    return close(rtree->descriptor);
}

