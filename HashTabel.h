#ifndef OPENUTASK14_LABELHASHTABEL_H
#define OPENUTASK14_LABELHASHTABEL_H

#define _XOPEN_SOURCE 500 /* Enable certain library functions (strdup) on linux.  See feature_test_macros(7) */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#define TYPE_SIZE 20

typedef struct entry_s {
    char *key;
    unsigned char value;
    char entryType[TYPE_SIZE];
    struct entry_s *next;
} entry;

typedef struct HashTable {
    int size;
    entry** table;
} hashTable;


hashTable *CreateHashTable( int size );
/*return value(address) to a specified key (label string)*/
unsigned char GetValue(hashTable* labelHashTable, char* key);
/*map the key to the value*/
void SetValue (hashTable* labelHashTable, char* key, unsigned char value,  char* entryType);
int PerformHashing( hashTable *hashtable, char *key );
entry *CreateNewPair( char *key, unsigned char value, char* entryType );
unsigned char GetValue( hashTable *hashtable, char *key );
char* GetType( hashTable *hashtable, char *key );
void ClearHashTable(hashTable** hashTablel);

#endif
