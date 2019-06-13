/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines Dispatcher Training Simulator (DTS) 
** hash and only used for internal.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XPOWER_HASH_H_
#define _XPOWER_HASH_H_

#include "xpower-config.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _xhash_entry {
    unsigned int hash;
    unsigned int index;
};

struct _xhash {
    unsigned int size,next;
    struct _xhash_entry *bucket;
};

extern int _xlookup_hash(unsigned int hash, const struct _xhash *table);
extern int _xinsert_hash(unsigned int hash, unsigned int index, struct _xhash *table);
extern void _xfree_hash(struct _xhash *table);

static inline void _xinit_hash(struct _xhash *table){
    table->size = 0;
    table->next = 0;
    table->bucket = NULL;
}

#ifdef __cplusplus
}
#endif

#endif/*_XPOWER_HASH_H_*/

