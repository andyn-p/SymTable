/*
Andy Nguyen - COS217 Assignment 3
*/

#include <stddef.h>
#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

/* Abstract SymTable to SymTable_T */
typedef struct SymTable *SymTable_T;

/* 
Return a new SymTable object with no bindings, or NULL if there is 
insufficient memory.
*/
SymTable_T SymTable_new(void);

/* 
Free all memory of the passed in oSymTable.
*/
void SymTable_free(SymTable_T oSymTable);

/* 
Return the number of bindings of oSymTable.
*/
size_t SymTable_getLength(SymTable_T oSymTable);

/* 
Bind pvValue to pcKey and put this binding into oSymTable if pcKey does
not exist in oSymTable, then return 1. Else, return 0.
*/
int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
const void *pvValue);

/* 
If oSymTable contains pcKey, then the binded value will be changed to 
pvValue and the old value will be returned. Else, return NULL.
*/
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
const void *pvValue);

/* 
Return 1 if oSymTable has a binding with pcKey. Else, return 0.
*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* 
Return the value bound to pcKey in oSymTable. Return NULL if pcKey does 
not exist in oSymTable.
*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* 
Return the value bound to pcKey in oSymTable and remove the binding
from oSymtable. Else, return NULL.
*/
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* 
Iterates through oSymTable and applies pfApply function to each pcKey 
and bound pvValue with additional pvExtra paramter.
*/
void SymTable_map(SymTable_T oSymTable,
void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
const void *pvExtra);

#endif