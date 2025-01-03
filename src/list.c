#include "list.h"
#include <stdlib.h>
#include <string.h>

list_t init_list(){
    const int initial_size = 10;
    list_t list = (list_t){
        .size = 0,
        .allocated_size = initial_size,
        .elements = malloc(sizeof(void*) * initial_size)
    };
    return list;
}

void list_append(list_t* list, void* element){
    if (list->size+1 == list->allocated_size){
        list->allocated_size *= 2;
        list->elements = realloc(list->elements, sizeof(void*) * list->allocated_size);
    }

    list->elements[list->size] = element;
    list->size++;
}

// TODO : list remove ?

void list_destroy(list_t list){
    free(list.elements);
}