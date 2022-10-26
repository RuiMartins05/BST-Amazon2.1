#include <stdio.h>
#include <string.h>

#include "../include/client_stub.h"
#include "../include/client_stub-private.h"

struct rtree_t* remoteTree;

int main(int argc, char *argv[]) {

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

    if (remoteTree == NULL)
        return NULL;
    
    int result = 0;

    char line[150];

    while (result == 0) {

        fgets(line, 150, stdin);

        char action[50] = strtok(action, "\n");
        char key[50] = strtok(NULL, "\n");
        char data[50] = strtok(NULL, "\n");

        if (strcmp(action, "size")) {

        } else if (strcmp(action, "height")) {
            
        } else if (strcmp(action, "del")) { 

        } else if (strcmp(action, "get")) {
            
        } else if (strcmp(action, "put")) {
            
        } else if (strcmp(action, "getkeys")) {
            
        } else if (strcmp(action, "quit")) {
            result = -1;
        }
    }

    free(remoteTree);
    return 0;
}
