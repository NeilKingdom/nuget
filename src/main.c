#include "../include/main.h"
#include "../include/table.h"
#include "../include/curses_helpers.h"

void setup_curses(void) {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    start_color();

    init_pair(1, COLOR_CYAN, COLOR_WHITE);
    init_pair(2, COLOR_BLUE, COLOR_WHITE);
}

void cleanup_curses(void) {
    endwin();
}

void handle_input(tableCtx_t table, char c) {
    switch (c) {
        case 'h':
            move_cursor(table, LEFT);
            draw_cell(table, table->cursor, true);
            refresh();
            break;
        case 'j':
            move_cursor(table, UP);
            draw_cell(table, table->cursor, true);
            refresh();
            break;
        case 'k':
            move_cursor(table, DOWN);
            draw_cell(table, table->cursor, true);
            refresh();
            break;
        case 'l':
            move_cursor(table, RIGHT);
            draw_cell(table, table->cursor, true);
            refresh();
            break;
    }
}

int main(int argc, char **argv) {
    char c;
    tableCtx_t table = NULL;

    /* Initialization */
    setup_curses();
    table = create_table_ctx();

    update_cell_value(table, "cell 1", (point_t){ 3, 8 });
    update_cell_value(table, "cell 2", (point_t){ 4, 3 });
    update_cell_value(table, "cell 3", (point_t){ 8, 9 });

    /* Main program loop */
    while ((c = getch()) != 'q') {
        handle_input(table, c);
    }

    /* Cleanup */
    destroy_table_ctx(table);
    cleanup_curses();

    return EXIT_SUCCESS;
}
