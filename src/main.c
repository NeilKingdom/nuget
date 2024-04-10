#include "main.h"

/* Externs */
uint8_t cell_cwidth = 0;
bool    quit_nuget = false;

static void cleanup_curses(void) {
    endwin();
}

static void setup_curses(void) {
    initscr(); /* Initialize default window (stdscr) */

    if (!has_colors()) {
        cleanup_curses();
        fprintf(stderr, "Sorry boomer. You'll need a terminal written later than 1970 to run this program\n");
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
    const keysym_t *seq
) {
    bool is_equal;
    unsigned i, j;
    size_t bsize;
    KeyChord_t *binds = NULL;

    switch (curr_mode) {
        case NORMAL:
            bsize = sizeof(norm_bindings) / sizeof(norm_bindings[0]);
            binds = norm_bindings;
            break;
        case INSERT:
            bsize = sizeof(ins_bindings) / sizeof(ins_bindings[0]);
            binds = ins_bindings;
            break;
        case VISUAL:
            bsize = sizeof(vis_bindings) / sizeof(vis_bindings[0]);
            binds = vis_bindings;
            break;
    }

    for (i = 0; i < bsize; ++i) {
        is_equal = false;

        for (j = 0; j < KEYSTROKE_MAX; ++j) {
            if (binds[i].seq[j] == XK_NULL || seq[j] == XK_NULL) {
                break;
            }

            if (binds[i].seq[j] != seq[j]) {
                is_equal = false;
                break;
            }

            is_equal = true;
        }

        if (is_equal) {
            kc_callback func = binds[i].func;
            void **args = binds[i].args;
            func(table, args);
            return;
        }
    }
}

/**
 * @brief Tries to find a binding for the current mode that begins with seq
 * @since 09-04-2024
 * @param seq The beginning sequence of keystrokes to search for in each binding
 * @returns True if a matching binding exists, or false otherwise
 */
static bool find_matching_bind(const keysym_t *seq, const size_t seq_size) {
    bool matches;
    unsigned i, j;
    size_t bsize;
    KeyChord_t *binds = NULL;

    switch (curr_mode) {
        case NORMAL:
            bsize = sizeof(norm_bindings) / sizeof(norm_bindings[0]);
            binds = norm_bindings;
            break;
        case INSERT:
            bsize = sizeof(ins_bindings) / sizeof(ins_bindings[0]);
            binds = ins_bindings;
            break;
        case VISUAL:
            bsize = sizeof(vis_bindings) / sizeof(vis_bindings[0]);
            binds = vis_bindings;
            break;
    }

    for (i = 0; i < bsize; ++i) {
        matches = true;

        for (j = 0; j < seq_size; ++j) {
            if (binds[i].seq[j] != seq[j]) {
                matches = false;
                break;
            }
        }

        if (matches) {
            break;
        }
    }

    return matches;
}

static void handle_input(pTableCtx_t restrict table) {
    int c;
    size_t bsize;
    unsigned i = 0;
    keysym_t seq[KEYSTROKE_MAX] = { 0 };

    /* Buffer keystrokes */
    while (i < KEYSTROKE_MAX && (c = getch()) != ERR) {
        seq[i++] = (keysym_t)c;
        if (!find_matching_bind(seq, i)) {
            break;
        }
        timeout(100);
    }
    timeout(0);

    /* Find and execute callback if a matching bind exists */
    exec_kc_callback(table, seq);
}

int main(int argc, char **argv) {
    pTableCtx_t table = NULL;

    /* Initialization */
    setup_curses();
    table = create_table_ctx();

    /* Main program loop */
    do {
        redraw_table(table);
        handle_input(table);
    } while (!quit_nuget);

    /* Cleanup */
    destroy_table_ctx(table);
    cleanup_curses();

    return EXIT_SUCCESS;
}
