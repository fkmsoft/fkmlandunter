/* checks.h
 *
 * this header announces all libcheck suites
 */

#ifndef _TESTS_CHECKS_H
#define _TESTS_CHECKS_H

#define _XOPEN_SOURCE 500
#include <check.h>
#include <string.h>

#include "../server/new/fkmlist.h"
#include "../clients/communication.h"
#include "../clients/sdl/config.h"

Suite *fkmlist_suite (void);
Suite *communication_suite (void);
Suite *sdl_config_suite (void);

#endif /* _TESTS_CHECKS_H */
