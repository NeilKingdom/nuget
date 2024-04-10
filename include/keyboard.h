#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "main.h"
#include "table.h"
#include <X11/keysym.h>

#define KEYSTROKE_MAX 10
#define XK_NULL 0

typedef uint16_t keysym_t;
typedef void (*kc_callback)(pTableCtx_t table, void **args);

typedef enum {
    NORMAL,
    INSERT,
    VISUAL
} Mode_t;

static Mode_t curr_mode = NORMAL;   /* Current vim mode */

typedef struct {
    keysym_t    seq[KEYSTROKE_MAX]; /* The sequence of keys that make up the chord (NULL-terminated) */
    kc_callback func;               /* A callback to be performed when the sequence is completed */
    void       *args[8];            /* Argument list for the function */
} KeyChord_t;

/*** Callbacks ***/

static void insert_mode(pTableCtx_t restrict table, void **args) {
    curr_mode = INSERT;
}

static void normal_mode(pTableCtx_t restrict table, void **args) {
    curr_mode = NORMAL;
}

static void jump_to_top(pTableCtx_t restrict table, void **args) {
    table->offset_y = 0;
}

static void jump_to_bottom(pTableCtx_t restrict table, void **args) {
    table->offset_y = MAX_ROWS - table->vis_rows;
}

static void page_down(pTableCtx_t restrict table, void **args) {
    if (table->offset_y < MAX_ROWS - table->vis_rows) {
        table->offset_y += table->vis_rows;
    }
}

static void page_up(pTableCtx_t restrict table, void **args) {
    if (table->offset_y > table->vis_rows) {
        table->offset_y -= table->vis_rows;
    }
}

static void quit(pTableCtx_t restrict table, void **args) {
    quit_nuget = true;
}

/**
 * @brief Moves the location of the cursor within the table
 * @since 11-03-2024
 * @param table The table context object
 * @param direction The cardinal direction in which to scroll the table
 */
static void move_cursor(pTableCtx_t restrict table, void **args) {
    Direction_t **ptr = (Direction_t**)args;
    const Direction_t direction = *ptr[0];

    switch (direction) {
        case LEFT:
            if (table->cursor.x > 0) {
                --table->cursor.x;
            } else if (table->cursor.x == 0) {
                scroll_table(table, LEFT);
            }
            break;
        case RIGHT:
            if (table->cursor.x < table->vis_cols - 1) {
                ++table->cursor.x;
            } else if (table->cursor.x == table->vis_cols - 1) {
                scroll_table(table, RIGHT);
            }
            break;
        case UP:
            if (table->cursor.y > 0) {
                --table->cursor.y;
            } else if (table->cursor.y == 0) {
                scroll_table(table, UP);
            }
            break;
        case DOWN:
            if (table->cursor.y < table->vis_rows - 1) {
                ++table->cursor.y;
            } else if (table->cursor.y == table->vis_rows - 1) {
                scroll_table(table, DOWN);
            }
            break;
    }
}

/*** Bindings ***/

Direction_t left = LEFT;
Direction_t right = RIGHT;
Direction_t up = UP;
Direction_t down = DOWN;

/* Normal mode bindings */
static KeyChord_t norm_bindings[] = {
    { .seq = { XK_i, XK_NULL },                 .func = insert_mode,       .args = NULL },
    { .seq = { XK_h, XK_NULL },                 .func = move_cursor,       .args = { &left } },
    { .seq = { XK_j, XK_NULL },                 .func = move_cursor,       .args = { &down } },
    { .seq = { XK_k, XK_NULL },                 .func = move_cursor,       .args = { &up } },
    { .seq = { XK_l, XK_NULL },                 .func = move_cursor,       .args = { &right } },
    { .seq = { XK_Z, XK_Z, XK_NULL },           .func = quit,              .args = NULL },
    { .seq = { XK_g, XK_g, XK_NULL },           .func = jump_to_top,       .args = NULL },
    { .seq = { XK_g, XK_G, XK_NULL },           .func = jump_to_bottom,    .args = NULL },
    { .seq = { XK_Control_L, XK_f, XK_NULL },   .func = page_down,         .args = NULL },
    { .seq = { XK_Control_L, XK_b, XK_NULL },   .func = page_up,           .args = NULL },
};

/* Insert mode bindings */
static KeyChord_t ins_bindings[] = {
    { .seq = { XK_Escape, XK_NULL },  .func = normal_mode,       .args = NULL },
};

/* Visual mode bindings */
static KeyChord_t vis_bindings[] = {
};

#endif /* KEYBOARD_H */
