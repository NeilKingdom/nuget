#include "main.h"

static volatile bool win_resize_pending = false;

/**
 * @brief Initializes the curses window for use in the program.
 * @since 11-03-2024
 */
static void setup_curses(void) {
    /* Initialize default window (stdscr) */
    initscr();

    if (!has_colors()) {
        fprintf(stderr, "Sorry boomer. You'll need a terminal written later than 1970 to run this program\n");
        quit_nuget(NULL, NULL, EXIT_FAILURE);
    }

    /* Enable 255 color mode */
    start_color();

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
    noecho();               /* Don't echo characters typed in the TTY */
    keypad(stdscr, true);   /* Enable special keys e.g. function keys */

    clear();
}

/**
 * @brief Executes the callback function associated with any keychords matching the user's input sequence.
 * @since 01-09-2024
 * @param table A reference to the table context object
 * @param input_seq The input sequence entered by the user
 * @param seq_idx The current offset into the input_seq buffer
 */
static void exec_kc_callback(TableCtx_t* table, keysym_t* input_seq, int* seq_idx) {
    size_t bsize, bseq_len, seq_len;
    unsigned i, nmatches;
    KeyChord_t curr_binding;
    KeyChord_t *bindings = NULL;

    switch (curr_mode) {
        case NORMAL:
            bsize = sizeof(norm_bindings) / sizeof(norm_bindings[0]);
            bindings = norm_bindings;
            break;
        case INSERT:
            bsize = sizeof(ins_bindings) / sizeof(ins_bindings[0]);
            bindings = ins_bindings;
            break;
        case VISUAL:
            bsize = sizeof(vis_bindings) / sizeof(vis_bindings[0]);
            bindings = vis_bindings;
            break;
    }

    seq_len = *seq_idx;
    for (i = 0, nmatches = 0; i < bsize; ++i) {
        curr_binding = bindings[i];

        /* Get current binding keychord sequence length */
        bseq_len = 0;
        while (curr_binding.seq[bseq_len] != XK_NULL) {
            bseq_len++;
        }

        /* Check if first n characters of the binding's sequence match what was entered */
        if (seq_len < bseq_len) {
            if (memcmp(input_seq, curr_binding.seq, seq_len) == 0) {
                nmatches++;
            }
            continue;
        } else if (seq_len == bseq_len) {
            if (memcmp(input_seq, curr_binding.seq, seq_len) == 0) {
                curr_binding.func(table, curr_binding.args);

                *seq_idx = 0;
                memset(input_seq, XK_NULL, KEYSTROKE_MAX);
                return;
            }
        }
    }

    /* No bindings match the input sequence, so reset the input sequence */
    if (nmatches == 0) {
        *seq_idx = 0;
        memset(input_seq, XK_NULL, KEYSTROKE_MAX);
    }
}

/**
 * @brief Blocks waiting for user input from stdscr and then adds it to input_seq for processing.
 * @since 01-09-2024
 * @param table A reference to the table context object
 * @param input_seq The input sequence buffer that keeps track of user input
 * @param seq_idx The current offset into the input_seq buffer
 */
static void handle_input(TableCtx_t* table, keysym_t* input_seq, int* seq_idx) {
    keysym_t c;

    c = (keysym_t)getch();
    if (*seq_idx == KEYSTROKE_MAX) {
        seq_idx = 0;
        memset(input_seq, XK_NULL, KEYSTROKE_MAX);
        return;
    }

    input_seq[(*seq_idx)++] = c;
    exec_kc_callback(table, input_seq, seq_idx);
}

static void set_win_resize_pending(int signo) {
    (void)signo;
    win_resize_pending = true;
}

static void *win_resize_hdlr(void *args) {
    int status;
    sigset_t sigset;
    struct sigaction act;
    struct winsize ws;
    TableCtx_t *table = (TableCtx_t*)args;

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGWINCH);

    act.sa_mask = sigset;
    act.sa_handler = set_win_resize_pending;

    /* Bind sigaction struct to SIGWINCH signal */
    status = sigaction(SIGWINCH, &act, NULL);
    if (status != 0) {
        perror("Failed to bind signal to handler");
        return NULL;
    }

    while (true) {
        if (win_resize_pending) {
            ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
            table->nvis_rows = ws.ws_row;
            table->cell_width = ws.ws_col / table->nvis_cols;
            resizeterm(ws.ws_row, ws.ws_col);
            refresh_table(table);

            win_resize_pending = false;
        }
    }
}

int main(int argc, char **argv) {
    int status;
    int seq_idx = 0;
    ncsv_t csv_ctx;
    pthread_t listener_thread;
    TableCtx_t *table = NULL;
    keysym_t input_seq[KEYSTROKE_MAX] = { XK_NULL };

    table = create_table_ctx();
    if (table == NULL) {
        quit_nuget(NULL, NULL, EXIT_FAILURE);
    }

    setup_curses();

    status = csv_init(&csv_ctx, (CSV_APPEND_NULL | CSV_EMPTY_IS_NULL));
    if (status != 0) {
        fprintf(stderr, "Failed to initialize the CSV parser");
        quit_nuget(table, NULL, EXIT_FAILURE);
    }

    status = pthread_create(&listener_thread, NULL, win_resize_hdlr, (void*)table);
    if (status != 0) {
        fprintf(stderr, "Failed to create pthread");
        quit_nuget(table, NULL, EXIT_FAILURE);
    }

    /* TODO: Action should be triggered by user */
    read_csv_data(table, &csv_ctx, "/home/neil/devel/projects/nuget/2024.csv");

    /* Program loop */
    while (true) {
        refresh_table(table);
        handle_input(table, input_seq, &seq_idx);
    }
}
