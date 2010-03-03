/* testenv.c
 *
 * Test environment for our children
 *
 * (c) Fkmsoft, 2010
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

#include "../../communication.h"

int main(int argc, char **argv) {
    if (argc < 5 || argc > 8) {
	printf("Usage: %s [passes] [SERVER] [BOT1] [BOT2] [BOT3] ([BOT4] [BOT5])\n", *(argv));
	exit(EXIT_FAILURE);
    }
    int i, j, k, status, passes = atoi(*++(argv)), botcount = argc - 3;
    char *server = *++(argv);
    char *bot[botcount];
    pid_t pid[botcount];
    pid_t wpid;

    for (i = 0; i < botcount; i++)
	bot[i] = *++(argv);

    for (i = 0; i < passes; i++) {
        /* starting server */
	switch (wpid = fork()) {
	    case -1:
		printf("Fehler in fork()\n");
	        exit(EXIT_FAILURE);
		break;
	    case 0:
		if (execl(server, "server", 0L) < 0)
		    printf("Error bei \"execl %s\": %s\n", server, strerror(errno));
		exit(EXIT_FAILURE);
		break;
	    default:
		break;
	}
	/* starting clients */
	for (j = 0; j < botcount; j++) {
	    switch (pid[j] = fork()) {
		case -1:
		    printf("Fehler in fork()\n");
		    exit(EXIT_FAILURE);
		    break;
		case 0:
            printf("Running execl(%s, \"\")\n", bot[j]);
            errno = 0;
		    if (execl(bot[j], "random", 0L) < 0)
			printf("Error bei \"execl %s\": %s\n", bot[j], strerror(errno));
		    exit(EXIT_FAILURE);
		    break;
		default:
		    /*printf("%d: ICH STARTE NUR DIE SCHLEIFE NEU!\n", getpid());*/
		    break;
	    }
	}	    
	for (j = 0; j < botcount; j++) {
	    wpid = wait(&status);
	    for (k = 0; k < botcount; k++) {
		if (wpid == pid[k])
		    printf("%d: Bot nr.%d returnierte %d\n", getpid(), j, status);
	    }	
	}
    }
    exit(EXIT_SUCCESS);
}
