/* control.h
 *
 * The control server interface.
 *
 * (c) 2010, Fkmsoft
 */

#ifndef CONTROL_H
#define CONTROL_H

/* for POLLRDHUP and isblank() */
#define _GNU_SOURCE

#include <ctype.h>
#include <alloca.h>

#include "../fkmserver.h"
#include "../fkmlandunter_util.h"

#define CTL_PORT 23000

typedef struct conserver conserver;
typedef char *(*cmd_cb)(char **argv);

/* open a control socket for s */
conserver *conserver_init(fkmserver *s, char *motd);

/* register a user */
int conserver_reg_user(conserver *c, char *name, char *pass);

/* register a command */
int conserver_reg_cmd(conserver *c, char *name, cmd_cb cb);

/* poll underlying fkmservers, act as fkmserver_poll on slave */
int conserver_poll(conserver *c);

#endif /* CONTROL_H */

/* vim: set sw=4 ts=4 et fdm=syntax: */
