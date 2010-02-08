#ifndef FKML_CLIENT_H
#define FKML_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <sys/select.h> // select()
#include <unistd.h>

#include "fkml_client_util.h"
#include "socketlayer.h"

/* fkmsft
 *
 * fkmlandunter prtcl:
 * -----------------------------
 * -> LOGIN name
 * <- ACK nam
 * <- START 3 nam1 nam2 nam3
 *
 * <- WEATHER 7 8
 * <- DECK 3 1 2 56
 * -> PLAY 56
 * <- ACk 56
 * <- FAIL 56
 * <- WLEVELS 7 8 0
 * <- POINTS 1 2 -1
 *
 * -> LOGOUT bye
 * <- TERMINATE fuck off
 *
 * -> MSG fu all los0rZ
 * <- MSGFROM name fu all los0rZ
 * -----------------------------
 */

#endif
