#ifndef E2FAE041_BEB5_410D_BC5A_EFC154CCDB19
#define E2FAE041_BEB5_410D_BC5A_EFC154CCDB19

#include <stdlib.h>

#define C_OK 0
#define C_NOK -1


typedef struct linked_list_node_t {
  struct linked_list_node_t *previous;
  struct linked_list_node_t *next;
  void *data;
} linked_list_node;

/*
 * Generic Linked List Type: gll_t
 */
typedef linked_list_node* linked_list_node_ptr;
typedef struct {
  int length;
  linked_list_node_ptr head;
  linked_list_node_ptr tail;
} linked_list;

typedef linked_list* linked_list_ptr;


linked_list_ptr linked_list_initialize(){
  linked_list_ptr list = (linked_list_ptr) malloc(sizeof(linked_list));
  list->length = 0;
  list->head = list->tail = NULL;
  return list;
}

linked_list_node_ptr linked_list_node_create(void* data){
    linked_list_node_ptr node = (linked_list_node_ptr) malloc(sizeof(linked_list_node));
    node->data = data;
    node->previous = node->next = NULL;
    return node;
}


int linked_list_push(linked_list_ptr list, void *data) 
{
  linked_list_node_ptr newNode = linked_list_node_create(data); 

  /* if list is empty */
  if(list->length == 0) {
    list->tail = newNode;
  } else {
    /* if there is at least one element */
    list->head->previous = newNode;
    newNode->next = list->head;
  }
  list->head = newNode;
  list->length++;
  return C_OK;
}

#endif /* E2FAE041_BEB5_410D_BC5A_EFC154CCDB19 */
