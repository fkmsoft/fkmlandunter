/* control.c
 *
 * The control server implementation.
 *
 * (c) 2010, Fkmsoft
 */

#include "control.h"

struct conserver {
    /* the original fkmserver which is being controlled */
    fkmserver *slave;

    /* the fkmserver managing the controlling clients */
    fkmserver *control;

    /* list of accepted users */
    fkmlist *users;

    /* list of available commands */
    fkmlist *cmds;
};

struct command {
    /* command name */
    char *name;

    /* handler callback */
    cmd_cb cb;
};

struct user {
    /* login name */
    char *name;

    /* login password */
    char *pass;

    /* file descriptor of socket */
    int fd;
};

int getfd(void *fd);

conserver *conserver_init(fkmserver *s, char *motd)
{
    conserver *c = malloc(sizeof(conserver));

    c->slave   = s;
    c->control = fkmserver_init(CTL_PORT, getfd);

    c->users = fkmlist_create();
    c->cmds  = fkmlist_create();

    return c;
}

int conserver_reg_user(conserver *c, char *name, char *pass)
{
    struct user *u = malloc(sizeof(struct user));

    u->name = name;
    u->pass = pass;

    c->users = fkmlist_append(c->users, u);

    return 0;
}

int conserver_reg_cmd(conserver *c, char *name, cmd_cb cb)
{
    struct command *cmd = malloc(sizeof(struct command));

    cmd->name = name;
    cmd->cb = cb;

    c->cmds = fkmlist_append(c->cmds, cmd);

    return 0;
}

int conserver_poll(conserver *c)
{
    int i, fdcount, *buf;
    struct pollfd *pfds;

    fdcount = c->slave->connections + c->control->connections + 2;

    pfds = alloca(fdcount * sizeof(struct pollfd));
    buf  = alloca(fdcount * sizeof(int));

    fkmserver_exportfds(c->slave, buf);
    fkmserver_exportfds(c->control, buf + c->slave->connections);

    pfds[0].fd = c->slave->socket;
    pfds[1].fd = c->control->socket;
    pfds[0].events = pfds[1].events = POLLIN;

    for (i = 2; i < fdcount; i++) {
        pfds[i].fd     = buf[i - 2];
        pfds[i].events = POLLIN | POLLRDHUP;
    }

    if (poll(pfds, fdcount, -1) == -1) {
        perror("conserver poll");
        return 0;
    }

    if (pfds[0].revents & POLLIN)
        return 0;

    for (i = 2; i < c->slave->connections + 2; i++) {
        if (pfds[i].revents & POLLIN)
            return i - 1;
        else if (pfds[i].revents & POLLRDHUP)
            return -i + 1;
    }

    /* handle control shit */

    return conserver_poll(c);
}

int getfd(void *fd)
{
    return (int)fd;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
