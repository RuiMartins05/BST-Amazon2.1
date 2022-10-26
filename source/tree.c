/*
* Grupo SD-050
* João Santos nº 57103
* Paulo Bolinhas nº 56300
* Rui Martins nº 56283
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/tree.h"
#include "../include/tree-private.h"
#include "../include/data.h"
#include "../include/entry.h"

struct tree_t; /* A definir pelo grupo em tree-private.h */

/* Função para criar uma nova árvore tree vazia.
 * Em caso de erro retorna NULL.
 */
struct tree_t *tree_create(){

    struct tree_t* current = (struct tree_t*) malloc(sizeof(struct tree_t));

    if(current == NULL){
        free(current->entry);
        free(current->left_child);
        free(current->right_child);
        free(current);
        return NULL;
    }

    current->entry = NULL;
    current->right_child=NULL;
    current->left_child=NULL;
    current->size = 0;
    return current;

}

/* Função para libertar toda a memória ocupada por uma árvore.
 */
void tree_destroy(struct tree_t *tree){

    if(tree!=NULL){
        if(tree->entry!=NULL) {
            entry_destroy(tree->entry);
            nodes_destroy(tree->right_child);
            nodes_destroy(tree->left_child);
        }
        free(tree);
    }

}

/* Função para libertar a memória associada a uma árvore.
 */
void nodes_destroy(struct tree_t *node) {

  if (node == NULL){
    return;
  }
  nodes_destroy(node->left_child);
  nodes_destroy(node->right_child);
  entry_destroy(node->entry);
  free(node);
}


/* Função para adicionar um par chave-valor à árvore.
 * Os dados de entrada desta função deverão ser copiados, ou seja, a
 * função vai *COPIAR* a key (string) e os dados para um novo espaço de
 * memória que tem de ser reservado. Se a key já existir na árvore,
 * a função tem de substituir a entrada existente pela nova, fazendo
 * a necessária gestão da memória para armazenar os novos dados.
 * Retorna 0 (ok) ou -1 em caso de erro.
 */
int tree_put(struct tree_t *tree, char *key, struct data_t *value){

    char* new_key = (char*) malloc(strlen(key)+1);

    if(new_key == NULL) {
        free(new_key);
        return -1;
    }

    strcpy(new_key, key);
    struct data_t *current_value = data_dup(value);
    struct entry_t *current_entry = entry_create(new_key, current_value);
    
    if (put_aux(tree, current_entry, new_key, tree->size) == -1)
        return -1;
    
    return 0;
}

/* Funçao que insere uma entry e a respetiva key na tree. Retorna 0 ou 1 (ok), -1 em caso de erro.
*/
int put_aux(struct tree_t *tree, struct entry_t *current_entry, char* key, int size) {
    if(tree->entry == NULL){

        tree->entry = current_entry; 
        tree->size = size + 1;
        return 0;

    } else {

        int comparator = entry_compare(tree->entry, current_entry);

        if(comparator == 0){
            entry_replace(tree->entry, current_entry->key, current_entry->value);
            if (tree->size == 1)
                return 0;
            return 1;
        } else if (comparator < 0){
            if (tree->right_child == NULL)
                tree->right_child = tree_create();
            if (put_aux(tree->right_child, current_entry, key, size) == 0)
                tree->size += 1;
            else
                return 1;
            return 0;
        } else {
            if (tree->left_child == NULL) 
                tree->left_child = tree_create();
            if (put_aux(tree->left_child, current_entry, key, size) == 0)
                tree->size+=1;
            else
                return 1;
            return 0;
        } 

    }
    
    entry_destroy(current_entry);
    return -1;

}

/* Função para obter da árvore o valor associado à chave key.
 * A função deve devolver uma cópia dos dados que terão de ser
 * libertados no contexto da função que chamou tree_get, ou seja, a
 * função aloca memória para armazenar uma *CÓPIA* dos dados da árvore,
 * retorna o endereço desta memória com a cópia dos dados, assumindo-se
 * que esta memória será depois libertada pelo programa que chamou
 * a função. Devolve NULL em caso de erro.
 */
