#include "hashtable.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// https://benhoyt.com/writings/hash-table-in-c/

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

// use also for the hashing functions in the std ?
// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint64_t hash_key(const char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

hashtable_t hashtable_create(){
    hashtable_t hashtable = (hashtable_t){
        .length = 0,
        .capacity = INITIAL_CAPACITY,
        .entries = calloc(INITIAL_CAPACITY, sizeof(hashtable_entry))
    };
    return hashtable;
}

void* hashtable_get(hashtable_t table, const char* key){
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(table.capacity - 1));

    while (table.entries[index].key != NULL){
        if (strcmp(key, table.entries[index].key) == 0){
            return table.entries[index].value;
        }
        index++;
        if (index >= table.capacity){
            // wrap around
            index = 0;
        }
    }

    return NULL;
}

static const char* hashtable_set_entry(hashtable_entry* entries, size_t capacity, const char* key, void* value, size_t* table_length) {
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity - 1));
    while (entries[index].key != NULL) {
        if (strcmp(key, entries[index].key) == 0) {
            entries[index].value = value;
            return entries[index].key;
        }
        index++;
        if (index >= capacity){
            index = 0;
        }
    }
    if (table_length){
        key = strdup(key);
        (*table_length)++;
    }
    entries[index].key = key;
    entries[index].value = value;
    return key;
}

static bool hashtable_expand(hashtable_t* table) {
    // Allocate new entries array.
    size_t new_capacity = table->capacity * 2;
    if (new_capacity < table->capacity){
        // overflow
        return false;
    }

    hashtable_entry* new_entries = calloc(new_capacity, sizeof(hashtable_entry));

    // Iterate entries, move all non-empty ones to new table's entries.
    for (size_t i = 0; i < table->capacity; i++) {
        hashtable_entry entry = table->entries[i];
        if (entry.key != NULL) {
            hashtable_set_entry(new_entries, new_capacity, entry.key, entry.value, NULL);
        }
    }

    // Free old entries array and update this table's details.
    free(table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;
    return true;
}

// return the key
const char* hashtable_set(hashtable_t* table, const char* key, void* value){
    assert(value != NULL);

    if (table->length >= table->capacity / 2){
        if (!hashtable_expand(table)){
            return NULL;
        }
    }
    return hashtable_set_entry(table->entries, table->capacity, key, value, &table->length);
}

// TODO : remove from hashtable

void hashtable_destroy(hashtable_t table){
    for (size_t i = 0; i < table.capacity; i++){
        free((void*)table.entries[i].key);
    }

    free(table.entries);
}

hashtable_iterator_t hashtable_iterator(hashtable_t* table) {
    hashtable_iterator_t it;
    it._table = table;
    it._index = 0;
    return it;
}

bool ht_next(hashtable_iterator_t* it) {
    // Loop till we've hit end of entries array.
    hashtable_t* table = it->_table;
    while (it->_index < table->capacity) {
        size_t i = it->_index;
        it->_index++;
        if (table->entries[i].key != NULL) {
            // Found next non-empty item, update iterator key and value.
            hashtable_entry entry = table->entries[i];
            it->key = entry.key;
            it->value = entry.value;
            return true;
        }
    }
    return false;
}
