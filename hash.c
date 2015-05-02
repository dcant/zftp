#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static const unsigned int initialsize = 2;
static const unsigned int  initialrate = 1;

typedef struct hashentry {
	void *key;
	void *value;
	struct hashentry *next;
} hashentry_t;

typedef struct hashht {
	unsigned int size;
	unsigned int mask; // mask of size;
	hashentry_t **bucket;
} hashht_t;

struct hashdict {
	int rehashing;
	int rehashidx;
	unsigned int size;
	unsigned int used;
	unsigned int rate;
	hashfunc_t func;
	hashht_t ht[2];
};

static unsigned int _initialhashfunc(void *key)
{
	char *str = (char *)key;
	unsigned int hash = 5381;
	int i = 0;
	while (*str != '\0') {
		hash += ((hash << 5) + hash) + *str;
		str++;
	}
	return hash;
}

static void _hash_reset_ht(hashht_t *ht)
{
	ht->size = 0;
	ht->mask = 0;
	ht->bucket = NULL;
}

static void _hash_expend(hashdict_t *dict)
{
	hashht_t n;
	n.size = dict->size * 2;
	n.mask = n.size - 1;
	dict->size = n.size;
	n.bucket = (hashentry_t **)malloc(n.size * sizeof(hashentry_t *));
	memset(n.bucket, 0, n.size * sizeof(hashentry_t *));
	dict->ht[1] = n;
	dict->rehashing = 1;
	dict->rehashidx = 0;
}

static void _rehash_step(hashdict_t *dict)
{
	assert(dict->rehashidx < dict->ht[0].size);
	hashentry_t *entry, *nextentry;

	while(dict->ht[0].bucket[dict->rehashidx] == NULL && dict->rehashidx < dict->ht[0].size)
		dict->rehashidx++;
	if (dict->rehashidx < dict->ht[0].size) {
		entry = dict->ht[0].bucket[dict->rehashidx];
		dict->ht[0].bucket[dict->rehashidx] = NULL;
		while (entry != NULL) {
			nextentry = entry->next;
			int pos = dict->func(entry->key) & dict->ht[1].mask;
			entry->next = dict->ht[1].bucket[pos];
			dict->ht[1].bucket[pos] = entry;
			entry = nextentry;
		}
		dict->rehashidx++;
	}
	if (dict->rehashidx >= dict->ht[0].size) {
		dict->rehashing = 0;
		dict->rehashidx = 0;
		free(dict->ht[0].bucket);
		dict->ht[0] = dict->ht[1];
		_hash_reset_ht(&dict->ht[1]);
		return;
	}
}

static hashentry_t *_hash_lookup_entry(hashdict_t *dict, void *key, int keysize)
{
	int pos = 0, i;
	hashentry_t *entry;
	int num = dict->rehashing ? 2 : 1;

	for (i = 0; i < num; i++) {
		dict->func(key);
		pos = dict->func(key) & dict->ht[i].mask;
		entry = dict->ht[i].bucket[pos];
		while (entry && memcmp(entry->key, key, keysize) != 0)
			entry = entry->next;
		if (entry != NULL)
			break;
	}
	return entry;
}

hashdict_t *hash_init(hashfunc_t func)
{
	hashdict_t *hashdict = (hashdict_t *)malloc(sizeof(hashdict_t));

	if (func == NULL)
		hashdict->func = _initialhashfunc;
	else
		hashdict->func = func;

	_hash_reset_ht(&hashdict->ht[0]);
	_hash_reset_ht(&hashdict->ht[1]);

	hashdict->rehashing = 0;
	hashdict->rehashidx = 0;
	hashdict->size = initialsize;
	hashdict->used = 0;
	hashdict->rate = initialrate;
	hashdict->ht[0].size = initialsize;
	hashdict->ht[0].mask = initialsize - 1;
	hashdict->ht[0].bucket = (hashentry_t **)malloc(initialsize * sizeof(hashentry_t *));
	memset(hashdict->ht[0].bucket, 0, initialsize * sizeof(hashentry_t *));
	return hashdict;
}

void hash_add_entry(hashdict_t *dict, void *key, int keysize, void *value, int valuesize)
{
	int hashcode = 0, pos = 0;
	hashentry_t *entry;
	hashht_t *ht;

	if (dict->used / dict->size >= dict->rate) {
		_hash_expend(dict);
	}
	if (dict->rehashing) {
		_rehash_step(dict);
	}
	if (_hash_lookup_entry(dict, key, keysize))
		return;
	ht = dict->rehashing ? &dict->ht[1] : &dict->ht[0];
	hashcode = dict->func(key);
	pos = hashcode & ht->mask;
	entry = (hashentry_t *)malloc(sizeof(hashentry_t));
	memset(entry, 0, sizeof(hashentry_t));
	entry->key = malloc(keysize);
	entry->value = malloc(valuesize);
	memcpy(entry->key, key, keysize);
	memcpy(entry->value, value, valuesize);
	entry->next = ht->bucket[pos];
	ht->bucket[pos] = entry;
	dict->used++;
}

void *hash_lookup_kv(hashdict_t *dict, void *key, int keysize)
{
	hashentry_t *entry = _hash_lookup_entry(dict, key, keysize);
	return entry ? entry->value : NULL;
}

void hash_remove_entry(hashdict_t *dict, void *key, int keysize)
{
	int pos = 0, i;
	hashentry_t *entry, *pre = NULL;
	int num = dict->rehashing ? 2 : 1;
	for (i = 0; i < num; i++) {
		pos = dict->func(key) & dict->ht[i].mask;

		entry = dict->ht[i].bucket[pos];
		while (entry && memcmp(entry->key, key, keysize) != 0) {
			pre = entry;
			entry = entry->next;
		}

		if (entry) {
			if (pre)
				pre->next = entry->next;
			else
				dict->ht[i].bucket[pos] = entry->next;

			free(entry->key);
			free(entry->value);
			free(entry);
			dict->used--;
		}
	}
}

void *hash_destroy(hashdict_t *dict)
{
	int num = dict->rehashing ? 2 : 1;
	int i, pos;
	hashentry_t *entry, *next;

	for (i = 0; i < num; i++) {
		for (pos = 0; pos < dict->ht[i].size; pos++) {
			entry = dict->ht[i].bucket[pos];
			while (entry) {
				next = entry->next;
				free(entry->key);
				free(entry->value);
				free(entry);
				entry = next;
			}
		}

		free(dict->ht[i].bucket);
	}
	free(dict);
}

void printhash(hashdict_t *dict)
{
	int pos = 0, i, j;
	hashentry_t *entry;
	int num = dict->rehashing ? 2 : 1;
	for (i = 0; i < num; i++) {
		for (pos = 0; pos < dict->ht[i].size; pos++) {
			entry = dict->ht[i].bucket[pos];
			while (entry) {
				printf("%s %d %d\n", (char *)entry->key, i, pos);
				entry = entry->next;
			}
		}
	}
}