/*
Andy Nguyen - COS217 Assignment 3
*/

#include "symtable.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* all of the bucket types */
static size_t bucketCounts[] = {509, 1021, 2039, 4093, 8191, 16381, 
32749, 65521};

/* number of bucket counts */
static const size_t numBucketCounts = 
sizeof(bucketCounts)/sizeof(bucketCounts[0]);

/* symbol table struct: uses a hash table */
struct SymTable {
  /* store ptr to the bucket array */
  struct Node **buckets;
  /* store length of symbol table */
  size_t length;
  /* store index that refers to bucket count */
  size_t bucketIndex;
};

/* binding struct: elements in the buckets of the hash table */
struct Node {
  /* store defensive key copy */
  const char *key;
  /* store ptr to value of this node */
  const void *value;
  /* store ptr to next node in bucket */
  struct Node *next;
};

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
inclusive. */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount) {
  const size_t HASH_MULTIPLIER = 65599;
  size_t u;
  size_t uHash = 0;
  assert(pcKey != NULL);

  for (u = 0; pcKey[u] != '\0'; u++)
    uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

  return uHash % uBucketCount;
}

/* helper function to free node and its defensive key */
static void freeNode(struct Node *node) {
  free((char*)node->key);
  free(node);
}

/* expand the oSymTable's bucket array if needed */
static void expand(SymTable_T oSymTable) {
  size_t newIndex = oSymTable->bucketIndex + 1;
  size_t newBucketCount;
  struct Node **newBuckets;
  size_t i;
  size_t keyHash;
  struct Node *curr;
  struct Node *next;

  /* don't exceed max buckets */
  if (newIndex > numBucketCounts - 1) {
    return;
  }

  /* alloc newbuckets */
  newBucketCount = bucketCounts[newIndex];
  newBuckets = 
  (struct Node**)calloc(newBucketCount, sizeof(struct Node*));
  if (newBuckets == NULL) {
    return;
  }

  /* rehash everything inside of symtable */
  for (i = 0; i < bucketCounts[newIndex - 1]; i++) {
    if (oSymTable->buckets[i] == NULL) {
      continue;
    }

    /* iterate through the lists and rehash each node in the list */
    curr = oSymTable->buckets[i];
    while (curr != NULL) {
      keyHash = SymTable_hash(curr->key, newBucketCount);
      next = curr->next;
      curr->next = newBuckets[keyHash];
      newBuckets[keyHash] = curr;
      curr = next;
    }
  }

  /* free old bucket array */
  free(oSymTable->buckets);

  /* update symtable's bucket ptr and bucket index */
  oSymTable->buckets = newBuckets;
  oSymTable->bucketIndex = newIndex;
}

SymTable_T SymTable_new(void) {
  SymTable_T table;
  
  table = (SymTable_T)malloc(sizeof(struct SymTable));
  if (table == NULL) {
    return NULL;
  }

  /* alloc memory for the buckets */
  table->buckets = 
  (struct Node**)calloc(bucketCounts[0], sizeof(struct Node*));
  if (table->buckets == NULL) {
    /* free table struct if buckets failed */
    free(table);
    return NULL;
  }
  
  /* default values */
  table->length = 0;
  table->bucketIndex = 0;
  return table;
}

