/* hashtable - Fast, simple array-based hash table, optimized for 'put' and 'get' ('delete' somewhat inefficient). */

/* $Id: hashtable.h,v 1.4 2005-06-24 17:41:52 acs Exp $ 
   Written by Adam Siepel, 2002.
   Copyright 2002, Adam Siepel, University of California.
*/


#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <lists.h>
#include <misc.h>

#define MULTIPLIER 31		/* recommended by Kernighan and Pike */
#define LOADING_FACTOR 5

typedef struct hash_table Hashtable;
struct hash_table {
  int nbuckets;                 /* Number of 'buckets' */
  List **keys,                  /* List of char* keys */
    **vals;                     /* Corresponding list of void* values */
};

Hashtable* hsh_new(int est_capacity);
void hsh_put(Hashtable *ht, char* key, void* val);
void* hsh_get(Hashtable* ht, char *key);
int hsh_delete(Hashtable* ht, char *key);
int hsh_reset(Hashtable *ht, char* key, void* val);
void hsh_free(Hashtable *ht);
void hsh_free_with_vals(Hashtable *ht);
unsigned int hsh_hash_func(Hashtable *ht, char* key);
List *hsh_keys(Hashtable *ht);

/***************************************************************************
 * inline functions; also defined in vector.c 
 ***************************************************************************/

/* we'll only inline the functions likely to be used heavily in inner
   loops */  

extern inline
unsigned int hsh_hash_func(Hashtable *ht, char* key) {
  unsigned int h = 0;
  int i = 0;
  for (i = 0; key[i] != '\0'; i++)
    h = MULTIPLIER * h + key[i];
  return h % ht->nbuckets;
}

extern inline
void hsh_put(Hashtable *ht, char* key, void* val) {
  unsigned int bucket = hsh_hash_func(ht, key);
  char *keycpy;
  if (ht->keys[bucket] == NULL) {
    ht->keys[bucket] = lst_new_ptr(LOADING_FACTOR);
    ht->vals[bucket] = lst_new_ptr(LOADING_FACTOR);
  }
  keycpy = smalloc(sizeof(char) * (strlen(key) + 1));
  strcpy(keycpy, key);

  lst_push_ptr(ht->keys[bucket], keycpy);
  lst_push_ptr(ht->vals[bucket], val);
}

/* hsh_get involves call to lst_find_compare with function pointer;
   not sure how best to inline, if at all */

#endif
