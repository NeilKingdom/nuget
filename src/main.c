#include "main.h"

/* Externs */
uint8_t cell_cwidth = 0;

/**
 * @brief Initializes the curses window for use in the program.
 * @since 11-03-2024
 */
static void setup_curses(void) {
    /* Initialize default window (stdscr) */
    initscr();

    if (!has_colors()) {
        endwin();
        fprintf(stderr, "Sorry boomer. You'll need a terminal written later than 1970 to run this program\n");
        exit(EXIT_FAILURE);
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
    noecho();               /* Don't echo typed characters to the TTY */
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
    unsigned i, nmatches;
    size_t bsize, bseq_len, seq_len;
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

int main(int argc, char **argv) {
    int seq_idx = 0;
    TableCtx_t *table = NULL;
    keysym_t input_seq[KEYSTROKE_MAX] = { XK_NULL };

    /* Initialization */
    setup_curses();
    table = create_table_ctx();

    /* Program loop */
    while (true) {
        clear();
        refresh_table(table);
        handle_input(table, input_seq, &seq_idx);
    }
}
