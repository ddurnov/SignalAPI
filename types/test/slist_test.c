#include <stdio.h>

#include "slist.h"

static void * slist_p;

char * strings[] = {"One", "Two", "Three", "Four"};

static void printElement(void * data_p)
{
    printf("%s\n", (char *)data_p);
}

static Boolean_t compare(void * data1_p, void * data2_p)
{
    return !strncmp((char *)data1_p, (char *)data2_p, strlen((char *)data1_p));
}

int main(void)
{
    int i;

    char * tmp = NULL;

    slist_p = Slist_create();

    printf("size = %d\n", Slist_getSize(slist_p));

    for (i = 0; i < SIZE_OF_ARRAY(strings); i++)
    {
        Slist_add(slist_p, strings[i]);
    }

    printf("size = %d\n", Slist_getSize(slist_p));

    Slist_foreach(slist_p, printElement);

    tmp = Slist_find(slist_p, "Three", compare);

    if (tmp != NULL)
    {
        printf("Hit: %s\n", (char *)tmp);
    }
    else
    {
        printf("Miss: %s\n", "Three");
    }

    tmp = Slist_find(slist_p, "Five", compare);

    if (tmp != NULL)
    {
        printf("Hit: %s\n", (char *)tmp);
    }
    else
    {
        printf("Miss: %s\n", "Five");
    }

    for (i = 0; i < SIZE_OF_ARRAY(strings); i++)
    {
        Slist_remove(slist_p, strings[i]);
    }

    printf("size = %d\n", Slist_getSize(slist_p));

    Slist_foreach(slist_p, printElement);

    tmp = Slist_find(slist_p, "Three", compare);

    if (tmp != NULL)
    {
        printf("Hit: %s\n", (char *)tmp);
    }
    else
    {
        printf("Miss: %s\n", "Three");
    }

    Slist_destroy(slist_p);

    return 0;
}
