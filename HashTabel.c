#include "HashTabel.h"


/* Create a new hashtable. */
hashTable *CreateHashTable( int size ) {

    hashTable *hashtable = NULL;
    int i;

    if( size < 1 ) return NULL;

    /* Allocate the table itself. */
    if( ( hashtable = malloc( sizeof( hashTable ) ) ) == NULL ) {
        return NULL;
    }

    /* Allocate pointers to the head nodes. */
    if( ( hashtable->table = malloc( sizeof( entry * ) * size ) ) == NULL ) {
        return NULL;
    }
    for( i = 0; i < size; i++ ) {
        hashtable->table[i] = NULL;
    }

    hashtable->size = size;

    return hashtable;
}

/* Hash a string for a particular hash table. */
int PerformHashing( hashTable *hashtable, char *key ) {

    unsigned long int hashval = 0;
    int i = 0;

    /* Convert our string to an integer */
    while( hashval < ULONG_MAX && i < strlen( key ) ) {
        hashval = hashval << 8;
        hashval += key[ i ];
        i++;
    }

    return hashval % hashtable->size;
}

/* Create a key-value pair. */
entry *CreateNewPair( char *key, unsigned char value, char* entryType ) {
    entry *newpair;

    if( ( newpair = malloc( sizeof( entry ) ) ) == NULL ) {
        return NULL;
    }

    if( ( newpair->key = strdup( key ) ) == NULL ) {
        return NULL;
    }

    newpair->value =value;

    if (entryType != NULL) {
        strcpy(newpair->entryType, entryType);
    }

    newpair->next = NULL;

    return newpair;
}

/* Insert a key-value pair into a hash table. */
void SetValue( hashTable *hashtable, char *key, unsigned char value,  char* entryType ) {
    int bin = 0;
    entry *newpair = NULL;
    entry *next = NULL;
    entry *last = NULL;

    bin = PerformHashing( hashtable, key );

    next = hashtable->table[ bin ];

    while( next != NULL && next->key != NULL && strcmp( key, next->key ) > 0 ) {
        last = next;
        next = next->next;
    }

    /* There's already a pair.  Let's replace that string. */
    if( next != NULL && next->key != NULL && strcmp( key, next->key ) == 0 ) {

        next->value = value;
        strcpy(next->entryType, entryType);
        /* Nope, could't find it.  Time to grow a pair. */
    } else {
        newpair = CreateNewPair( key, value, entryType);

        /* We're at the start of the linked list in this bin. */
        if( next == hashtable->table[ bin ] ) {
            newpair->next = next;
            hashtable->table[ bin ] = newpair;

            /* We're at the end of the linked list in this bin. */
        } else if ( next == NULL ) {
            last->next = newpair;

            /* We're in the middle of the list. */
        } else  {
            newpair->next = next;
            last->next = newpair;
        }
    }
}

/* Retrieve a key-value pair from a hash table. */
unsigned char GetValue( hashTable *hashtable, char *key ) {
    int bin = 0;
    entry *pair;

    bin = PerformHashing( hashtable, key );

    /* Step through the bin, looking for our value. */
    pair = hashtable->table[ bin ];
    while( pair != NULL && pair->key != NULL && strcmp( key, pair->key ) > 0 ) {
        pair = pair->next;
    }

    /* Did we actually find anything? */
    if( pair == NULL || pair->key == NULL || strcmp( key, pair->key ) != 0 ) {
        return '\0';

    } else {
        return pair->value;
    }

}

char* GetType( hashTable *hashtable, char *key ) {
    int bin = 0;
    entry *pair;

    bin = PerformHashing( hashtable, key );

    /* Step through the bin, looking for our value. */
    pair = hashtable->table[ bin ];
    while( pair != NULL && pair->key != NULL && strcmp( key, pair->key ) > 0 ) {
        pair = pair->next;
    }

    /* Did we actually find anything? */
    if( pair == NULL || pair->key == NULL || strcmp( key, pair->key ) != 0 ) {
        return NULL;

    } else {
        return pair->entryType;
    }
}

void ClearHashTable(hashTable** hashTablel) {
    int i;
    for (i = 0; i < (*hashTablel)->size; i++) {
        while ((*hashTablel)->table[i] != NULL){
            free((*hashTablel)->table[i]->key);
            free((*hashTablel)->table[i]);
            (*hashTablel)->table[i] = (*hashTablel)->table[i]->next;
        }
    }
    free(*hashTablel);
}