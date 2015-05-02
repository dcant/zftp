#ifndef _HASH_H_
#define _HASH_H_

typedef unsigned int (*hashfunc_t)(void *key);

typedef struct hashdict hashdict_t;

hashdict_t *hash_init(hashfunc_t);
void hash_add_entry(hashdict_t *dict, void *key, int keysize, void *value, int valuesize);
void *hash_lookup_kv(hashdict_t *dict, void *key, int keysize);
void *hash_destroy(hashdict_t *dict);
void hash_remove_entry(hashdict_t *dict, void *key, int keysize);

void printhash(hashdict_t *dict);

#endif