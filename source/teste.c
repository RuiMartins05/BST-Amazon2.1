#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    

    char line[150];
    int res;
    while (1) {

        printf("INSIRA UMA ACAO\n");
        fgets(line, 150, stdin);
        printf("%s\n", line);

        char *action;
        action = strtok(line, " ");
        char *key = strtok(NULL, " ");
        char *data = strtok(NULL, "\n");

        if (action == NULL) {
            printf("NULL");
            return 0;
        }

        printf("%s\n", action);
        printf("%s\n", key);
        printf("%s\n", data);
        break;
        // char *key = strtok(NULL, " ");
        // char *data = strtok(NULL, " ");

        // if (strcmp(action, "size")) {
        //     printf("SIZE");
        //     break;
        // } else {
        //     break;
        // }
    }
    return 0;
}