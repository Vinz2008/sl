#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    const char* key;  // key is NULL if this slot is empty
    void* value;
} hashtable_entry;

typedef struct {
    hashtable_entry* entries;  // hash slots
    size_t capacity;    // size of _entries array
    size_t length;      // number of items in hash table
} hashtable_t;

#define INITIAL_CAPACITY 16

hashtable_t hashtable_create();
void* hashtable_get(hashtable_t table, const char* key);
const char* hashtable_set(hashtable_t* table, const char* key, void* value);
void hashtable_destroy(hashtable_t table);


typedef struct {
    const char* key;
    void* value;

    // Don't use these fields directly.
    hashtable_t* _table;       // reference to hash table being iterated
    size_t _index;    // current index into ht._entries
} hashtable_iterator_t;

// Return new hash table iterator (for use with ht_next).
hashtable_iterator_t ht_iterator(hashtable_t* table);

// Move iterator to next item in hash table, update iterator's key
// and value to current item, and return true. If there are no more
// items, return false. Don't call ht_set during iteration.
bool ht_next(hashtable_iterator_t* it);
