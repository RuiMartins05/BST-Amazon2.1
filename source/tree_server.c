#include <stdio.h>
#include <string.h>
#include "../include/network_server.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("ERRO MAIN SERVER - Eh necessario passar o numero de porto");
        return -1;
    }
    
    short port = (short)atoi(argv[1]);

    int sockfd = network_server_init(port);
    tree_skel_init();
    
    network_main_loop(sockfd);

    tree_skel_destroy();
    network_server_close();

}