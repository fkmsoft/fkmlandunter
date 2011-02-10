/* cmd.h
 *
 * Commands for the control server.
 *
 * (c) 2010, Fkmsoft
 */

#ifndef COMMAND_H
#define COMMAND_H

#include "control.h"

conserver *setup(fkmserver *s, char *motd);

char *echo(fkmserver *slv, int argc, char **argv);

#endif /* COMMAND_H */

/* vim: set sw=4 ts=4 et fdm=syntax: */
