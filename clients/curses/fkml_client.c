/* fkml_client.c - the real one...fkml_client */
#include "fkml_client.h"

#define DEBUG 0

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

    char *input = NULL;
    int i, status;
    bool indeck;
    gamestr *game = create_game();

    /* creating interface */
    initialise_windows();
    write_win(CHAT_BOX, "Chat is disabled...\n");

    /* creating socket */
    sock_num = create_sock();
    if (sock_num < 0)
        error_exit(sock_num, "Error creating socket");
    fpsock = fdopen(sock_num, "a+");
    setbuf(fpsock, NULL);

    /* LOGIN */
    do {
        /* nick */
        write_win(GAME_BOX, "Please enter your nickname: \n");
        read_win(GAME_BOX, game->player.name, MAXNICK);

        /* connect */
        write_win(GAME_BOX, "IP = %s, Port = %d. Trying to connect...", ip, port);
        if (connect_socket(sock_num, ip, port) < 0)
            error_exit(sock_num, "Can't connect to server");
        write_win(GAME_BOX, "Connecting succeed\n");

        /* login */
        write_win(GAME_BOX, "Try to login with nick %s...", game->player.name);
        send_to(fpsock, "LOGIN %s\n", game->player.name);

        /* ack? */
        input = receive_from(fpsock);
    } while (strncmp(input, "ACK", 3) != 0);
    write_win(GAME_BOX, "Login succeed\n");
    
    /* START */
    write_win(GAME_BOX, "Do you want to start the game now? (y)es/(n)o: ");
    read_win(GAME_BOX, input, 1);
    write_win(GAME_BOX, "%s\n", input);
    if (input[0] == 'y')
        send_to(fpsock, "START\n");
    do {
        input = receive_from(fpsock);
    } while (strncmp(input, "START ", 6) != 0);
    parse_start(game, input);

    write_win(GAME_BOX, "Game is starting with %d players\n", game->count);

    /* STARTING GAME */
    while (game->round < game->count) {
        /* get + print deck & rings & weather*/
        while (!game->deck || !game->rings || !game->weather) {
            status = poll_input(sock_num, 0);
            if (status == -1)
                error_exit(sock_num, "Server closed socket");
            if (status == -2)
                error_exit(sock_num, "Polling failed");
            if (status == sock_num) {
                input = receive_from(fpsock);
                if (DEBUG)
                    write_win(GAME_BOX, "Input from socket:%s\n", input);
                if (parse_cmd(game, input) < 0 && DEBUG)
                    write_win(GAME_BOX, "server cmd not valid:%s\n", input);
                if (game->message)
                    print_message(game);
            } else {
                input = read_chat();
                if (input != NULL) {
                    write_win(CHAT_BOX, "%s: %s", game->player.name, input);
                    send_to(fpsock, "MSG %s\n", input);
                }
            }
        }
        print_deck(game);
        /* print_rings(game); */
        print_all(game);
        print_weathercards(game);

        /* play */
        write_win(GAME_BOX, "Please choose a card from your deck: ");
        indeck = false;
        while (!indeck) { 
            read_win(GAME_BOX, input, 2);
            for (i = 0; i < 12; i++)
                if (atoi(input) > 0 && atoi(input) == game->player.weathercards[i])
                    indeck = true;
        }
        write_win(GAME_BOX, "%s\nPlease wait till the other players acted\n", input);
        do {
            send_to(fpsock, "PLAY %s\n", input);
            input = receive_from(fpsock);
        } while (strncmp(input, "ACK ", 4) != 0);

        /* get + print wlevel */
        while (!game->wlevel) {
            status = poll_input(sock_num, 0);
            if (status == -1)
                error_exit(sock_num, "Server closed socket");
            if (status == -2)
                error_exit(sock_num, "Polling failed");
            if (status == sock_num) {
                input = receive_from(fpsock);
                if (parse_cmd(game, input) < 0 && DEBUG)
                    write_win(GAME_BOX, "server cmd not valid:%s\n", input);
                else if (game->message)
                    print_message(game);
            } else {
                input = read_chat();
                if (input != NULL) {
                    write_win(CHAT_BOX, "%s: %s", game->player.name, input);
                    send_to(fpsock, "MSG %s\n", input);
                }
            }
        }
        /* print_wlevel(game); */
        /* print_all(game); */

        /* points? */
        input = receive_from(fpsock);
        if (parse_cmd(game, input) < 0 && DEBUG)
            write_win(GAME_BOX, "server cmd not valid:%s\n", input);
        if (game->points)
            print_points(game);
    }

    write_win(GAME_BOX, "END OF GAME!\n");
    getch();


    destroy_windows();
    close(sock_num);

    return 0;
}

/* bonus */
void print_deck(gamestr *game)
{
    int i, j;
    write_win(GAME_BOX, "Your current deck is:\n");
    for (j = 1; j <= 60; j++) {
        for (i = 0; i < 12; i++)
            if (game->player.weathercards[i] == j)
                write_win(GAME_BOX, "%d, ", game->player.weathercards[i]);
    }
    write_win(GAME_BOX, "\n");
    game->deck = false;
}

void print_rings(gamestr *game)
{
    int i;
    write_win(GAME_BOX, "Lifebelt ranking:\n");
    for (i = 0; i < game->count; i++)
        if (game->villain[i].lifebelts == (-1))
            write_win(GAME_BOX, "%s: drowned\n", game->villain[i].name);
        else
            write_win(GAME_BOX, "%s: %d\n", game->villain[i].name, game->villain[i].lifebelts);
    game->rings = false;
}

void print_weathercards(gamestr *game)
{
    write_win(GAME_BOX, "The new watercards are %d and %d\n", game->w_card[0], game->w_card[1]); 
    game->weather = false;
}

void print_wlevel(gamestr *game)
{
    int i;
    write_win(GAME_BOX, "The waterlevels are:\n");
    for (i = 0; i < game->count; i++) {
        if (game->villain[i].water_level == (-1))
            write_win(GAME_BOX, "%s: drowned\n", game->villain[i].name);
        else
            write_win(GAME_BOX, "%s: %d\n", game->villain[i].name, game->villain[i].water_level);
    }
    game->wlevel = false;
}

void print_points(gamestr *game)
{
    int i;
    write_win(GAME_BOX, "The score is:\n");
    for (i = 0; i < game->count; i++)
        write_win(GAME_BOX, "%s: %d\n", game->villain[i].name, game->villain[i].points);
    game->points = false;
    write_win(GAME_BOX, "A new round has been started...\n");
}

void print_message(gamestr *game)
{
    write_win(CHAT_BOX, "%s: %s\n", game->msg_from, game->msg_data);
    game->message = false; 
}

void error_exit(int sock, char *msg)
{
    close(sock);
    destroy_windows();
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

void print_all(gamestr *game)
{
    int i, j;
    player *p;

    write_win(GAME_BOX, "Name");

    for (i = 0; i < game->tabnum; i++)
        write_win(GAME_BOX, "\t");

    write_win(GAME_BOX, "Water\tBelts\tScore\n");
    for (i = 0; i < game->count; i++) {
        p = &game->villain[i];
        write_win(GAME_BOX, "%s", p->name);

        for (j = 0; j < p->tabnum; j++)
            write_win(GAME_BOX, "\t");

        write_win(GAME_BOX, "%d\t%d\t%d\n",
                  p->water_level, p->lifebelts, p->points);
    }

    game->wlevel = game->points = game->rings = false;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
