#include "../include/table.h"
#include "../include/keyboard.h"

/* Externs */
uint8_t cell_cwidth = 0;

static void setup_curses(void) {
    initscr();              /* Initialize stdscr */
    raw();                  /* Key chords are not interpreted */
    curs_set(0);            /* Set cursor invisible */
    noecho();               /* Don't print typed characters */
    start_color();          /* Enable 255 color mode */
    keypad(stdscr, TRUE);   /* Enable special keys e.g. function keys */
    clear();                /* Clear the screen */

    /* Color pairs */
    init_pair(1, COLOR_WHITE, COLOR_YELLOW);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
}

static void cleanup_curses(void) {
    endwin();
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

    draw_row_ids(table);
    draw_col_ids(table);

    move(0, 0); /* Move cursor to top left */
    chgat(cell_cwidth, A_BOLD, 1, NULL);
    redraw_table(table);

    /* Main program loop */
    while ((c = getch()) != 'q') {
        handle_input(table, c);
        redraw_table(table);
    }

    /* Cleanup */
    destroy_table_ctx(table);
    cleanup_curses();

    return EXIT_SUCCESS;
}
