#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/data.h"
#include "../include/entry.h"
#include "../include/client_stub.h"
#include "../include/client_stub-private.h"
#include "../include/network_client.h"
#include "../sdmessage.pb-c.h"

#include <sys/socket.h>
#include <arpa/inet.h>

/* Função para estabelecer uma associação entre o cliente e o servidor, 
 * em que address_port é uma string no formato <hostname>:<port>.
 * Retorna NULL em caso de erro.
 */
struct rtree_t *rtree_connect(const char *address_port) {

    char* hostname = strtok((char *) address_port, ":");
    int port = atoi(strtok(NULL, ":"));

    struct rtree_t *remoteTree = (struct rtree_t *) malloc (sizeof(struct rtree_t));

    if (remoteTree == NULL) {
        printf("remote tree == NULL");
        free(remoteTree);
        return NULL;
    }

    // remoteTree->socket = (struct sockaddr_in *) malloc (sizeof(struct sockaddr_in));
    remoteTree->socket.sin_family = AF_INET;
    remoteTree->socket.sin_port = htons(port);
    
    int sinAdressResult = inet_pton(AF_INET, hostname, &remoteTree->socket.sin_addr);

    if (sinAdressResult == -1){
        printf("AF INET nao contem um endereco valido");
        return NULL;
    } else if (sinAdressResult == 0) {
        printf("hostname nao contem um endereco valido");
        return NULL;
    }

    int connectResult = network_connect(remoteTree);
    if (connectResult == -1) {
        printf("network_connnect falhou");
        free(remoteTree);
        return NULL;
    } 

    //Maybe SIGINT aqui

    return remoteTree;
    
}

/* Termina a associação entre o cliente e o servidor, fechando a 
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtree_disconnect(struct rtree_t *rtree) {
    
    if (network_close(rtree) == -1) {
        printf("network_close == -1");
        return -1;
    }

    free(rtree);

    return 0;

}

/* Função para adicionar um elemento na árvore.
 * Se a key já existe, vai substituir essa entrada pelos novos dados.
 * Devolve 0 (ok, em adição/substituição) ou -1 (problemas).
 */
int rtree_put(struct rtree_t *rtree, struct entry_t *entry){
    
    MessageT msg;  
    message_t__init(&msg);

    message_t__entry_t__init(msg.entry);

    msg.opcode = MESSAGE_T__OPCODE__OP_PUT; 
    msg.c_type = MESSAGE_T__C_TYPE__CT_ENTRY; 

    msg.entry->data->datasize = entry->value->datasize;

    msg.entry->data->data = strdup(entry->value->data);
    msg.entry->key = strdup(entry->key);

    MessageT *msgReceived = network_send_receive(rtree, &msg);
    if(msgReceived == NULL){
        return -1;
    }
    
    entry_destroy(msg.entry);

    if(msgReceived->opcode == MESSAGE_T__OPCODE__OP_PUT + 1){
        message_t__free_unpacked(msgReceived, NULL);
        return 0;
    }
    else{
        message_t__free_unpacked(msgReceived, NULL);
        return -1;
    }

}
    

/* Função para obter um elemento da árvore.
 * Em caso de erro, devolve NULL.
 */
struct data_t *rtree_get(struct rtree_t *rtree, char *key) {

    MessageT msg;
    message_t__init(&msg);

    msg.opcode = MESSAGE_T__OPCODE__OP_GET; 
    msg.c_type = MESSAGE_T__C_TYPE__CT_KEY;

    msg.entry->key = key;

    MessageT *msgReceived = network_send_receive(rtree, &msg);
    if(msgReceived  == NULL) {
        return NULL;
    }

    struct data_t *dataResult = NULL;
    if(msgReceived->opcode == MESSAGE_T__OPCODE__OP_GET+1 && msgReceived->c_type == MESSAGE_T__C_TYPE__CT_VALUE){
        
        if(msgReceived->entry->data->data == NULL || strlen(msgReceived->entry->data->data) == 0){
            dataResult = data_create2(msgReceived->entry->data->datasize, NULL); 
        }
        else{
            dataResult = data_create2(msgReceived->entry->data->datasize, strdup(msgReceived->entry->data->data)); 
        }
    }

    else{

        message_t__free_unpacked(msgReceived, NULL);
        return data_create2(0,NULL);
    }
    
    message_t__free_unpacked(msgReceived, NULL);

    return dataResult;
}

/* Função para remover um elemento da árvore. Vai libertar 
 * toda a memoria alocada na respetiva operação rtree_put().
 * Devolve: 0 (ok), -1 (key not found ou problemas).
 */
int rtree_del(struct rtree_t *rtree, char *key){

    MessageT msg;
    message_t__init(&msg);

    msg.opcode = MESSAGE_T__OPCODE__OP_DEL; 
    msg.c_type = MESSAGE_T__C_TYPE__CT_KEY; 

    msg.entry->key = key; 

    MessageT *msgReceived = network_send_receive(rtree, &msg);
    if(msgReceived == NULL){
        return -1;
    }

    if(msgReceived->opcode == MESSAGE_T__OPCODE__OP_DEL+1 && msgReceived->c_type == MESSAGE_T__C_TYPE__CT_NONE){
        message_t__free_unpacked(msgReceived, NULL);
        return 0;
    }

    //caso erro
    message_t__free_unpacked(msgReceived, NULL);
    return -1;
}

/* Devolve o número de elementos contidos na árvore.
 */
int rtree_size(struct rtree_t *rtree) {

    MessageT msg;
    message_t__init(&msg);

    msg.opcode = MESSAGE_T__OPCODE__OP_SIZE; 
    msg.c_type = MESSAGE_T__C_TYPE__CT_NONE; 

    struct _MessageT *msgReceived = network_send_receive(rtree, &msg);
    if(msgReceived == NULL){
        return -1;
    }

    if(msgReceived->opcode == MESSAGE_T__OPCODE__OP_SIZE + 1 && msgReceived->c_type == MESSAGE_T__C_TYPE__CT_RESULT){

        message_t__free_unpacked(msgReceived, NULL);
        return msgReceived->size;
    }  

    //caso erro
    message_t__free_unpacked(msgReceived, NULL);
    return -1;
}

/* Função que devolve a altura da árvore.
 */
int rtree_height(struct rtree_t *rtree) {

    MessageT msg;
    message_t__init(&msg);

    msg.opcode = MESSAGE_T__OPCODE__OP_HEIGHT; 
    msg.c_type = MESSAGE_T__C_TYPE__CT_NONE; 

    struct _MessageT *msgReceived = network_send_receive(rtree, &msg);
    if(msgReceived == NULL){
        return -1;
    }

    if(msgReceived->opcode == MESSAGE_T__OPCODE__OP_HEIGHT + 1 && msgReceived->c_type == MESSAGE_T__C_TYPE__CT_RESULT){

        message_t__free_unpacked(msgReceived, NULL);
        return msgReceived->height;
    }  

    //caso erro
    message_t__free_unpacked(msgReceived, NULL);
    return -1;
}

/* Devolve um array de char* com a cópia de todas as keys da árvore,
 * colocando um último elemento a NULL.
 */
char **rtree_get_keys(struct rtree_t *rtree);

/* Devolve um array de void* com a cópia de todas os values da árvore,
 * colocando um último elemento a NULL.
 */
void **rtree_get_values(struct rtree_t *rtree);