void SymTable_free(SymTable_T oSymTable) {
  size_t i;
  struct Node *curr;
  assert(oSymTable != NULL);

  for (i = 0; i < bucketCounts[oSymTable->bucketIndex]; i++) {
    /* skip empty buckets */
    if (oSymTable->buckets[i] == NULL) {
      continue;
    }

    /* iterate through list in this bucket */
    curr = oSymTable->buckets[i];
    while (curr != NULL) {
      oSymTable->buckets[i] = curr->next;
      freeNode(curr);
      curr = oSymTable->buckets[i];
    }
  }
  free(oSymTable->buckets);
  free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
  assert(oSymTable != NULL);
  return oSymTable->length;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
const void *pvValue) {
  size_t keyHash;
  struct Node *curr;
  assert(oSymTable != NULL && pcKey != NULL);

  /* expand before adding new element if at capacity */
  if (oSymTable->length == bucketCounts[oSymTable->bucketIndex]) {
    expand(oSymTable);
  }

  /* check if node binding alread exists */
  keyHash = SymTable_hash(pcKey, bucketCounts[oSymTable->bucketIndex]);
  curr = oSymTable->buckets[keyHash];
  while (curr != NULL) {
    if (strcmp(pcKey, curr->key) == 0) {
      return 0;
    }
    curr = curr->next;
  }

  /* make new node binding */
  curr = (struct Node*)malloc(sizeof(struct Node));
  if (curr == NULL) {
    return 0;
  }
  curr->key = (const char*)malloc(strlen(pcKey) + 1);
  if (curr->key == NULL) {
    free(curr);
    return 0;
  }
  strcpy((char*)curr->key, pcKey);
  
  curr->value = pvValue;
  curr->next = oSymTable->buckets[keyHash];
  oSymTable->buckets[keyHash] = curr;

  oSymTable->length++;

  return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
const void *pvValue) {
  size_t keyHash;
  struct Node *curr;
  void *oldValue;
  assert(oSymTable != NULL && pcKey != NULL);

  /* find if pcKey exists in a bucket and replace its value */
  keyHash = SymTable_hash(pcKey, bucketCounts[oSymTable->bucketIndex]);
  curr = oSymTable->buckets[keyHash];
  while (curr != NULL) {
    if (strcmp(pcKey, curr->key) == 0) {
      oldValue = (void*)curr->value;
      curr->value = pvValue;
      return oldValue;
    }
  }
  return NULL;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
  size_t keyHash;
  struct Node *curr;
  assert(oSymTable != NULL && pcKey != NULL);

  /* find if pcKey exists in a bucket */
  keyHash = SymTable_hash(pcKey, bucketCounts[oSymTable->bucketIndex]);
  curr = oSymTable->buckets[keyHash];
  while (curr != NULL) {
    if (strcmp(pcKey, curr->key) == 0) {
      return 1;
    }
    curr = curr->next;
  }
  return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
  size_t keyHash;
  struct Node *curr;
  assert(oSymTable != NULL && pcKey != NULL);

  /* find pcKey in the buckets */
  keyHash = SymTable_hash(pcKey, bucketCounts[oSymTable->bucketIndex]);
  curr = oSymTable->buckets[keyHash];
  while (curr != NULL) {
    if (strcmp(pcKey, curr->key) == 0) {
      return (void*)curr->value;
    }
    curr = curr->next;
  }
  return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
  size_t keyHash;
  struct Node *curr;
  struct Node *prev;
  void *value;
  assert(oSymTable != NULL && pcKey != NULL);

  keyHash = SymTable_hash(pcKey, bucketCounts[oSymTable->bucketIndex]);
  prev = oSymTable->buckets[keyHash];
  /* empty bucket*/
  if (prev == NULL) { 
    return NULL;
  }

  /* removing first value */
  if (strcmp(pcKey, prev->key) == 0) {
    value = (void*)prev->value;
    oSymTable->buckets[keyHash] = prev->next;
    oSymTable->length--;
    freeNode(prev);
    return value;
  }

  /* curr ptr is in front of prev */
  curr = prev->next;
  while (curr != NULL) {
    if (strcmp(pcKey, curr->key) == 0) {
      value = (void*) curr->value;
      prev->next = curr->next;
      oSymTable->length--;
      freeNode(curr);
      return value;
    }

    /* iterate prev and curr forward */
    prev = curr;
    curr = curr->next;
  }
  return NULL;
}

void SymTable_map(SymTable_T oSymTable,
void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
const void *pvExtra) {
  size_t i;
  struct Node *curr;
  assert(oSymTable != NULL && pfApply != NULL);

  for (i = 0; i < bucketCounts[oSymTable->bucketIndex]; i++) {
    /* skip empty buckets */
    if (oSymTable->buckets[i] == NULL) {
      continue;
    }

    /* apply to all nodes in this list in the bucket */
    curr = oSymTable->buckets[i];
    while (curr != NULL) {
      (*pfApply)(curr->key, (void*)curr->value, (void*)pvExtra);
      curr = curr->next;
    }
  }
}
