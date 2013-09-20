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

    /* the message of the day */
    char *motd;

    /* the prompt */
    char *prompt;

    /* the command used for logging out */
    char *logout_cmd;
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
};

struct client {
    /* login name */
    char *name;

    /* auth successful? */
    bool in;

    /* fd of socket */
    int  fd;
};

static int con_getfd(struct client *c);
static void *con_client(int fd);

static int parse_command(conserver *c, int idx);

static const char *arsch = "dogishead";

conserver *conserver_init(fkmserver *s, char *motd)
{
    conserver *c = malloc(sizeof(conserver));

    c->slave   = s;
    c->control = fkmserver_init(CTL_PORT, (void *)con_getfd);

    c->users  = fkmlist_create();
    c->cmds   = fkmlist_create();

    c->motd   = motd;
    c->prompt = "> ";

    c->logout_cmd = "exit";

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
    void *cl;
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

    /* TODO handle control shit */
    if (pfds[1].revents & POLLIN) {
        fkmserver_addnetc(c->control, con_client);
        fkmserver_cidxsend(c->control, c->control->connections - 1, "fkmlu login: ");
    }

    for (i = 2 + c->slave->connections; i < fdcount; i++) {
        if (pfds[i].revents & POLLIN) {
            parse_command(c, i - 2 - c->slave->connections);
        } else if (pfds[i].revents & POLLRDHUP) {
            cl = fkmserver_getc(c->control, i - 2 - c->slave->connections);
            fkmserver_rmidxc(c->control, i - 2 - c->slave->connections);
            free(cl);
        }
    }

    return conserver_poll(c);
}

static int con_getfd(struct client *c)
{
    return c->fd;
}

static void *con_client(int fd)
{
    struct client *c = malloc(sizeof(struct client));

    c->name = 0;
    c->in   = false;
    c->fd   = fd;

    return c;
}

static int parse_command(conserver *c, int idx)
{
    char *s, *p, *q, **argv;
    int i, k;

    struct user *usr;
    struct client *clnt;
    struct command *cmd;

    clnt = fkmlist_get(c->control->clients, idx);
    s = fkmserver_cidxrecv(c->control, idx);

    trim(s, "\r\n");

    /* check whether logged in */

    if (!clnt->name) {

        /* check for valid user name */

        for (i = 0; (usr = fkmlist_get(c->users, i)); i++) {
            if (!strcmp(usr->name, s)) {
                clnt->name = strdup(s);
                break;
            }
        }

        fkmserver_cidxsend(c->control, idx, "Password: ");

        if (!clnt->name)
            clnt->name = strdup(arsch);

    } else if (!clnt->in) {

        /* check password */
        
        for (i = 0; (usr = fkmlist_get(c->users, i)); i++)
            if (!strcmp(usr->name, clnt->name))
                /* we assume the user exists, because we checked the name before */
                break;
        
        if (clnt->name != arsch && !strcmp(usr->pass, s)) {
            fkmserver_cidxsend(c->control, idx, c->motd);
            fkmserver_cidxsend(c->control, idx, c->prompt);
            clnt->in = true;
        } else {
            fkmserver_cidxsend(c->control, idx, "Wrong password!\n");
            fkmserver_rmidxc(c->control, idx);
        }
    } else {

        /* handle command, split arguments */

        argv = alloca(strlen(s) * sizeof(char *));

        for (i = 0, p = s; *p;) {
            while (isblank(*p))
                p++;

            q = p;

            while (isgraph(*p))
                p++;

            if (p != q) {
                if (*p) {
                    *p++ = 0;
                }

                argv[i++] = q;
            }
        }

        /*argv[i++] = strdup("end");*/
        p = argv[i] = 0;
        k = i;

        /* search for command */

        if (!strcmp(argv[0], c->logout_cmd)) {
            fkmserver_rmidxc(c->control, idx);
        } else {
            for (i = 0; !p && (cmd = fkmlist_get(c->cmds, i++)); ) {
                if (!strcmp(argv[0], cmd->name)) {

                    /* execute & print result */
                    p = cmd->cb(c->slave, k, argv);
                    if (p) {
                        fkmserver_cidxsend(c->control, idx, p);
                        free(p);
                    } else {
                        p = (char *)1;
                    }
                    break;
                }
            }

            if (*argv && !p) {
                p = alloca((64 + strlen(argv[0])) * sizeof(char));
                sprintf(p, "Unknown command: %s\n", argv[0]);
                fkmserver_cidxsend(c->control, idx, p);
            }
            fkmserver_cidxsend(c->control, idx, c->prompt);
        }
    }

    /* free(s); */

    return 1;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
