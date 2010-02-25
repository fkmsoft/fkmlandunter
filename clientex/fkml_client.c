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

    char *input = NULL;
    int i, wcard;
    bool indeck;
    gamestr *game = create_game();

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
        write_win(GAME_BOX, "Please enter your nickname: \n");
        read_win(GAME_BOX, game->player.name, MAXNICK);

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
        write_win(GAME_BOX, "Try to login with nick %s...", game->player.name);
        send_to(fpsock, "LOGIN %s\n", game->player.name);

        /* ack? */
        input = receive_from(fpsock);
    } while (strncmp(input, "ACK", 3) != 0);
    write_win(GAME_BOX, "Login succeed\n");
    
    /* start */
    write_win(GAME_BOX, "");
    do {
        input = receive_from(fpsock);
    } while (strncmp(input, "START ", 6) != 0);
    parse_start(game, input);

    write_win(GAME_BOX, "Game is starting with %d players\n", game->count);


    /* STARTING GAME */
    while (game->round < game->count) {

        /* get + print deck & rings & weather*/
        while (!game->deck || !game->rings || !game->weather) {
            input = receive_from(fpsock);
            if (parse_cmd(game, input) < 0)
                write_win(GAME_BOX, "server cmd not valid:%s\n", input);
        }
        print_deck(game);
        print_rings(game);
        print_weathercards(game);

        /* play */
        write_win(GAME_BOX, "Please choose a card from your deck: ");
        indeck = false;
        while (!indeck) { 
            read_win(GAME_BOX, input, 2);
            wcard = atoi(input);
            for (i = 0; i < 12; i++)
                if (wcard > 0 && wcard == game->player.weathercards[i])
                    indeck = true;
        }
        write_win(GAME_BOX, "%d\nPlease wait till the other players acted\n", wcard);
        do {
            send_to(fpsock, "PLAY %d\n", wcard);
            input = receive_from(fpsock);
        } while (strncmp(input, "ACK ", 4) != 0);

        /* get + print wlevel */
        while (!game->wlevel) {
            input = receive_from(fpsock);
            if (parse_cmd(game, input) < 0)
                write_win(GAME_BOX, "server cmd not valid:%s\n", input);
        }
        print_wlevel(game);

        /* points? */
        input = receive_from(fpsock);
        if (parse_cmd(game, input) < 0)
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
    int i;
    write_win(GAME_BOX, "Your current deck is:\n");
    for (i = 0; i < 12; i++)
        if (game->player.weathercards[i] > 0)
            write_win(GAME_BOX, "%d, ", game->player.weathercards[i]);
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


/* vim: set sw=4 ts=4 et fdm=syntax: */
