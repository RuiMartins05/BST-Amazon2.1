#include "../include/sdmessage.pb-c.h"
#include "../include/tree.h"

struct tree_t *tree;

/* Inicia o skeleton da árvore.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). 
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int tree_skel_init() {
    
    tree = tree_create();

    if(tree == NULL){
        return -1;
    }
    return 0;

}

/* Liberta toda a memória e recursos alocados pela função tree_skel_init.
 */
void tree_skel_destroy(){

    tree_destroy(tree);
}


/* Executa uma operação na árvore (indicada pelo opcode contido em msg)
 * e utiliza a mesma estrutura message_t para devolver o resultado.
 * Retorna 0 (OK) ou -1 (erro, por exemplo, árvore nao incializada)
*/
int invoke(struct _MessageT *msg){

    if(msg == NULL || tree == NULL)
        return -1;

    if(msg->opcode == MESSAGE_T__OPCODE__OP_PUT){
        
        struct data_t *data = data_create2(msg->entry->data->datasize, msg->entry->data->data);

        if(msg->c_type == MESSAGE_T__C_TYPE__CT_ENTRY){
            if(msg->entry->data->data == NULL) {
                errorCase(msg);
                return -1;
            }
        }

        if(tree_put(tree,msg->entry->key,data) == -1 ){
            errorCase(msg);
            free(data); //destruir data
            return 0;
        }

        msg->opcode = MESSAGE_T__OPCODE__OP_PUT + 1;
        msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
        free(data); //destruir data

        return 0;

    } else if (msg->opcode == MESSAGE_T__OPCODE__OP_GET){

        struct data_t *data = tree_get(tree,msg->entry->key);

        if(msg->c_type = MESSAGE_T__C_TYPE__CT_KEY){
            if(msg->entry->key == NULL){
                errorCase(msg);
                return -1;
            }
        }

        if(data == NULL){
            errorCase(msg);
            msg->entry->data->data = NULL;
            msg->entry->data->datasize = 0;
            data_destroy(data); //destruir data->data e data->datasize
            return 0;
        }


        msg->opcode = MESSAGE_T__OPCODE__OP_GET + 1;
        msg->c_type = MESSAGE_T__C_TYPE__CT_VALUE;
        msg->entry->data->data = strdup(data->data); //duplicar o data de modo a dar free a este
        msg->entry->data->datasize = data->datasize;
        data_destroy(data); //destruir data->data e data->datasize

        return 0;

    } else if (msg->opcode == MESSAGE_T__OPCODE__OP_DEL){

        if(msg->c_type == MESSAGE_T__C_TYPE__CT_KEY){

            if (tree_del(tree, msg->entry->key) == -1) {
                errorCase(msg);
                return 0;
            }
            
        }

        msg->opcode = MESSAGE_T__OPCODE__OP_DEL + 1;
        msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;

        return 0;

    } else if (msg->opcode == MESSAGE_T__OPCODE__OP_SIZE){

        if(msg->c_type == MESSAGE_T__C_TYPE__CT_NONE){

            int size = tree_size(tree);
            msg->opcode = MESSAGE_T__OPCODE__OP_SIZE + 1;
            msg->c_type = MESSAGE_T__C_TYPE__CT_RESULT;
            msg->size = size;
            return 0;
        }

    
    } else if (msg->opcode == MESSAGE_T__OPCODE__OP_HEIGHT){

        if(msg->c_type == MESSAGE_T__C_TYPE__CT_NONE){
            
            msg->height = tree_height(tree);
            msg->opcode = MESSAGE_T__OPCODE__OP_HEIGHT + 1;
            msg->c_type = MESSAGE_T__C_TYPE__CT_RESULT;
            return 0;
        }

    } else if (msg->opcode == MESSAGE_T__OPCODE__OP_GETKEYS){
        if (msg->c_type == MESSAGE_T__C_TYPE__CT_NONE) {
            msg->opcode = MESSAGE_T__OPCODE__OP_GETKEYS + 1;
            msg->c_type = MESSAGE_T__C_TYPE__CT_KEYS;
            msg->n_data = tree_size(tree);
            msg->data = tree_get_keys(tree);

            if(msg->n_data > 0) {
                //TODO
            }
        }
        return 0;

    } else if (msg->opcode == MESSAGE_T__OPCODE__OP_GETVALUES){
        //TO DO
        return 0;

    } else if(msg->opcode == MESSAGE_T__OPCODE__OP_ERROR) {
        return -1;
    }    
    
    return -1;

}

void errorCase (MessageT *msg){
    msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
    msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
}
