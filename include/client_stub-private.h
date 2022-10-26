#ifndef _CLIENT_STUB_PRIVATE_H
#define _CLIENT_STUB_PRIVATE_H

#include "data.h"
#include "entry.h"

#include <netinet/in.h> //socket struct
/* Estrutura que define a árvore de dados
*/
struct rtree_t {
    
    struct sockaddr_in socket;
    int descriptor;

};
extern struct rtree_t *remoteTree;


#endif