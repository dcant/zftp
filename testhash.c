#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "hash.h"
#include <mcheck.h>

int main()
{
	hashdict_t *hash;
//	mtrace();
	hash = hash_init(NULL);
	uint32_t x = -83777344;
	char *one = (char *)hash_lookup_kv(hash, &x, sizeof(x));
	printf("%s\n", one ? one : "no");
	char *key = "hello";
	char *value = "world";
	hash_add_entry(hash, key, strlen(key), value, strlen(value));
	char *v = (char *)hash_lookup_kv(hash, key, strlen(key));
	printf("%s\n", v);
	hash_add_entry(hash, "guess", strlen("guess"), "nice", strlen("nice"));
	hash_add_entry(hash, "fdaff", strlen("guess"), "nice", strlen("nice"));
	hash_add_entry(hash, "fdagf", strlen("guess"), "nice", strlen("nice"));
	hash_add_entry(hash, "jfdjj", strlen("guess"), "nice", strlen("nice"));
	hash_add_entry(hash, "idfjf", strlen("guess"), "nice", strlen("nice"));
	hash_add_entry(hash, "jidkl", strlen("guess"), "nice", strlen("nice"));
	hash_add_entry(hash, "icidk", strlen("guess"), "nice", strlen("nice"));
	v = (char *)hash_lookup_kv(hash, "guess", strlen(key));
	printhash(hash);
	hash_remove_entry(hash, key, strlen(key));
	v = (char *)hash_lookup_kv(hash, key, strlen(key));
	printf("%s\n", v == NULL ? "no data" : v);
	v = (char *)hash_lookup_kv(hash, "fdaff", strlen(key));
	printf("%s\n", v == NULL ? "not found" : v);
	printhash(hash);
	hash_destroy(hash);
//	muntrace();
	return 0;
}