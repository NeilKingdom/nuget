#include "../include/main.h"
#include "../include/table.h"
#include "../include/curses_helpers.h"

/* Externs */
uint8_t cell_cwidth = 0;

void setup_curses(void) {
    initscr();              /* Initialize stdscr */
    raw();                  /* Key chords are not interpreted */
    //curs_set(0);            /* Set cursor invisible */
    noecho();               /* Don't print typed characters */
    start_color();          /* Enable 255 color mode */
    keypad(stdscr, TRUE);   /* Enable special keys e.g. function keys */
    move(0, 0);             /* Move cursor to top left */
    clear();                /* Clear the screen */

    /* Color pairs */
    init_pair(1, COLOR_WHITE, COLOR_YELLOW);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
}

void cleanup_curses(void) {
    endwin();
}

void handle_input(pTableCtx_t table, char c) {
    switch (c) {
        case 'h':
            move_cursor(table, LEFT);
            break;
        case 'j':
            move_cursor(table, DOWN);
            break;
        case 'k':
            move_cursor(table, UP);
            break;
        case 'l':
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
    chgat(cell_cwidth, A_BOLD, 1, NULL);

    update_cell_value(table, "cell 1", (point_t){ 3, 8 });
    update_cell_value(table, "cell 2", (point_t){ 4, 3 });
    update_cell_value(table, "cell 3", (point_t){ 8, 9 });

    /* Main program loop */
    while ((c = getch()) != 'q') {
        redraw_table(table);
        handle_input(table, c);
    }

    /* Cleanup */
    destroy_table_ctx(table);
    cleanup_curses();

    return EXIT_SUCCESS;
}
