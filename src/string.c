#include "string.h"
#include <stdlib.h>

string_t init_string(){
    const int initial_size = 10;
    string_t list = (string_t){
        .length = 0,
        .allocated_size = initial_size,
        .str = malloc(sizeof(char) * initial_size)
    };
    return list;
}

void string_append(string_t* s, char element){
    if (s->length == s->allocated_size){
        s->allocated_size *= 2;
        s->str = realloc(s->str, sizeof(char) * s->allocated_size);
    }

    s->str[s->length] = element;
    s->str[s->length+1] = '\0';
    s->length++;
}


void string_destroy(string_t list){
    free(list.str);
}