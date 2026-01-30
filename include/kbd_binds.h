#ifndef KBD_BINDS_H
#define KBD_BINDS_H

#include <X11/keysym.h>

#include "common.h"
#include "table.h"
#include "helpers.h"

#define KEYSTROKE_MAX 10
#define XK_NULL 0
#define XK_Control(x) ((x) & 0x1F)

typedef int32_t keysym_t;
typedef void (*kc_callback)(TableCtx_t *table, void **args);

typedef struct {
    keysym_t    seq[KEYSTROKE_MAX]; // The sequence of keys that make up the chord (NULL-terminated)
    kc_callback func;               // A callback to be performed when the sequence is completed
    void       *args[8];            // Argument list for the function
} KeyChord_t;

typedef enum {
    NORMAL,
    INSERT,
    VISUAL,
    COMMAND
} Mode_t;

static Mode_t curr_mode = NORMAL;

/*** Forward function declarations ***/

void handle_input(TableCtx_t *table, keysym_t *input_seq, int *input_seq_off);
void exec_kc_callback(TableCtx_t *table, keysym_t *input_seq, int *input_seq_off);

/*** Static declarations ***/

static Direction_t left  = LEFT;
static Direction_t right = RIGHT;
static Direction_t up    = UP;
static Direction_t down  = DOWN;

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

static void insert_mode(TableCtx_t *table, void **args) {
    Cell_t *cell = get_cell(table, table->cursor);
    unsigned width = MIN(MAX(cell->len, table->cell_width), table->cell_width * 4);
    unsigned height = (int)ceilf((float)cell->len / (float)width);
    WINDOW *dialog = newwin(height, width, table->cursor.y, table->cursor.x * table->cell_width);
    PANEL *dialog_panel = new_panel(dialog);

    curs_set(1);
    wbkgdset(dialog, COLOR_PAIR(SECONDARY));

    char buf[256];
    size_t buf_sz = sizeof(buf);
    unsigned buf_off = cell->len;
    memcpy(buf, cell->str, cell->len);

    char c;
    do {
        width = MIN(MAX(buf_off, table->cell_width), table->cell_width * 4);
        height = MAX((int)ceilf((float)buf_off / (float)width), 1);
        wresize(dialog, height, width);
        wclear(dialog);

        for (unsigned y = 0; y < height; ++y) {
            unsigned nchars_prior = y * width;
            if (nchars_prior >= buf_off) {
                break;
            }
            mvwprintw(dialog, y, 0, "%.*s", MIN(width, buf_off - nchars_prior), buf + nchars_prior);
        }

        int cursor_x = buf_off % width;
        int cursor_y = height;
        wmove(dialog, cursor_y, cursor_x);

        top_panel(dialog_panel);
        update_panels();
        doupdate();

        c = wgetch(dialog);
        if (buf_off < buf_sz && c >= ' ' && c <= '~') {
            buf[buf_off++] = c;
        }
    } while (c != '\n' && c != 27);

    if (c == '\n') {
        set_cell_text(table, table->cursor, buf, buf_off);
    }

    del_panel(dialog_panel);
    delwin(dialog);
    update_panels();
    doupdate();
    curs_set(0);

    // Rerender in table
    render_cell(table, table->cursor, ALIGN_LEFT, true);
}

static void normal_mode(TableCtx_t *table, void **args) {
    curr_mode = NORMAL;
}

static void visual_mode(TableCtx_t *table, void **args) {
    curr_mode = VISUAL;
}

static void command_mode(TableCtx_t *table, void **args) {
    curr_mode = COMMAND;
}

static void jump_to_start(TableCtx_t *table, void **args) {
    table->abs_offset.x = 0;
    table->cursor.x = table->table_offset.x;
}

static void jump_to_end(TableCtx_t *table, void **args) {
    table->abs_offset.x = MAX_COLS - table->nvis_cols;
    table->cursor.x = table->nvis_cols - 1;
}

static void jump_to_top(TableCtx_t *table, void **args) {
    table->abs_offset.y = 0;
    table->cursor.y = table->table_offset.y;
}

static void jump_to_bottom(TableCtx_t *table, void **args) {
    table->abs_offset.y = MAX_ROWS - table->nvis_rows;
    table->cursor.y = table->nvis_rows - table->table_offset.y;
}

static void page_up(TableCtx_t *table, void **args) {
    int64_t new_offset;

    new_offset = table->abs_offset.y - table->nvis_rows + table->table_offset.y;
    new_offset = (new_offset < 0) ? 0 : new_offset;
    table->abs_offset.y = new_offset;
}

static void page_down(TableCtx_t *table, void **args) {
    uint64_t new_offset;
    const uint64_t bottom_bound = MAX_ROWS - table->nvis_rows;

    new_offset = table->abs_offset.y + table->nvis_rows - table->table_offset.y;
    new_offset = (new_offset > bottom_bound) ? bottom_bound : new_offset;
    table->abs_offset.y = new_offset;
}

