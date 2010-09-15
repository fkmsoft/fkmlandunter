/* fkmlist.h
 *
 * The Fkmsoft implementation of a (singly) linked list.
 *
 * (c) 2010, Fkmsoft
 */

#ifndef _FKMLIST_H
#define _FKMLIST_H

#include <stdlib.h>
#include <stdbool.h>

/* the data structure of a fkmlist. */
typedef struct fkmlist fkmlist;

/* return a pointer to a new, empty fkmlist. */
fkmlist *fkmlist_create();

/* append the item pointed to by data to the end of l.
 * returns a pointer to the modified fkmlist (it may have changed from l,
 * so make sure you save it) */
fkmlist *fkmlist_append(fkmlist *l, void *data);

/* return a pointer to the index'th item in l. */
void *fkmlist_get(fkmlist *l, int index);

/* remove the index'th item from l.
 * returns a pointer to the modified fkmlist, may differ from l. */
fkmlist *fkmlist_rm(fkmlist *l, int index, bool freedata);

/* remove the entire fkmlist pointed to by l.
 * if freedata is true, this function will also free all data elements in the
 * list */
void fkmlist_del(fkmlist *l, bool freedata);

#endif /* _FKMLIST_H */

/* vim: set sw=4 ts=4 et fdm=syntax: */
