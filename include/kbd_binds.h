#ifndef KBD_H
#define KBD_H

#include <X11/keysym.h>

#include "common.h"
#include "table.h"
#include "helpers.h"

#define KEYSTROKE_MAX 10
#define XK_NULL 0

typedef uint16_t keysym_t;
typedef void (*kc_callback)(TableCtx_t *table, void **args);

typedef enum {
    NORMAL,
    INSERT,
    VISUAL
} Mode_t;

static Mode_t curr_mode = NORMAL;

typedef struct {
    keysym_t    seq[KEYSTROKE_MAX]; /* The sequence of keys that make up the chord (NULL-terminated) */
    kc_callback func;               /* A callback to be performed when the sequence is completed */
    void       *args[8];            /* Argument list for the function */
} KeyChord_t;

/*** Static declarations ***/

static Direction_t left     = LEFT;
static Direction_t right    = RIGHT;
static Direction_t up       = UP;
static Direction_t down     = DOWN;

/*** Callbacks ***/

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

static void insert_mode(TableCtx_t *table, void **args) {
    curr_mode = INSERT;
}

static void normal_mode(TableCtx_t *table, void **args) {
    curr_mode = NORMAL;
}

static void visual_mode(TableCtx_t *table, void **args) {
    curr_mode = VISUAL;
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

static void quit_nuget_wrapper(TableCtx_t *table, void **args) {
    /* TODO: Doesn't cleanup csv parser */
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

/* Normal mode bindings */
static KeyChord_t norm_bindings[] = {
    { .seq = { XK_i, XK_NULL },       .func = insert_mode,        .args = { NULL }},
    { .seq = { XK_h, XK_NULL },       .func = move_cursor,        .args = { &left }},
    { .seq = { XK_j, XK_NULL },       .func = move_cursor,        .args = { &down }},
    { .seq = { XK_k, XK_NULL },       .func = move_cursor,        .args = { &up }},
    { .seq = { XK_l, XK_NULL },       .func = move_cursor,        .args = { &right }},
    { .seq = { XK_V, XK_NULL },       .func = visual_mode,        .args = { NULL }},
    { .seq = { XK_0, XK_NULL },       .func = jump_to_start,      .args = { NULL }},
    { .seq = { XK_dollar, XK_NULL },  .func = jump_to_end,        .args = { NULL }},
    { .seq = { XK_G, XK_NULL },       .func = jump_to_bottom,     .args = { NULL }},
    { .seq = { XK_g, XK_g, XK_NULL }, .func = jump_to_top,        .args = { NULL }},
    { .seq = { XK_Z, XK_Z, XK_NULL }, .func = quit_nuget_wrapper, .args = { NULL }},
    { .seq = { XK_b, XK_NULL },       .func = page_up,            .args = { NULL }},
    { .seq = { XK_f, XK_NULL },       .func = page_down,          .args = { NULL }},
};

/* Insert mode bindings */
static KeyChord_t ins_bindings[] = {
    { .seq = { XK_Escape, XK_NULL },    .func = normal_mode,    .args = { NULL }}
};

/* Visual mode bindings */
static KeyChord_t vis_bindings[] = {
    { .seq = { XK_Escape, XK_NULL },    .func = normal_mode,    .args = { NULL }},
};

#endif /* KBD_H */
