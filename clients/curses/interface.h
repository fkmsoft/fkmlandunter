/* interface.h
 *
 * interface
 *
 * (c) Fkmsoft, 2010
 */

#ifndef INTERFACE_H
#define INTERFACE_H

#include <ncurses.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#define GAME_BOX    (1)
#define	CHAT_BOX    (2)
#define	win_percent (60)
#define CHAT_BUFFER (100)

typedef struct WINDOW_STRUCT {
    WINDOW *output;
    WINDOW *input;
    char *title;
    int lines;
} win_struct;

void initialise_windows();

void destroy_windows();

void write_win(int struct_no, char *format, ...);

void clear_input(win_struct *w_struct);

void scroll_win(win_struct *w_struct);

void read_win(int struct_no, char *s, int size);

char *read_chat();

#endif
/* vim: set sw=4 ts=4 et fdm=syntax: */
