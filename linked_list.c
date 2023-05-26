#include <stdlib.h>
#include <stdio.h>

#include <omp.h>
#include <float.h>
#include "linked_list.h"
//I want to represent the elements in a linked list due to the ability to move elements in O(N). Since we will do lots of moves i think this way of representing the data is the fastest
typedef struct Node{
    double data;
    struct Node *prev;
    struct Node *next;
} node_t;

int nLLs;
node_t **root;
node_t **last;
int *sizes;


int getSize(int listSelect){
    if(listSelect <= 0){
        return sizes[listSelect];
    }
    int size = 0;
    for(int i = 0; i < nLLs; i++)
        size += sizes[i];
    return size;
}

void addLL(int listSelect, double newObj){

    if(root[listSelect] == NULL){

        root[listSelect] = malloc(sizeof(node_t));
        root[listSelect] = &((node_t){newObj, NULL, root[listSelect]});
        
        last[listSelect] = malloc(sizeof(node_t));
        last[listSelect] = &((node_t){newObj, root[listSelect], NULL});
        
        sizes[listSelect] = 1;
        return;
    }

    node_t *nNode = malloc(sizeof(node_t));
    nNode = &((node_t){newObj, last[listSelect], NULL});
    last[listSelect]->next = nNode;
    last[listSelect] = nNode;
    sizes[listSelect]++;
    
    //THE LARGEST AND SMALLEST VARIABLES ARE ONLY USED FOR GRAPHICS!!
    if(newObj > largest)
        largest = newObj;
    if(newObj < smallest)
        smallest = newObj;
    
}

node_t* findLL(int listSelect, double elem){
    node_t *curr = root[listSelect];
    while(curr != last[listSelect]){
        if(curr->data == elem)
            return curr;
        curr = curr->next;
    }
    return NULL;
 
}

int swapLL(int listSelect, double elem1, double elem2){
    node_t *first;
    node_t *second;
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            first = findLL(listSelect, elem1);
        }
        #pragma omp section
        {
            second = findLL(listSelect, elem2);
        }
    }
    
    if(first == NULL || second == NULL)
        return 0;
    node_t *tmpN = first->next;
    node_t *tmpP = first->prev;
    //Change pointers of swapped elements
    first->prev = second->prev;
    first->next = second->next;
    
    second->prev = tmpP;
    second->next = tmpN;
    
    //Change pointers of elements linked to swapped elements
    first->prev->next = first;
    first->next->prev = first;
    
    second->prev->next = second;
    second->next->prev = first;
    
    return 1;
}

double getLL(int listSelect, int index){

    node_t *tmp = root[listSelect];
    while(index > 0 && tmp != NULL){
        tmp = tmp->next;
        index--;
    }
    //printf("\n\nMADE IT HERE!!!\n\n");

    if(tmp == NULL){
        printf("ERROR: Accessed index out of bounds!");
        return 0.0;
    }
    
    printf("\n\n%f\n\n", tmp->data);

    
    return tmp->data;
}

void destroyLL(){
    for(int i = 0; i < nLLs; i++){
        free(root[i]);
        free(last[i]);
    }
    free(root);
    free(last);
    free(sizes);
}


void setupLL(int numLLs){
    root = malloc(sizeof(node_t*) * numLLs);
    last = malloc(sizeof(node_t*) * numLLs);
    sizes = malloc(sizeof(int) * numLLs);

    nLLs = numLLs;
    
    for(int i = 0; i < numLLs; i++){
        root[i] = NULL;
        last[i] = NULL;
    }
    
    largest = -DBL_MAX;
    smallest = DBL_MAX;

    
}