static void delete_cell(TableCtx_t *table, void **args) {
    set_cell_text(table, table->cursor, NULL, 0);
}

static void change_cell(TableCtx_t *table, void **args) {
    delete_cell(table, NULL);
    insert_mode(table, NULL);
}

static void quit_nuget_wrapper(TableCtx_t *table, void **args) {
    // TODO: Doesn't cleanup csv parser
    quit_nuget(table, NULL, EXIT_SUCCESS);
}

/**
 * @brief Moves the location of the cursor within the table
 * @since 11-03-2024
 * @param table The table context object
 * @param direction The cardinal direction in which to scroll the table
 */
static void move_cursor(TableCtx_t *table, void **args) {
    Direction_t **pdir = (Direction_t**)args;
    const Direction_t direction = *pdir[0];

    switch (direction) {
        case LEFT:
            if (table->cursor.x > table->table_offset.x) {
                --table->cursor.x;
            } else if (table->cursor.x == table->table_offset.x) {
                scroll_table(table, LEFT);
            }
            break;
        case RIGHT:
            if (table->cursor.x < (uint64_t)(table->nvis_cols - table->table_offset.x)) {
                ++table->cursor.x;
            } else if (table->cursor.x == (uint64_t)(table->nvis_cols - table->table_offset.x)) {
                scroll_table(table, RIGHT);
            }
            break;
        case UP:
            if (table->cursor.y > table->table_offset.y) {
                --table->cursor.y;
            } else if (table->cursor.y == table->table_offset.y) {
                scroll_table(table, UP);
            }
            break;
        case DOWN:
            if (table->cursor.y < (uint64_t)(table->nvis_rows - table->table_offset.y)) {
                ++table->cursor.y;
            } else if (table->cursor.y == (uint64_t)(table->nvis_rows - table->table_offset.y)) {
                scroll_table(table, DOWN);
            }
            break;
    }
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

/*** Bindings ***/

// Normal mode bindings
static KeyChord_t norm_bindings[] = {
    { .seq = { XK_i, XK_NULL },             .func = insert_mode,        .args = { NULL }},
    { .seq = { XK_V, XK_NULL },             .func = visual_mode,        .args = { NULL }},
    { .seq = { XK_ColonSign, XK_NULL },     .func = command_mode,       .args = { NULL }},
    { .seq = { XK_h, XK_NULL },             .func = move_cursor,        .args = { &left }},
    { .seq = { XK_j, XK_NULL },             .func = move_cursor,        .args = { &down }},
    { .seq = { XK_k, XK_NULL },             .func = move_cursor,        .args = { &up }},
    { .seq = { XK_l, XK_NULL },             .func = move_cursor,        .args = { &right }},
    { .seq = { XK_0, XK_NULL },             .func = jump_to_start,      .args = { NULL }},
    { .seq = { XK_dollar, XK_NULL },        .func = jump_to_end,        .args = { NULL }},
    { .seq = { XK_G, XK_NULL },             .func = jump_to_bottom,     .args = { NULL }},
    { .seq = { XK_g, XK_g, XK_NULL },       .func = jump_to_top,        .args = { NULL }},
    { .seq = { XK_Control(XK_b), XK_NULL }, .func = page_up,            .args = { NULL }},
    { .seq = { XK_Control(XK_f), XK_NULL }, .func = page_down,          .args = { NULL }},
    { .seq = { XK_d, XK_w, XK_NULL },       .func = delete_cell,        .args = { NULL }},
    { .seq = { XK_d, XK_i, XK_w, XK_NULL }, .func = delete_cell,        .args = { NULL }},
    { .seq = { XK_d, XK_a, XK_w, XK_NULL }, .func = delete_cell,        .args = { NULL }},
    { .seq = { XK_c, XK_w, XK_NULL },       .func = change_cell,        .args = { NULL }},
    { .seq = { XK_c, XK_i, XK_w, XK_NULL }, .func = change_cell,        .args = { NULL }},
    { .seq = { XK_c, XK_a, XK_w, XK_NULL }, .func = change_cell,        .args = { NULL }},
    { .seq = { XK_Z, XK_Z, XK_NULL },       .func = quit_nuget_wrapper, .args = { NULL }},
    { .seq = { XK_Z, XK_Q, XK_NULL },       .func = quit_nuget_wrapper, .args = { NULL }},
};

// Insert mode bindings
static KeyChord_t ins_bindings[] = {
    { .seq = { XK_Escape, XK_NULL }, .func = normal_mode, .args = { NULL }},
};

// Visual mode bindings
static KeyChord_t vis_bindings[] = {
    { .seq = { XK_Escape, XK_NULL },    .func = normal_mode,  .args = { NULL }},
    { .seq = { XK_ColonSign, XK_NULL }, .func = command_mode, .args = { NULL }},
};

#endif
