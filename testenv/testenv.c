/* testenv.c
 *
 * Test environment for our children
 *
 * (c) Fkmsoft, 2010
 */

/* for sigaction */
#define _POSIX_SOURCE
/* for killpg */
#define _BSD_SOURCE

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

/* for sleep */
#include <unistd.h>

/* for sigaction() and killpg() */
#include <signal.h>

/* for getopt */
#include <getopt.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
/*#include "../../communication.h"*/

#define PIDFILE (".testenv.pids")

void kill_chldrn(void);
void kill_children(int s);

int main(int argc, char **argv) {
    int i, j, k = 1, status, passes = 1, botcount;
    bool debug = false, silent = false;
    char *server = "./server";
    char *silentstr = "", *debugstr = "", *passstr = "1";
    while ((i = getopt(argc, argv, "s:p:dq")) != -1) {
        k++;
        switch (i) {
            case 'p':
                k++;
                passes = atoi(optarg);
                passstr = optarg;
                break;
            case 'q':
                silent = true;
                silentstr = "-s";
                break;
            case 'd':
                debug = true;
                debugstr = "-d";
                break;
            case 's':
                k++;
                server = optarg;
                break;
            case '?':
            case ':':
                printf("Usage: %s [-q] [-d] [-p passes] [-s SERVER] [BOT1] [BOT2] [BOT3] ([BOT4] [BOT5])\n", *(argv));
                exit(EXIT_FAILURE);
            default:
                puts("foo");
        }
    }

    if ((botcount = argc - k) > 5) {
        puts("Too many bots");
        exit(EXIT_FAILURE);
    }

    char *bot[botcount];
    int botpts[botcount];
    pid_t pid[botcount+1];
    pid_t wpid;

    for (i = 0; i < botcount; i++) {
        bot[i] = argv[i+k];
        botpts[i] = 0;
        if (debug)
            printf("bot %d: %s\n", i, bot[i]);
    }

    /* set up stuff to occur on exit */
    /* atexit(kill_chldrn); */ /* not needed */
    /* calloc() gives zeroed memory */
    struct sigaction *sa = calloc(1, sizeof(struct sigaction));
    sa->sa_handler = kill_children;
    if (-1 == sigaction(SIGTERM, sa, 0))
        perror("SIGTERM");
    if (-1 == sigaction(SIGINT, sa, 0))
        perror("SIGINT");
    /* make sure this runs in a process group for itself 
     * so that it only brings down its children when killed,
     * not other random processes */
    /* setgpid(0, 0); */

    /* start forking */
    printf("Starting %d passes of tests with %d bots\n", passes, botcount);

    char botnumstr[2] = "3";
    /* starting server */
    switch (pid[botcount] = fork()) {
        case -1:
            printf("Fehler in fork()\n");
            exit(EXIT_FAILURE);
            break;
        case 0:
            /*fprintf(pidfile, "%d\n", getpid());
            fflush(pidfile);*/
            if (botcount > 3)
                botnumstr[0] = '0' + botcount;
            if (execl(server, "server", "-l", botnumstr,
                       "-g", passstr, debugstr, 0L) < 0)
                printf("Error bei \"execl %s\": %s\n",
                        server, strerror(errno));
            _exit(EXIT_FAILURE);
            break;
        default:
            /* wait a while to make sure server is running when clients
             * start to connect */
            /* sleep(1); */
            usleep(100000);
            break;
    }

    for (i = 0; i < passes; i++) {
        /* starting clients */
        /* char *botname = "PLAYER X"; */
        char botname[9] = { 'P', 'l', 'a', 'y', 'e', 'r', '_', '_', 0 };
        for (j = 0; j < botcount; j++) {
            switch (pid[j] = fork()) {
                case -1:
                    printf("Fehler in fork()\n");
                    exit(EXIT_FAILURE);
                    break;
                case 0:
                    /*fprintf(pidfile, "%d\n", getpid());
                    fflush(pidfile);*/
                    botname[7] = j + '0';
                    if (debug)
                        printf("Running execl(\"%s\", \"random\", \"%s\","
                            " \"%s\", \"-n\", \"%s\", 0L)\n",
                                bot[j], debugstr, silentstr, botname);
                    if (execl(bot[j], "random", debugstr, silentstr,
                                "-n", botname, 0L) < 0)
                        perror("execl");
                    _exit(EXIT_FAILURE);
                    break;
                default:
                    break;
            }
        }            

        for (j = 0;
                (i == passes - 1) ? j < botcount + 1 : j < botcount;
                j++) {
            bool foo = false;
            int l;
            wpid = wait(&status);
            if (j == 0)
                printf("\nPass %04d summary:\n"
                        "------------------\n", i);
            for (k = 0;
                    (i == passes - 1) ? k < botcount + 1 : k < botcount;
                    k++)
                if (wpid == pid[k] && k < botcount) {
                    printf("%d (bot #%d)", wpid, k);
                    l = k;
                    foo = true;
                }
            if (!foo) {
                if (wpid == pid[botcount])
                    printf("%d (server)", wpid);
                else
                    printf("invalid child %d", wpid);
            }

            if (WIFEXITED(status)) {
                k = WEXITSTATUS(status);
                if (foo) {
                    if (k > 200)
                        k -= 0x100;
                    botpts[l] += k;
                }
                printf(" returned %d\n", k);
            } else if (WIFSIGNALED(status))
                printf(" killed by signal %d\n", WTERMSIG(status));
        }
    }

    puts("\nOverall summary:\n"
            "----------------");
    for (i = 0; i < botcount; i++) {
        printf("bot #%d: %d points, %.2f on average\n",
                i, botpts[i], (double)botpts[i]/(double)passes);
    }

    exit(EXIT_SUCCESS);
}

void kill_chldrn(void) {
    kill_children(-1);
}

void kill_children(int s) {
    printf("killig children (sig %d)\n", s);
    killpg(getpgrp(), SIGKILL);
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
