#include <stdio.h>
#include <string.h>

#include "../include/client_stub.h"
#include "../include/client_stub-private.h"
#include "../include/entry.h"

struct rtree_t* remoteTree;

int main(int argc, char *argv[]) {

    if(argc != 2) {
        printf("Sintaxe do comando de execução errada. Forma correta: ./tree_client <server>:<port>");
        return -1;
    }
    printf("------CLIENT INTERFACE------\n");
    
    printf("Available Actions\n");
    printf("size - returns tree size\n");
    printf("height - returns tree height\n");
    printf("del <key> - delete node with key = key\n");
    printf("get <key> - get node with key = key\n");
    printf("put <key> <data> - put node with key and data in the tree\n");
    printf("getkeys - get all the keys from the tree\n");
    printf("quit - stops the program\n\n");

    remoteTree = rtree_connect(argv[1]);

    if (remoteTree == NULL) {
        printf("erro ao connectar ao servidor");
        return -1;
    }

    char line[150];
    int res;
    char *key; 
    char *data;

    while (1) {

        printf("INSIRA UMA ACCAO:\n");
        fgets(line, 150, stdin);

        char *action = strtok(line, " ");

        if (strcmp(action, "size")) {
            
            res = rtree_size(remoteTree);
            if (res >= 0)
                printf("Tamanho da árvore %s\n", res);
            else
                printf("Tamanho da árvore negativo\n");

        } else if (strcmp(action, "height")) {

            res = rtree_height(remoteTree);
            if (res >= 0)
                printf("Altura da árvore %s\n", res);
            else
                printf("Altura da árvore negativo\n");

        } else if (strcmp(action, "del")) { 
            
            key = strtok(NULL, " ");

            res = rtree_del(remoteTree, key);
            if (res == 0)
                printf("Key removida com sucesso\n");
            else
                printf("Key not found ou outros problemas\n");

        } else if (strcmp(action, "get")) {
            
            key = strtok(NULL, " ");
            
            struct data_t *dataRes = rtree_get(remoteTree, key);
            if (dataRes != NULL)
                printf("Estrutura data_t obitda com sucesso\n");//maybe print struct
            else
                printf("Estrutura data_t nula\n");

        } else if (strcmp(action, "put")) {

            key = strtok(NULL, " ");
            data = strtok(NULL, "\n");

            struct entry_t *entryToPut = entry_create(key, data);
            res = rtree_put(remoteTree, entryToPut);

            if (res == 0)
                printf("Entry adicionada ou substituida\n");
            else
                printf("Entry nao colocada com sucesso\n");

        } else if (strcmp(action, "getkeys")) {

            char** keysRes = rtree_get_keys(remoteTree);

            if (keysRes != NULL) {
                printf("Keys obtidas com sucesso\n");
                printf("Keys: \n %s\n", keysRes[0]);
            } else {
                printf("Keys nao obtidas com sucesso\n");
            }
            
        } else if (strcmp(action, "getvalues")) {

            char** valuesRes = rtree_get_values(remoteTree);

            if (valuesRes != NULL) {
                printf("Values obtidos com sucesso\n");
                printf("Values: \n %s\n", valuesRes[0]);
            } else {
                printf("Values nao obtidos com sucesso\n");
            }

        } else if (strcmp(action, "quit")) {
            rtree_disconnect(remoteTree);
            return 0;
        }
    }
}
