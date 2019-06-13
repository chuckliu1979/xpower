/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines Dispatcher Training Simulator (DTS) 
** hash and only used for internal.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include "xpower-hash.h"
#include "xpower-config.h"

/*
 * look up a hash entry in the hash table. return the pointer to
 * the existing entry, or the empty slot if none existed. the caller
 * need check it.
 */
static struct _xhash_entry *_xlookup_hash_entry(unsigned int hash, const struct _xhash *table){
    register unsigned int size = table->size, next = hash % size;
    struct _xhash_entry *bucket = table->bucket;
    
    while (bucket[next].index){
        if (bucket[next].hash == hash)
            break;
        next++;
        if (next >= size)
            next = 0;
    }
    
    return bucket + next;
}

/*
 * insert a new hash entry pointer into the table. if that hash entry already
 * existed, return the pointer to the existing entry, otherwise return NULL
 */
static struct _xhash_entry *_xinsert_hash_entry(unsigned int hash, unsigned int index, struct _xhash *table){
    struct _xhash_entry *entry = _xlookup_hash_entry(hash,table);
    if (!entry->index){
        entry->hash  = hash;
        entry->index = index;
        table->next++;
        return NULL;
    }
    return entry;
} 

/*
 * increase the hash table size, the new size got from the following format:
 * new_size = (((old_size)+16)*3/2)
 */
static void _xgrow_hash(struct _xhash *table){
    unsigned int old_size = table->size, new_size;
    struct _xhash_entry *old_bucket = table->bucket, *new_bucket;
    
    new_size = (((old_size)+16)*3/2);
    new_bucket = (struct _xhash_entry *)_xcalloc(sizeof(struct _xhash_entry), new_size);
    _xmem_copy(old_bucket, new_bucket, old_size *sizeof(struct _xhash_entry));
    table->size = new_size;
    table->bucket = new_bucket;
    
    _xfree(old_bucket);  
}

/*!
 * look up the hash table to find the entry with hash value,return the
 * index field of that entry or 0 if not found
 */
int _xlookup_hash(unsigned int hash, const struct _xhash *table){
    struct _xhash_entry *entry;
    if (!table->bucket)
        return 0;
    entry = _xlookup_hash_entry(hash,table);
    return entry->hash == hash ? entry->index : 0;
}

/*!
 * insert a new entry into the hash table, return 0 if success or
 * old index already existed.
 */
int _xinsert_hash(unsigned int hash, unsigned int index, struct _xhash *table){
    struct _xhash_entry *entry;
    unsigned int next = table->next;
    if (next >= table->size/2)
        _xgrow_hash(table);
    entry = _xinsert_hash_entry(hash,index,table);
    return  entry ? entry->index : 0;
}

/*!
 * deallocate the hash table
 */
void _xfree_hash(struct _xhash *table){
    _xfree(table->bucket);
    table->bucket = NULL;
    table->size = 0;
    table->next = 0;
}