struct data_t *tree_get(struct tree_t *tree, char *key){
    char *new_key = (char*) malloc(strlen(key)+1);

    if(new_key == NULL) {
        free(new_key);
        return NULL;
    }

    strcpy(new_key,key);
    struct entry_t *current_entry = entry_create(new_key, NULL);

    struct tree_t* finded_tree = tree_search(tree, current_entry);

    if (finded_tree == NULL) {
        entry_destroy(current_entry);
        return NULL;
    }

    struct data_t *current_data = data_dup(finded_tree->entry->value);
        if (current_data == NULL) {
            entry_destroy(current_entry);
            data_destroy(current_data);
            return NULL;
        }
    entry_destroy(current_entry);
    return current_data;
}

/* Função para obter a árvore associada ao conteúdo da entrada current_entry. 
 * Retorna NULL em caso de erro.
 */
struct tree_t* tree_search(struct tree_t* tree, struct entry_t* current_entry) {

    if (tree == NULL || tree->entry == NULL)
        return NULL;
        
    if (entry_compare(tree->entry, current_entry) == 0) {
        return tree;
    } else if (entry_compare(tree->entry, current_entry) < 0) {
        return tree_search(tree->right_child, current_entry);
    } else {
        return tree_search(tree->left_child, current_entry);
    }
}

/* Função para remover um elemento da árvore, indicado pela chave key,
 * libertando toda a memória alocada na respetiva operação tree_put.
 * Retorna 0 (ok) ou -1 (key not found).
 */
int tree_del(struct tree_t *tree, char *key){
    if(tree->entry == NULL || tree_size(tree) == 0) {
        return -1;
    }

    // char *new_key = (char*) malloc(strlen(key)+1);
    // strcpy(new_key,key);
    struct entry_t *current_entry = entry_create(key, NULL);

    if(tree->size == 1 && entry_compare(tree->entry, current_entry) != 0) {
        entry_destroy(current_entry);
        return -1;
    }

    return delete_aux(tree, current_entry);
}

/* Função para remover o elemento da árvore associado ao conteúdo da entrada current_entry.
 * Retorna 0 (ok), -1 caso contrário.
 */
int delete_aux(struct tree_t* tree, struct entry_t* current_entry) {
    int comparator = entry_compare(tree->entry, current_entry);

    if(comparator < 0) {

        if(tree->right_child == NULL)
            return -1;
        else if (delete_aux(tree->right_child, current_entry)==0) {
            tree->size -= 1;
            return 0;

        }

    } else if(comparator > 0) {

        if(tree->left_child == NULL)
            return -1;
        else if (delete_aux(tree->left_child, current_entry)==0) {
            tree->size -= 1;
            return 0;

        }

    } else {
        //No child
        if(tree->right_child == NULL && tree->left_child == NULL) {
            tree->entry->value = NULL;
            tree->entry = NULL;
            entry_destroy(tree->entry);
            return 0;
        }

        //One child
        else if(tree->right_child == NULL || tree->left_child == NULL) {
            struct tree_t* temp = tree_create();

            if(tree->right_child == NULL) {
                temp = tree->left_child;
                entry_replace(tree->entry, temp->entry->key, temp->entry->value);
            }
            else {
                temp = tree->right_child;
                entry_replace(tree->entry, temp->entry->key, temp->entry->value);
            }
            tree->entry->value = NULL;
            tree->entry = NULL;
            entry_destroy(tree->entry);
            return 0;
        }

        //Two children
        else {
            struct tree_t* temp = tree_create();
            temp = get_min(tree->right_child);
            entry_replace(tree->entry, temp->entry->key, temp->entry->value);
            delete_aux(tree->right_child, temp->entry);
            return 0;  
        }
    }
    return -1;
}

/* Função para obter o menor valor da árvore.
*/
struct tree_t* get_min(struct tree_t* tree) {
    if(tree->entry == NULL)
        return NULL;
    else if(tree->left_child != NULL)
        return get_min(tree->left_child);
    return tree;
}   


