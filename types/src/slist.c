/*******************************************************************************
 *
 * Project:
 *
 * Subsystem:
 *
 * Filename: slist.c
 *
 * Description: Single linked list implementation.
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

#include <stdio.h>
#include <malloc.h>

#include "slist.h"

void *
Slist_create()
{
    Slist_t * aSlist_p = NULL;

    aSlist_p = (Slist_t *)malloc(sizeof(Slist_t));

    if (aSlist_p == NULL)
    {
        printf("Unable to allocate memory\n");
        return aSlist_p;
    }

    aSlist_p->size   = 0;
    aSlist_p->head_p = NULL;

    return (void *)aSlist_p;
}

void
Slist_destroy(void * slist_p)
{
    Slist_t * aSlist_p      = (Slist_t *)slist_p;
    SlistCell_t * tmpCell_p = NULL;

    if (slist_p == NULL)
    {
        return;
    }

    while (aSlist_p->size != 0)
    {
        tmpCell_p = aSlist_p->head_p;

        aSlist_p->head_p = aSlist_p->head_p->next_p;

        free(tmpCell_p);

        aSlist_p->size--;
    }

    return;
}

Boolean_t
Slist_add(void * slist_p, void * data_p)
{
    Slist_t * aSlist_p      = (Slist_t *)slist_p;
    SlistCell_t * tmpCell_p = NULL;

    if (slist_p == NULL)
    {
        return FALSE;
    }

    tmpCell_p = (SlistCell_t *)malloc(sizeof(SlistCell_t));

    if (tmpCell_p == NULL)
    {
        printf("Unable to allocate memory\n");
        return FALSE;
    }

    tmpCell_p->data_p = data_p;
    tmpCell_p->next_p = NULL;

    if (aSlist_p->size != 0)
    {
        tmpCell_p->next_p = aSlist_p->head_p;
    }

    aSlist_p->head_p = tmpCell_p;

    aSlist_p->size++;

    return TRUE;
}

Boolean_t
Slist_remove(void * slist_p, void * data_p)
{
    Slist_t * aSlist_p          = (Slist_t *)slist_p;
    SlistCell_t * tmpCell_p     = NULL;
    SlistCell_t * tmpCellPrev_p = NULL;

    if (slist_p == NULL)
    {
        return FALSE;
    }

    tmpCell_p = aSlist_p->head_p;

    while (tmpCell_p != NULL)
    {
        if (tmpCell_p->data_p == data_p)
        {
            if (tmpCellPrev_p == NULL)
            {
                aSlist_p->head_p = tmpCell_p->next_p;
            }
            else
            {
                tmpCellPrev_p->next_p = tmpCell_p->next_p;
            }

            free(tmpCell_p);

            aSlist_p->size--;

            return TRUE;
        }

        tmpCellPrev_p = tmpCell_p;
        tmpCell_p     = tmpCell_p->next_p;

    }

    return FALSE;
}

void *
Slist_find(void * slist_p, void * data_p, isEqualCheck_t check)
{
    Slist_t * aSlist_p      = (Slist_t *)slist_p;
    SlistCell_t * tmpCell_p = NULL;

    if (slist_p == NULL)
    {
        return NULL;
    }

    for (tmpCell_p = aSlist_p->head_p;
         tmpCell_p != NULL;
         tmpCell_p = tmpCell_p->next_p)
    {
        if (check(tmpCell_p->data_p, data_p))
        {
            return tmpCell_p->data_p;
        }
    }

    return NULL;
}

unsigned int
Slist_getSize(void * slist_p)
{
    Slist_t * aSlist_p = (Slist_t *)slist_p;

    if (slist_p == NULL)
    {
        return 0;
    }

    return aSlist_p->size;
}

void
Slist_foreach(void * slist_p, void (*executeFunc)(void *, void *), void * arg_p)
{
    Slist_t * aSlist_p      = (Slist_t *)slist_p;
    SlistCell_t * tmpCell_p = NULL;

    if (slist_p == NULL)
    {
        return;
    }

    for (tmpCell_p = aSlist_p->head_p;
         tmpCell_p != NULL;
         tmpCell_p = tmpCell_p->next_p)
    {
        executeFunc(tmpCell_p->data_p, arg_p);
    }

    return;
}
