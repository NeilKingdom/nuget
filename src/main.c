#include "main.h"

static volatile sig_atomic_t win_resize_pending = false;

static void set_win_resize_pending(int signo) {
    (void)signo;
    win_resize_pending = true;
}

/**
 * @brief Initializes the curses window for use in the program.
 * @since 11-03-2024
 */
static void setup_curses(void) {
    // Initialize default window (stdscr)
    initscr();

    if (!has_colors()) {
        fprintf(
            stderr,
            "Sorry boomer. You'll need a terminal "
            "written later than 1970 to run this program\n"
        );
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
    //read_csv_data(table, &csv_ctx, "/home/neil/devel/projects/nuget/2024.csv");

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
