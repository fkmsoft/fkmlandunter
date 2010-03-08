/* fkmlist.c
 *
 * (c) 2010, Fkmsoft
 */

#include "fkmlist.h"

fkmlist *fkmlist_create()
{
    return (fkmlist *) 0/*calloc(1, sizeof(fkmlist))*/;
}

fkmlist *fkmlist_append(fkmlist *l, void *data)
{
    fkmlist *n = (fkmlist *) malloc(sizeof(fkmlist));
    n->data = data;
    n->next = 0;

    if (l) {
        fkmlist *p = l;
        while (p->next)
            p = p->next;
        p->next = n;
        return l;
    }

    return n;
}

void *fkmlist_get(fkmlist *l, int index)
{
    if (!l)
        return 0;

    int i;
    fkmlist *p = l;

    for (i = 0; i < index; i++, p = p->next)
        if (i + 1 < index && !p->next)
            return 0;

    return p->data;
}

fkmlist *fkmlist_rm(fkmlist *l, int index, bool freedata)
{
    if (!l)
        return 0;

    fkmlist *p = l;
    if (index == 0) {
        p = l->next;
        if (freedata)
            free(l->data);
        free(l);
        return p;
    }

    int i;
    for (i = 0; i < index - 1; i++, p = p->next)
        if (!p->next)
            return 0;

    if (!p->next)
        return 0;
    else {
        fkmlist *rm = p->next;
        if (freedata)
            free(rm->data);
        p->next = rm->next;
        free(rm);
        return l;
    }
}

void fkmlist_del(fkmlist *l, bool freedata)
{
    while (l) {
        fkmlist *rm = l;
        if (freedata)
            free(rm->data);
        l = rm->next;
        free(rm);
    }

    return;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
