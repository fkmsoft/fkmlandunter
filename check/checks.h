/* checks.h
 *
 * this header announces all libcheck suites
 */

#ifndef _TESTS_CHECKS_H
#define _TESTS_CHECKS_H

#include <check.h>

#include "../server/new/fkmlist.h"
#include "../clients/communication.h"

Suite *fkmlist_suite (void);
Suite *communication_suite (void);

#endif /* _TESTS_CHECKS_H */
