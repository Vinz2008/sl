#include "string.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define _GNU_SOURCE
#include <stdio.h>

string_t init_string(){
    const int initial_size = 10;
    string_t list = (string_t){
        .length = 0,
        .allocated_size = initial_size,
        .str = malloc(sizeof(char) * initial_size)
    };
    return list;
}

string_t init_string_from_str(const char* string){
    const int length = strlen(string);
    string_t list = (string_t){
        .length = length,
        .allocated_size = length * 2,
        .str = malloc(sizeof(char) * length * 2)
    };
    strcpy(list.str, string);
    return list;
}

string_t init_string_from_allocated_str(char* string){
    const int length = strlen(string);
    string_t list = (string_t){
        .length = length,
        .allocated_size = length,
        .str = string,
    };
    return list;
}

void string_append(string_t* s, char element){
    if (s->length+1 >= s->allocated_size){
        s->allocated_size *= 2;
        s->str = realloc(s->str, sizeof(char) * s->allocated_size);
    }

    s->str[s->length] = element;
    s->str[s->length+1] = '\0';
    s->length++;
}
void string_append_str(string_t* s, const char* str){
    int str_len = strlen(str);
    if (s->length + str_len + 1 >= s->allocated_size){
        while (s->length + str_len + 1 >= s->allocated_size){
            s->allocated_size *= 2;
        }
        s->str = realloc(s->str, sizeof(char) * s->allocated_size);
    }

    memcpy(s->str + s->length, str, str_len);
    s->str[s->length+str_len] = '\0';
    s->length += str_len;
}

static void empty_string(string_t* s){
    string_destroy(*s);
    *s = init_string();
}


static bool contains_formats(const char* format){
    for (int i = 0; i < strlen(format); i++){
        if (format[i] == '%'){
            return true;
        }
    }
    return false;
}

void vstring_writef(string_t* s, const char* format, va_list vlist){
    if (!contains_formats(format)){
        string_append_str(s, format);
        return;
    }
    char* buf;
    int ret = vasprintf(&buf, format, vlist);
    if (ret == -1){
        return;
    }

    string_append_str(s, buf);

    free(buf);
}

void string_writef(string_t* s, char* format, ...){
    va_list vlist;
    va_start(vlist, format);
    vstring_writef(s, format, vlist);
    va_end(vlist);
}

void string_overwritef(string_t* s, char* format, ...){
    empty_string(s);
    va_list vlist;
    va_start(vlist, format);
    vstring_writef(s, format, vlist);
    va_end(vlist);
}


void string_destroy(string_t list){
    free(list.str);
}