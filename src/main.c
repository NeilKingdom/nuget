#include "table.h"
#include "curses_helpers.h"
#include "keyboard.h"
#include <ncurses.h>

/* Externs */
uint8_t cell_cwidth = 0;

static void cleanup_curses(void) {
    endwin();
}

static void setup_curses(void) {
    initscr(); /* Initialize default window (stdscr) */

    if (!has_colors()) {
        fprintf(stderr, "Sorry boomer. You'll need a terminal written later than 1970 for this program\n");
        cleanup_curses();
        exit(EXIT_FAILURE);
    } else {
        start_color(); /* Enable 255 color mode */
    }

    /* Color pairs */
    init_pair(DEFAULT, DEFAULT_PAIR);
    init_pair(PRIMARY, PRIMARY_PAIR);
    init_pair(PRIMARY_INV, PRIMARY_INV_PAIR);
    init_pair(SECONDARY, SECONDARY_PAIR);
    init_pair(SECONDARY_INV, SECONDARY_INV_PAIR);
    init_pair(CURSOR, CURSOR_PAIR);

    /* TTY setup */
    raw();                  /* Key chords are not interpreted */
    curs_set(0);            /* Set cursor invisible */
    noecho();               /* Don't print typed characters */
    keypad(stdscr, true);   /* Enable special keys e.g. function keys */

    clear(); /* Clear the screen */
}

static void handle_input(pTableCtx_t table, char c) {
    switch (c) {
        case XK_h:
            move_cursor(table, LEFT);
            break;
        case XK_j:
            move_cursor(table, DOWN);
            break;
        case XK_k:
            move_cursor(table, UP);
            break;
        case XK_l:
            move_cursor(table, RIGHT);
            break;
    }
}

int main(int argc, char **argv) {
    char c;
    pTableCtx_t table = NULL;

    /* Initialization */
    setup_curses();
    table = create_table_ctx();

    /* Main program loop */
    do {
        redraw_table(table);
        handle_input(table, c);
    }
    while ((c = getch()) != 'q');

    /* Cleanup */
    destroy_table_ctx(table);
    cleanup_curses();

    return EXIT_SUCCESS;
}
