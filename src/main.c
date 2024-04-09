#include "table.h"
#include "curses_helpers.h"
#include "keyboard.h"

/* Externs */
uint8_t cell_cwidth = 0;
bool    quit_nuget = false;

static void cleanup_curses(void) {
    endwin();
}

static void setup_curses(void) {
    initscr(); /* Initialize default window (stdscr) */

    if (!has_colors()) {
        fprintf(stderr, "Sorry boomer. You'll need a terminal written later than 1970 to run this program\n");
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
    raw();                  /* Disables input buffering and POSIX signals */
    curs_set(0);            /* Set cursor invisible */
    noecho();               /* Don't echo typed characters to the TTY */
    keypad(stdscr, true);   /* Enable special keys e.g. function keys */

    clear(); /* Clear the screen */
}

static void exec_kc_callback(
    pTableCtx_t restrict table,
    keysym_t *cbuf,
    KeyChord_t *bindings,
    const size_t bindings_sz
) {
    unsigned i, j;
    bool is_equal;

    for (i = 0; i < bindings_sz; ++i) {
        is_equal = false;

        for (j = 0; j < KEYSTROKE_MAX; ++j) {
            if (bindings[i].seq[j] != cbuf[j]) {
                is_equal = false;
                break;
            }

            is_equal = true;
        }

        if (is_equal) {
            kc_callback func = bindings[i].func;
            void **args = bindings[i].args;
            func(table, args);
        }
    }
}

static void handle_keystroke(pTableCtx_t restrict table) {
    int c;
    unsigned i = 0;
    size_t bindings_sz;
    keysym_t cbuf[KEYSTROKE_MAX] = { 0 };

    /* Queue keystrokes */
    while (i++ < KEYSTROKE_MAX && (c = getch()) != ERR) {
        cbuf[i] = (keysym_t)c;
        timeout(100);
    }

    timeout(0);

    /* Find and execute callback for the matching binding */

    switch (curr_mode) {
        case NORMAL:
            bindings_sz = sizeof(norm_bindings) / sizeof(norm_bindings[0]);
            exec_kc_callback(table, cbuf, norm_bindings, bindings_sz);
            break;
        case INSERT:
            bindings_sz = sizeof(ins_bindings) / sizeof(ins_bindings[0]);
            exec_kc_callback(table, cbuf, ins_bindings, bindings_sz);
            break;
        case VISUAL:
            bindings_sz = sizeof(vis_bindings) / sizeof(vis_bindings[0]);
            exec_kc_callback(table, cbuf, vis_bindings, bindings_sz);
            break;
    }
}

int main(int argc, char **argv) {
    pTableCtx_t table = NULL;

    /* Initialization */
    setup_curses();
    table = create_table_ctx();

    /* Main program loop */
    do {
        redraw_table(table);
        handle_keystroke(table);
    } while (!quit_nuget);

    /* Cleanup */
    destroy_table_ctx(table);
    cleanup_curses();

    return EXIT_SUCCESS;
}
