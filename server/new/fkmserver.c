/* fkmserver.c
 *
 * A Fkmbab server.
 *
 * (c) 2010, Fkmsoft
 */

#include "fkmserver.h"

static int rcvtill(char *buf, int fd, char delim);

fkmserver *fkmserver_init(int port, int (*getfd)(void *))
{
    fkmserver *s = malloc(sizeof(fkmserver));

    s->connections = 0;
    s->clients = fkmlist_create();
    s->getfd = getfd;
    if (port < 0)
        s->socket = -1;
    else {
        if ((s->socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            perror("socket creation failed");

        struct sockaddr_in s_addr;
        memset(&s_addr, 0, sizeof(s_addr));

        s_addr.sin_family = AF_INET;
        s_addr.sin_addr.s_addr = INADDR_ANY;
        s_addr.sin_port = htons(port);

        int val = 1;
        if (setsockopt(s->socket, SOL_SOCKET, SO_REUSEADDR,
                &val, sizeof(int)) == -1)
            perror("setsockopt");

        if (bind(s->socket, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1)
            perror("bind");
        if (listen(s->socket, 1) == -1)
            perror("listen");
    }

    return s;
}

void fkmserver_stop(fkmserver *s)
{
    if (!s)
        return;

    int i;
    void *c;
    for (i = 0; i < s->connections; i++) {
        c = fkmlist_get(s->clients, i);
        close(s->getfd(c));
    }

    fkmlist_del(s->clients, true);
    close(s->socket);

    return;
}

bool fkmserver_addnetc(fkmserver *s, void *(create_client)(int fd))
{
    if (!s)
        return false;

    int fd;
    if ((fd = accept(s->socket, 0, 0)) == -1) {
        perror("accept");
        return false;
    }

    void *cdata = create_client(fd);

    if (!cdata)
        return false;

    s->clients = fkmlist_append(s->clients, cdata);
    s->connections++;

    return true;
}

void fkmserver_refusenetc(fkmserver *s, char *message)
{
    if (!s)
        return;

    int fd = accept(s->socket, 0, 0);

    if (message)
        write(fd, message, strlen(message));

    close(fd);

    return;
}

bool fkmserver_addpipec(fkmserver *s, char *path)
{
    if (!s)
        return false;

    /* FIXME */
    return false;
}

void *fkmserver_getc(fkmserver *s, int index)
{
    if (!s)
        return 0;

    return fkmlist_get(s->clients, index);
}

bool fkmserver_rmc(fkmserver *s, int fd)
{
    if (!s)
        return false;

    int i;
    void *c;
    for (i = 0; i < s->connections; i++) {
        c = fkmlist_get(s->clients, i);
        if (s->getfd(c) == fd) {
            close(fd);
            s->clients = fkmlist_rm(s->clients, i, true);
            s->connections--;
            return true;
        }
    }

    return false;
}

bool fkmserver_rmidxc(fkmserver *s, int index)
{
    if (!s)
        return false;

    void *c = fkmlist_get(s->clients, index);
    close(s->getfd(c));
    s->clients = fkmlist_rm(s->clients, index, true);
    s->connections--;
    return true;
}

bool fkmserver_cfdsend(fkmserver *s, int fd, char *msg)
{
    if (!s || (write(fd, msg, strlen(msg) == -1)))
        return false;

    return true;
}

bool fkmserver_cidxsend(fkmserver *s, int index, char *msg)
{
    if (!s)
        return false;

    void *c;
    if ((c = fkmlist_get(s->clients, index))) {
        write(s->getfd(c), msg, strlen(msg));
    }

    return false;
}

char *fkmserver_cfdrecv(fkmserver *s, int fd)
{
    static char msg[MAXMSGLEN];

    if (!s)
        return 0;

    rcvtill(msg, fd, '\n');
    return msg;
}

char *fkmserver_cidxrecv(fkmserver *s, int index)
{
    static char msg[MAXMSGLEN];

    if (!s)
        return 0;

    void *c = fkmlist_get(s->clients, index);
    rcvtill(msg, s->getfd(c), '\n');

    return msg;
}

/* return value > 0: input on client with this index
 * return value < 0: disconnect by client with this index
 * return value == 0: new connect to server, or error (check errno) */
int fkmserver_poll(fkmserver *s)
{
    if (!s)
        return 0;

    int i, ret = 0;
    struct pollfd *pfds = malloc((s->connections + 1)*sizeof(struct pollfd));

    void *c;
    pfds[0].fd = s->socket;
    pfds[0].events = POLLIN;
    for (i = 1; i <= s->connections; i++) {
        c = fkmlist_get(s->clients, i - 1);
        pfds[i].fd = s->getfd(c);
        pfds[i].events = POLLIN | POLLRDHUP;
    }

    if (poll(pfds, s->connections + 1, -1) == -1) {
        perror("poll");
        return ret;
    }

    for (i = 1; (ret == 0) && (i <= s->connections); i++) {
        if (pfds[i].revents & POLLIN)
            ret = i;
        else if (pfds[i].revents & POLLRDHUP)
            ret = -i;
    }

    free(pfds);

    return ret;
}

static int rcvtill(char *buf, int fd, char delim)
{
    int count = 0;
    char *p = buf;
    do {
        if(read(fd, p++, 1) == -1) {
            if (errno == ECONNRESET) {
                puts("client disconnect");
                buf = "LOGOUT\n";
            } else
                perror("read");
            *p = 0;
            return count;
        }
        count++;
    } while ((p - buf < MAXMSGLEN) && (*(p - 1) != delim));

    *p = 0;
    return count;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
