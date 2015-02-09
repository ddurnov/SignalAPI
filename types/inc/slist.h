/*******************************************************************************
 *
 * Project:
 *
 * Subsystem:
 *
 * Filename: slist.h
 *
 * Description: Single linked list header.
 *
 * Author: Dmitry Durnov (dmitry.durnov at gmail.com)
 *
 * History:
 * -----------------------------------------------------------------------------
 * Version  Date      Author            Comment
 * -----------------------------------------------------------------------------
 * 0.1.0    20100604  Dmitry Durnov     Created.
 * 0.2.0    20100605  Dmitry Durnov     Slist_find has been implemented.
 * 0.2.1    20100606  Dmitry Durnov     Slist_foreach has been updated.
 *
 ******************************************************************************/

#ifndef SLIST_H
#define SLIST_H

#include "types_general.h"

typedef struct SlistCell SlistCell_t;
typedef struct Slist     Slist_t;

typedef Boolean_t (*isEqualCheck_t) (void * data1_p, void * data2_p);

struct SlistCell {
    SlistCell_t * next_p;
    void * data_p;
};

struct Slist {
    SlistCell_t * head_p;
    unsigned int size;
};

void *
Slist_create();

void
Slist_destroy(void * aSlist_p);

Boolean_t
Slist_add(void * aSlist_p, void * data_p);

Boolean_t
Slist_remove(void * aSlist_p, void * data_p);

void *
Slist_find(void * aSlist_p,
           void * data_p,
           Boolean_t (*isEqualCheck)(void * data1_p, void * data2_p));

unsigned int
Slist_getSize(void * aSlist_p);

void
Slist_foreach(void * aSlist_p, void (*executeFunc)(void *, void *), void * arg_p);

#endif /* SLIST_H */
