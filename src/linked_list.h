#ifndef E2FAE041_BEB5_410D_BC5A_EFC154CCDB19
#define E2FAE041_BEB5_410D_BC5A_EFC154CCDB19

#include <stdlib.h>

#define OK 1
#define NOK 0

typedef struct linked_list_node_t
{
    struct linked_list_node_t *previous;
    struct linked_list_node_t *next;
    void *data;
} linked_list_node;

/*
 * Generic Linked List Type: gll_t
 */
typedef linked_list_node *linked_list_node_ptr;
typedef struct
{
    int length;
    linked_list_node_ptr head;
    linked_list_node_ptr tail;
} linked_list;

typedef linked_list *linked_list_ptr;

linked_list_ptr linked_list_initialize()
{
    linked_list_ptr list = (linked_list_ptr)malloc(sizeof(linked_list));
    list->length = 0;
    list->head = list->tail = NULL;
    return list;
}

linked_list_node_ptr linked_list_node_create(void *data)
{
    linked_list_node_ptr node = (linked_list_node_ptr)malloc(sizeof(linked_list_node));
    node->data = data;
    node->previous = node->next = NULL;
    return node;
}

int linked_list_push_front(linked_list_ptr list, void *data)
{
    linked_list_node_ptr newNode = linked_list_node_create(data);

    if (list->length == 0)
    {
        list->tail = newNode;
    }
    else
    {
        list->head->previous = newNode;
        newNode->next = list->head;
    }
    list->head = newNode;
    list->length++;
    return OK;
}

int linked_list_push_back(linked_list_ptr list, void *data)
{
    linked_list_node_ptr newNode = linked_list_node_create(data);

    /* if list is empty */
    if (list->length == 0)
    {
        list->tail = newNode;
        list->head = newNode;
        list->length++;
        return OK;
    }
    list->tail->next = newNode;
    newNode->previous = list->tail;
    list->tail = newNode;
    list->length++;
    return OK;
}

linked_list_node_ptr find_by_name(linked_list_ptr list, int (*predicate)(void *, char *), char *name_to_find)
{
    linked_list_node_ptr current = list->head;
    while (current != NULL)
    {
        if (predicate(current->data, name_to_find))
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int linked_list_remove_by_name(linked_list_ptr list, int (*predicate)(void *, char *), char *name_to_find, void (*free_data)(void *))
{
    linked_list_node_ptr current = find_by_name(list, predicate, name_to_find);
    if (current == NULL)
    {
        return NOK;
    }
    if (current->previous == NULL)
    {
        list->head = current->next;
    }
    else
    {
        current->previous->next = current->next;
    }
    list->length--;
    free_data(current->data);
    free(current);
    return OK;
}

linked_list_node_ptr find_at_position(linked_list_ptr list, int position)
{
    if (position < 0 || position >= list->length)
    {
        return NULL;
    }
    linked_list_node_ptr current = list->head;
    for (int i = 0; i < position; i++)
    {
        current = current->next;
    }
    return current;
}

int linked_list_push_at_position(linked_list_ptr list, void *data, int position)
{
   if(position < 0 || position > list->length){
        return NOK;
    }

    linked_list_node_ptr newNode = linked_list_node_create(data);
    if(position == 0){
        linked_list_push_front(list, data);
        return OK;
    }
    if(position == list->length){
        linked_list_push_back(list, data);
        return OK;
    }
    linked_list_node_ptr current = find_at_position(list, position);
    if(current == NULL){
        return NOK;
    }

    newNode->next = current;
    newNode->previous = current->previous;
    current->previous->next = newNode;
    current->previous = newNode;
    list->length++;
    return OK;

}

int linked_list_remove_at_position(linked_list_ptr list, int position, void (*free_data)(void *))
{
    if (position < 0 || position >= list->length)
    {
        return NOK;
    }
    linked_list_node_ptr current = find_at_position(list, position);
    if (current == NULL)
    {
        return NOK;
    }

    if (current->previous == NULL)
    {
        list->head = current->next;
    }
    else
    {
        current->previous->next = current->next;
    }

    if (current->next == NULL)
    {
        list->tail = current->previous;
    }
    else
    {
        current->next->previous = current->previous;
    }
    list->length--;
    free_data(current->data);
    free(current);
    return OK;
}

int linked_list_pop_front(linked_list_ptr list, void (*free_data)(void *))
{
  return linked_list_remove_at_position(list, 0, free_data);
}

int linked_list_destroy(linked_list_ptr list, void (*free_data)(void *))
{
    int destroyed_nodes = 0;
    linked_list_node_ptr current = list->head;
    linked_list_node_ptr next;

    while (current != NULL)
    {
        next = current->next;
        free_data(current->data);
        free(current);
        current = next;
        destroyed_nodes++;
    }
    free(list);
    return destroyed_nodes;
}

#endif /* E2FAE041_BEB5_410D_BC5A_EFC154CCDB19 */
