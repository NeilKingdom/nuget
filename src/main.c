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
    TableCtx_t *table,
    const keysym_t *seq
) {
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
        if (memcmp(binds[i].seq, seq, KEYSTROKE_MAX) == 0) {
            kc_callback func = binds[i].func;
            void **args = binds[i].args;
            func(table, args);
            return;
        }
    }
}

static bool match_count(const keysym_t *seq) {
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
        if (memcmp(binds[i].seq, seq, KEYSTROKE_MAX) == 0) {
            return true;
        }
    }

    return false;
}

static bool block;

static void timer_done(union sigval sig) {
    block = false;
}

static void handle_input(TableCtx_t *table) {
    int c;
    size_t bsize;
    unsigned i = 0;
    keysym_t seq[KEYSTROKE_MAX] = { XK_NULL };

    int status;
    timer_t timer_id;
    block = true;

    struct itimerspec tspec = {
        .it_interval = { .tv_sec = 0, .tv_nsec = 100 * 1000 * 1000 },
        .it_value    = { .tv_sec = 0, .tv_nsec = 100 * 1000 * 1000 }
    };

    struct sigevent evt = {
        .sigev_notify = SIGEV_THREAD,
        .sigev_notify_function = timer_done
    };

    status = timer_create(CLOCK_MONOTONIC, &evt, &timer_id);
    if (status != 0) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }

    status = timer_settime(timer_id, 0, &tspec, NULL);
    if (status != 0) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }

    /* Buffer keystrokes */
    while (i < KEYSTROKE_MAX) {
        if (!block) {
            break;
        }

        seq[i++] = (keysym_t)getch();
        timer_settime(timer_id, 0, &tspec, NULL);
        usleep(100 * 1000);
    }

    /* Find and execute callback if a matching bind exists */
    exec_kc_callback(table, seq);

    timer_delete(timer_id);
}

int main(int argc, char **argv) {
    TableCtx_t *table = NULL;

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
