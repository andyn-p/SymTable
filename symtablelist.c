/*
Andy Nguyen - COS217 Assignment 3
*/

#include "symtable.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* symbol table struct, uses a linked list */
struct SymTable {
  /* store ptr to firt node in linked list */
  struct Node *first; 
  /* store length of list */
  size_t length;
};

/* nodes that are used in the symbol table's linked list */
struct Node {
  /* store defensive key copy */
  const char *key;
  /* store ptr to value of this node */
  const void *value;
  /* store ptr to next node in list */
  struct Node *next;
};

/* helper function to free node and its defensive key */
static void freeNode(struct Node *node) {
  free((char*)node->key);
  free(node);
}

SymTable_T SymTable_new(void) {
  SymTable_T table;
  table = (SymTable_T)malloc(sizeof(struct SymTable));
  if (table == NULL) {
    return NULL;
  }

  /* set default values */
  table->first = NULL;
  table->length = 0;
  return table;
}

void SymTable_free(SymTable_T oSymTable) {
  struct Node *curr;
  assert(oSymTable != NULL);

  /* iterate first ptr through list, curr ptr is set freed */
  curr = oSymTable->first;
  while (curr != NULL) {
    oSymTable->first = curr->next;    
    freeNode(curr);
    curr = oSymTable->first;
  }
  free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
  assert(oSymTable != NULL);
  return oSymTable->length;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
const void *pvValue) {
  struct Node *curr;
  assert(oSymTable != NULL && pcKey != NULL);

  curr = oSymTable->first;
  while (curr != NULL) {
    /* early out if we see it */
    if (strcmp(pcKey, curr->key) == 0) {
      return 0;
    }    
    curr = curr->next;
  }

  /* allocate memory for the new node and defensive key copy */
  curr = (struct Node*)malloc(sizeof(struct Node));
  if (curr == NULL) {
    return 0;
  }
  curr->key = (const char*)malloc(strlen(pcKey) + 1);
  if (curr->key == NULL) {
    free(curr); /* free the node if key can't be copied */
    return 0;
  }
  strcpy((char*)curr->key, pcKey);

  /* place the value in and insert node at front */
  curr->value = pvValue;
  curr->next = oSymTable->first;
  oSymTable->first = curr;

  oSymTable->length++;
  return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
const void *pvValue) {
  struct Node *curr;
  void *oldValue;
  assert(oSymTable != NULL && pcKey != NULL);

  curr = oSymTable->first;
  while (curr != NULL) {
    /* replace key and early out if key is found*/
    if (strcmp(pcKey, curr->key) == 0) {
      oldValue = (void*)curr->value;
      curr->value = pvValue;
      return oldValue;
    }
    curr = curr->next;
  }
  return NULL;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
  struct Node *curr;
  assert(oSymTable != NULL && pcKey != NULL);

  curr = oSymTable->first;
  while (curr != NULL) {
    /* early out if found */
    if (strcmp(pcKey, curr->key) == 0) {
      return 1;
    }
    curr = curr->next;
  }
  return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
  struct Node *curr;
  assert(oSymTable != NULL && pcKey != NULL);

  curr = oSymTable->first;
  while (curr != NULL) {
    /* early out if found */
    if (strcmp(pcKey, curr->key) == 0) {
      return (void*)curr->value;
    }
    curr = curr->next;
  }
  return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
  struct Node *curr;
  struct Node *prev;
  void *value;
  assert(oSymTable != NULL && pcKey != NULL);

  prev = oSymTable->first;
  /* empty list */
  if (prev == NULL) {
    return NULL;
  }

  /* first item is being removed */
  if (strcmp(pcKey, prev->key) == 0) {
    value = (void*)prev->value;
    oSymTable->first = prev->next;
    oSymTable->length--;
    freeNode(prev);
    return value;
  }

  /* curr ptr is in front of prev */
  curr = prev->next;
  while (curr != NULL) {
    if (strcmp(pcKey, curr->key) == 0) {
      value = (void*)curr->value;
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
  struct Node *curr;
  assert(oSymTable != NULL && pfApply != NULL);

  curr = oSymTable->first;
  while (curr != NULL) {
    (*pfApply)(curr->key, (void*)curr->value, (void*)pvExtra);
    curr = curr->next;
  }
}
