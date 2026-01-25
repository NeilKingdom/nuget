#include "main.h"

static volatile sig_atomic_t win_resize_pending = false;

/**
 * @brief Initializes the curses window for use in the program.
 * @since 11-03-2024
 */
static void setup_curses(void) {
    // Initialize default window (stdscr)
    initscr();

    if (!has_colors()) {
        fprintf(stderr, "Sorry boomer. You'll need a terminal written later than 1970 to run this program\n");
        quit_nuget(NULL, NULL, EXIT_FAILURE);
    }

    // Enable 255 color mode
    start_color();

    // Color pairs
    init_pair(DEFAULT,          DEFAULT_PAIR);
    init_pair(PRIMARY,          PRIMARY_PAIR);
    init_pair(PRIMARY_INV,      PRIMARY_INV_PAIR);
    init_pair(SECONDARY,        SECONDARY_PAIR);
    init_pair(SECONDARY_INV,    SECONDARY_INV_PAIR);
    init_pair(CURSOR,           CURSOR_PAIR);

    // TTY setup
    raw();                  // Disables input buffering and POSIX signals
    curs_set(0);            // Hide cursor
    noecho();               // Don't echo characters typed in the TTY
    keypad(stdscr, true);   // Enable special keys e.g. function keys

    clear();
}

/**
 * @brief Executes the callback function associated with any keychords matching the user's input sequence.
 * @since 01-09-2024
 * @param table A reference to the table context object
 * @param input_seq The input sequence entered by the user
 * @param input_seq_off The current offset into the user_input_seq buffer
 */
static void exec_kc_callback(TableCtx_t *table, keysym_t *input_seq, int *input_seq_off) {
    size_t num_bindings;
    KeyChord_t *bindings = NULL;
    switch (curr_mode) {
        case NORMAL:
            num_bindings = sizeof(norm_bindings) / sizeof(norm_bindings[0]);
            bindings = norm_bindings;
            break;
        case INSERT:
            num_bindings = sizeof(ins_bindings) / sizeof(ins_bindings[0]);
            bindings = ins_bindings;
            break;
        case VISUAL:
            num_bindings = sizeof(vis_bindings) / sizeof(vis_bindings[0]);
            bindings = vis_bindings;
            break;
    }

    int binding_seq_len;
    bool prefix_match_found = false;
    for (unsigned i = 0; i < num_bindings; ++i) {
        KeyChord_t curr_binding = bindings[i];

        // Get current binding keychord sequence length
        binding_seq_len = 0;
        while (curr_binding.seq[binding_seq_len] != XK_NULL) {
            binding_seq_len++;
        }

        // Check if first n characters of the binding's sequence match what was entered
        if (*input_seq_off < binding_seq_len && memcmp(input_seq, curr_binding.seq, *input_seq_off) == 0) {
            prefix_match_found = true;
            continue;
        }

        // If match found, execute
        if (*input_seq_off == binding_seq_len && memcmp(input_seq, curr_binding.seq, *input_seq_off) == 0) {
            curr_binding.func(table, curr_binding.args);
            *input_seq_off = 0;
            memset(input_seq, XK_NULL, KEYSTROKE_MAX);
            return;
        }
    }

    // No bindings match the input sequence, so reset the input sequence
    if (!prefix_match_found) {
        *input_seq_off = 0;
        memset(input_seq, XK_NULL, KEYSTROKE_MAX);
    }
}

/**
 * @brief Blocks waiting for user input from stdscr and then adds it to input_seq for processing.
 * @since 01-09-2024
 * @param table A reference to the table context object
 * @param input_seq The input sequence buffer that keeps track of user input
 * @param input_seq_off The current offset into the input_seq buffer
 */
static void handle_input(TableCtx_t *table, keysym_t *input_seq, int *input_seq_off) {
    keysym_t c = (keysym_t)getch();
    if (*input_seq_off == KEYSTROKE_MAX) {
        input_seq_off = 0;
        memset(input_seq, XK_NULL, KEYSTROKE_MAX);
        return;
    }

    input_seq[(*input_seq_off)++] = c;
    exec_kc_callback(table, input_seq, input_seq_off);
}

static void set_win_resize_pending(int signo) {
    (void)signo;
    win_resize_pending = true;
}

int main(int argc, char **argv) {
    int status;
    int input_seq_off = 0;
    keysym_t input_seq[KEYSTROKE_MAX] = { XK_NULL };

    // Setup listener for window resize event
    struct sigaction act = { 0 };
    act.sa_handler = set_win_resize_pending;
    sigemptyset(&act.sa_mask);
    sigaction(SIGWINCH, &act, NULL);

    TableCtx_t *table = create_table_ctx();
    if (table == NULL) {
        quit_nuget(NULL, NULL, EXIT_FAILURE);
    }

    ncsv_t csv_ctx;
    status = csv_init(&csv_ctx, (CSV_APPEND_NULL | CSV_EMPTY_IS_NULL));
    if (status != 0) {
        fprintf(stderr, "Failed to initialize the CSV parser");
        quit_nuget(table, NULL, EXIT_FAILURE);
    }

    setup_curses();

    // TODO: Action should be triggered by user
    read_csv_data(table, &csv_ctx, "/home/neil/devel/projects/nuget/2024.csv");

    // Program loop
    while (true) {
        refresh_table(table);
        handle_input(table, input_seq, &input_seq_off);
        if (win_resize_pending) {
            struct winsize ws;
            ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
            resizeterm(ws.ws_row, ws.ws_col);
            table->nvis_rows = ws.ws_row;
            table->cell_width = ws.ws_col / table->nvis_cols;
            refresh_table(table);
            win_resize_pending = false;
        }
    }
}
