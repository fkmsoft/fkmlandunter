/* fkml_client.c - the real one...fkml_client */
#include "fkml_client.h"

int main(int argc, char *argv[])
{
    int sock_num;
    FILE *fpsock;
    int port;
    char *ip;
    if (argc != 3) {
        ip = "127.0.0.1";
        port = 1337;
    } else {
        ip = *(++argv);
        port = atoi(*++argv);
    }

    char *input;
    player *p = create_player();
    opponents *o = NULL;
    int i, wcard, round = 0, w_card[] = { 0, 0 };
    bool indeck;

    /* creating interface */
    initialise_windows();
    write_win(CHAT_BOX, "Chat is disabled...\n");

    /* creating socket */
    sock_num = create_sock();
    if (sock_num < 0) {
        destroy_windows();
        close(sock_num);
        fprintf(stderr, "Error creating socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }    
    fpsock = fdopen(sock_num, "a+");

    /* LOGIN */
    do {
        /* nick */
        p->name = malloc(MAXNICK * sizeof(char));
        write_win(GAME_BOX, "Please enter your nickname: \n");
        read_win(GAME_BOX, p->name, MAXNICK);

        /* connect */
        write_win(GAME_BOX, "IP = %s, Port = %d. Trying to connect...", ip, port);
        if (connect_socket(sock_num, ip, port) < 0) {
            destroy_windows();
            close(sock_num);
            fprintf(stderr, "Can't connect to server: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        write_win(GAME_BOX, "Connecting succeed\n");

        /* login */
        write_win(GAME_BOX, "Try to login with nick %s...", p->name);
        send_to(fpsock, "LOGIN %s\n", p->name);

        /* ack? */
        input = receive_from(fpsock);
    } while (strncmp(input, "ACK", 3) != 0);
    write_win(GAME_BOX, "Login succeed\n");
    
    /* start */
    do {
        input = receive_from(fpsock);
    } while (strncmp(input, "START ", 6) != 0);
    o = parse_start(input);

    write_win(GAME_BOX, "Game is starting with %d players\n", o->count);


    /* STARTING GAME */
    while (round < o->count) {
        /* deck */
        while (strncmp(input, "DECK ", 5) != 0) {
            write_win(GAME_BOX, "A new round has been started...\n");
            input = receive_from(fpsock);
        }
        parse_deck(p, input);
        write_win(GAME_BOX, "Your current weathercards are:\n");
        for (i = 0; i < 12; i++) {
            if (p->weathercards[i] > 0) {
                write_win(GAME_BOX, "%d, ", p->weathercards[i]);
            }
        }
        write_win(GAME_BOX, "\n");


        /* rings */
        while (strncmp(input, "RINGS ", 6) != 0)
            input = receive_from(fpsock);
        parse_rings(o, input);
        write_win(GAME_BOX, "Lifebelt ranking:\n");
        for (i = 0; i < o->count; i++) {
            if (o->p[i].lifebelts == (-1))
                write_win(GAME_BOX, "%s: drowned\n");
            else
                write_win(GAME_BOX, "%s: %d\n", o->p[i].name, o->p[i].lifebelts);
        }

        /* weather */
        while (strncmp(input, "WEATHER ", 8) != 0)
            input = receive_from(fpsock);
        parse_weather(w_card, input);
        write_win(GAME_BOX, "The new watercards are %d and %d\n", w_card[0], w_card[1]); 


        /* play */
        indeck = false;
        write_win(GAME_BOX, "Please choose a weathercard from your deck: ");
        while (!indeck) { 
            read_win(GAME_BOX, input, 2);
            wcard = atoi(input);
            for (i = 0; i < 12; i++)
                if (wcard > 0 && wcard == p->weathercards[i])
                    indeck = true;
        }
        write_win(GAME_BOX, "%d\n", wcard);
        write_win(GAME_BOX, "Please wait till the other players acted\n");
        do {
            send_to(fpsock, "PLAY %d\n", wcard);
            input = receive_from(fpsock);
        } while (strncmp(input, "ACK ", 4) != 0);


        /* wlevels */
        while (strncmp(input, "WLEVELS ", 8) != 0)
            input = receive_from(fpsock);
        parse_wlevels(o, input);
        write_win(GAME_BOX, "The waterlevels are:\n");
        for (i = 0; i < o->count; i++) {
            if (o->p[i].water_level == (-1))
                write_win(GAME_BOX, "%s: drowned\n", o->p[i].name);
            else
                write_win(GAME_BOX, "%s: %d\n", o->p[i].name, o->p[i].water_level);
        }

        /* points or next round? */
        input = receive_from(fpsock);
        if (strncmp(input, "POINTS ", 7) == 0) {
            round++;
            parse_points(o, input);
            write_win(GAME_BOX, "The score is:\n");
            for (i = 0; i < o->count; i++)
                write_win(GAME_BOX, "%s: %d\n", o->p[i].name, o->p[i].points);
        }
    }

    write_win(GAME_BOX, "END OF GAME!\n");
    getch();


    destroy_windows();
    close(sock_num);

    return 0;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
