/* interface.c - The fat-headed interface for fkmlandunter */
#include "interface.h"


win_struct game_box;
win_struct chat_box;

void initialise_windows()
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    /* while((c=getch()) == KEY_DOWN || c==KEY_UP || c==KEY_LEFT || c==KEY_RIGHT) */

    int ymax, xmax, height;
    getmaxyx(stdscr, ymax, xmax);
    height = (win_percent / 100.0) * ymax;

    refresh();
    
    /* creating game window */
    game_box.output = newwin(height - 3, xmax, 0, 0);
    game_box.input  = newwin(3, xmax, height - 3, 0);
    game_box.title  = "game";
    game_box.lines  = 0;
    write_win(1, "");

    /* creating chat window */
    chat_box.output = newwin(ymax - height - 3, xmax, height, 0);
    chat_box.input  = newwin(3, xmax, ymax - 3, 0);
    chat_box.title  = "chat";
    chat_box.lines  = 0;
    write_win(2, "");
}

void destroy_windows()
{
    delwin(game_box.output);
    delwin(game_box.input);
    delwin(chat_box.output);
    delwin(chat_box.input);    
    endwin();
}

/* struct_no 1 = game_box; struct_no 2 = chat_box */
void write_win(int struct_no, char *format, ...)
{
    win_struct *win_struct;
    if (struct_no == GAME_BOX)
	win_struct = &game_box;
    if (struct_no == CHAT_BOX)
	win_struct = &chat_box;

    va_list ap;
    char *p, *sval;
    int ival;

    int x, y;
    getyx(win_struct->output, y, x);

    if (strlen(format) > 0) {
	va_start(ap, format);
	wmove(win_struct->output, win_struct->lines + 1, x);
	for (p = format; *p; p++) {
	    scroll_win(win_struct);
	    if (*p != '%') {
		if (*p == '\n') {
		    win_struct->lines++;
		    /* set curser on new line */
		    wmove(win_struct->output, win_struct->lines + 1, 1);
		} else
		    waddch(win_struct->output, *p);
		continue;
	    }
	    switch (*++p) {
	    case 'd':
            ival = va_arg(ap, int);
            wprintw(win_struct->output, "%d", ival);
            break;
	    case 's':
            for (sval = va_arg(ap, char *); *sval; sval++) {
                if (*sval == '\n') {
                win_struct->lines++;
                /* set curser on new line */
                wmove(win_struct->output, win_struct->lines + 1, 1);
                } else
                waddch(win_struct->output, *sval);
            }
            break;
	    default:
            waddch(win_struct->output, *sval);
            win_struct->lines++;
            break;
	    }
	}	
	va_end(ap);
    } else {
        /* to initialise or the like */
        box(win_struct->output, 0, 0);
        mvwprintw(win_struct->output, 0, 1, " %s output: ", win_struct->title);
        wmove(win_struct->output, 1, 1);
    }

    /* wenn fmt leer, dann input box zeichnen + löschen */
    if (strlen(format) == 0) {
        clear_input(win_struct);
    }

    wrefresh(win_struct->output);
    wrefresh(win_struct->input);
}

/* clears input window and set curser */
void clear_input(win_struct *w_struct)
{
    wmove(w_struct->input, 1, 0);
    winsertln(w_struct->input);
    box(w_struct->input, 0, 0);
    mvwprintw(w_struct->input, 0, 1, " %s input: ", w_struct->title);

    wmove(w_struct->input, 1, 1);
    wrefresh(w_struct->input);
}

/* scrolls if necessary (last line written) */
void scroll_win(win_struct *w_struct)
{
    int y, x, ymax, xmax;
    getmaxyx(w_struct->output, ymax, xmax);
    getyx(w_struct->output, y, x);

    if (w_struct->lines >= ymax - 2) {
        /* delete last line (box bottom) */
        wmove(w_struct->output, ymax - 1, 0);
        wdeleteln(w_struct->output);

        /* scroll and reprint box */
        scrollok(w_struct->output, TRUE);
        wscrl(w_struct->output, 1);
        box(w_struct->output, 0, 0);
        mvwprintw(w_struct->output, 0, 1, " %s output: ", w_struct->title);

        /* set curser on new line */
        wmove(w_struct->output, w_struct->lines, 1);

        w_struct->lines--;
    }
}

/* struct_no 1 = game_box; struct_no 2 = chat_box */
void read_win(int struct_no, char *s, int size)
{
    win_struct *w_struct;
    if (struct_no == GAME_BOX)
	w_struct = &game_box;
    if (struct_no == CHAT_BOX)
	w_struct = &chat_box;

    echo();
    curs_set(2);
    mvwgetnstr(w_struct->input, 1, 1, s, size);
    noecho();
    curs_set(0);
    clear_input(w_struct);
}

void read_chat(char *s)
{
    int x, y;
    win_struct *w_struct = &chat_box;
    getyx(w_struct->input, y, x);
    static int pos = 0;
    static char input[CHAT_BUFFER];
    int c = 0;

    echo();
    curs_set(2);
    c = mvwgetch(w_struct->input, 1, x);
    input[pos++] = c;
    if (c == '\n' || pos >= CHAT_BUFFER) {
        pos = 0;
        s = malloc(strlen(input) * sizeof(char));
        strcpy(s, input); 
        noecho();
        curs_set(0);
        clear_input(w_struct);
    }
}
/* vim: set sw=4 ts=4 et fdm=syntax: */
