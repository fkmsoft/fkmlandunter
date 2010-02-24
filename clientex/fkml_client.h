/* fkml_client.h
 *
 * fkmlandunter client
 *
 * (c) Fkmsoft, 2010
 */

#ifndef FKML_CLIENT_H
#define FKML_CLIENT_H

/* fdopen */
#define _POSIX_SOURCE

#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "interface.h"
#include "communication.h"

#define BUF_SIZE    (1024)
#define MAXNICK     (30)

#endif
/* vim: set sw=4 ts=4 et fdm=syntax: */