/* Função que devolve o número de elementos contidos na árvore.
 */
int tree_size(struct tree_t *tree){

    return tree->size;
}

/* Função que devolve a altura da árvore.
 */
int tree_height(struct tree_t *tree){

    if (tree == NULL) 
        return 0;
    else {
        int left_height = tree_height(tree->left_child);
        int right_height = tree_height(tree->right_child);
        return max(left_height, right_height) + 1;
    }
}

/* Função para obter o máximo dos dois argumentos.
*/
int max(int x, int y){
    if(x > y){
        return x;
    }else{
        return y;
    }
}

/* Função que devolve um array de char* com a cópia de todas as keys da
 * árvore, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária. As keys devem vir ordenadas segundo a ordenação lexicográfica das mesmas.
 */
char **tree_get_keys(struct tree_t *tree) {
    
  if(tree == NULL){
    return NULL;
  }

    int size = tree_size(tree);
    char ** keys = (char**) malloc(sizeof(char*)*(size + 1));

    if (keys == NULL) {
        free(keys);
        return NULL;
    }
        

    for (int i = 0; i < size + 1; i++)
        keys[i] = NULL;
    
    key_put(tree, keys, 0);

    char* temp;
    for (int i = 0; i < size; i++) {
      for (int j = i + 1; j < size; j++) {

         if (strcmp(keys[i], keys[j]) > 0) {
            temp = (char*) malloc(sizeof(keys[i]));
            if(temp == NULL) {
                free(temp);
                return NULL;
            }
            strcpy(temp, keys[i]);
            strcpy(keys[i], keys[j]);
            strcpy(keys[j], temp);
            free(temp);
         }
      }
   }
    return keys;
};

/* Função que forma um array composto pelas keys existentes na àrvore.
 */
void key_put(struct tree_t *node, char **keys, int value){

    int currentPosition = value;

    if (node->left_child != NULL)
        key_put(node->left_child, keys, currentPosition);
    
    if (node->right_child != NULL) 
        key_put(node->right_child, keys, currentPosition);

    if (keys[currentPosition] != NULL) {
        while(keys[currentPosition] != 0) 
            currentPosition +=1;  
    }

    keys[currentPosition] = (char*) malloc(strlen(node->entry->key)+1);
    
    if (keys[currentPosition] == NULL) {
        free(keys);
        return;
    }
        
    
    strcpy(keys[currentPosition], node->entry->key);
}



/* Função que devolve um array de void* com a cópia de todas os values da
* árvore, colocando o último elemento do array com o valor NULL e
* reservando toda a memória necessária.
*/
void **tree_get_values(struct tree_t *tree){
    
    if (tree == NULL)
        return NULL;

    int size = tree_size(tree);
    void** values = (void**) malloc(tree_size(tree) * sizeof(void**) + sizeof(NULL));

    if (values == NULL) {
        free(values);
        return NULL;
    }

    for (int i = 0; i < size + 1; i++)
        values[i] = NULL;

    values_put(tree, values, 0);
    return values;
}

/* Função que forma um array composto pelos values existentes na àrvore.
*/
void values_put(struct tree_t *node, void **values, int value){

    int currentPosition = value;

    if (node->left_child != NULL)
        values_put(node->left_child, values, currentPosition);
    
    if (node->right_child != NULL) 
        values_put(node->right_child, values, currentPosition);

    if (values[currentPosition] != NULL) {
        while(values[currentPosition] != 0) 
            currentPosition +=1;  
    }

    struct data_t* dup = data_dup(node->entry->value);
    values[currentPosition] = dup->data;
}

/* Função que liberta toda a memória alocada por tree_get_keys().
 */
void tree_free_keys(char **keys){
  int counter = 0;
  
  while(keys[counter] != NULL) {
    free(keys[counter]);
    counter++;
    }
    
  free(keys);
}

/* Função que liberta toda a memória alocada por tree_get_values().
 */
void tree_free_values(void **values){
    int counter = 0;
    while(values[counter] != NULL) {
        free(values[counter]);
        counter++;
}

    free(values);
}