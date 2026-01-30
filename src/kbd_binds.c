#include "kbd_binds.h"

/**
 * @brief Executes the callback function associated with any keychords matching the user's input sequence.
 * @since 01-09-2024
 * @param table A reference to the table context object
 * @param input_seq The input sequence entered by the user
 * @param input_seq_off The current offset into the user_input_seq buffer
 */
void exec_kc_callback(TableCtx_t *table, keysym_t *input_seq, int *input_seq_off) {
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
        case COMMAND:
            return;
    }

    int binding_seq_len;
    bool prefix_match_found = false;
    for (unsigned i = 0; i < num_bindings; ++i) {
        bool found_match = true;
        KeyChord_t curr_binding = bindings[i];

        // Get current binding keychord sequence length
        binding_seq_len = 0;
        while (curr_binding.seq[binding_seq_len] != XK_NULL) {
            binding_seq_len++;
        }

        // Compare input against binding prefix
        for (int j = 0; j < *input_seq_off; ++j) {
            if (j >= binding_seq_len ||
                (curr_binding.seq[j] != input_seq[j] &&
                 curr_binding.seq[j] != '*')) {
                found_match = false;
                break;
            }
        }

        if (!found_match) {
            continue;
        }

        // We're matching up until this point...
        prefix_match_found = true;

        // Matched full sequence. Excecute callback!
        if (*input_seq_off == binding_seq_len) {
            curr_binding.func(table, curr_binding.args);
            *input_seq_off = 0;
            memset(input_seq, XK_NULL, KEYSTROKE_MAX);
            return;
        }
    }

    // Exhausted all possible bindings, so reset the input sequence
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
void handle_input(TableCtx_t *table, keysym_t *input_seq, int *input_seq_off) {
    keysym_t c = (keysym_t)getch();
    if (*input_seq_off == KEYSTROKE_MAX) {
        *input_seq_off = 0;
        memset(input_seq, XK_NULL, KEYSTROKE_MAX);
        return;
    }

    input_seq[(*input_seq_off)++] = c;
    exec_kc_callback(table, input_seq, input_seq_off);
}
