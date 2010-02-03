/* queue.c
 *
 * (c) 2010, fkmsft
 */

#include <stdlib.h>
#include <stdio.h>
#include <poll.h>

struct qelem {
    char *msg;
    FILE *client;
    struct qelem *next;
};

static struct qelem *queue = NULL;

struct qelem *q_addelem(char *msg, FILE *client, struct qelem *first)
{
    if(first) {
        /* puts("Walking queue"); */
        struct qelem *p;

        for (p = first; p->next; p = p->next)
            ;
        p->next = malloc(sizeof(struct qelem));
        p = p->next;
        p->msg = msg;
        p->client = client;
        p->next = NULL;
    } else {
        /* puts("Creating queue"); */
        first = malloc(sizeof(struct qelem));
        first->msg = msg;
        first->client = client;
        first->next = NULL;
    }

    return first;
}

int q_delelem(struct qelem *prev, struct qelem *ptr)
{
    if (prev) {
        prev->next = ptr->next;
        free(ptr);
        return 1;
    } else {
        struct qelem *old;
        old = ptr;
        ptr = ptr->next;
        free(old);
        return -1;
    }
}

int cputs(char *s, int cfd, FILE *cstream)
{
    /* static struct qelem *queue = NULL; */

    int chcount = 0;
    struct qelem *prev, *ptr;
    struct pollfd pfd;
    pfd.fd = cfd;
    pfd.events = POLLOUT;

    /* puts("Adding to queue"); */
    queue = q_addelem(s, cstream, queue);
    /* puts("New msg added:"); */
    /* puts(s); */

    for (ptr = queue, prev = 0; ptr && (chcount = poll(&pfd, 1, -100));
            prev = ptr, ptr = ptr->next) {
        /* printf("Poll returned %d\n", chcount); */
        if (ptr->client == cstream) {
            chcount += fputs(ptr->msg, ptr->client);
            /* puts("Removing from queue"); */
            if (-1 == q_delelem(prev, ptr))
                queue = NULL;
        }
    }
    fflush(cstream);

    /* printf("Wrote %d characters\n", chcount); */

    return chcount;
}

void q_p(void)
{
    int i;
    struct qelem *p;
    printf("Queue is %p\n", queue);
    for(i = 0, p = queue; p; p = p->next, i++) {
        printf("Elem %d: %s (for %p)\n", i, p->msg, p->client);
    }
}

/* vim: set sw=4 ts=4 fdm=syntax: */
