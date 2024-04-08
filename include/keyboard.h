#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "table.h"
#include <X11/keysym.h>

typedef uint16_t keysym_t;
typedef void *(*kc_callback)(pTableCtx_t table, void *data);

typedef enum {
    NORMAL,
    INSERT,
    VISUAL
} Mode_t;

extern Mode_t curr_mode;

typedef struct {
    keysym_t    seq[10];    /* The sequence of keys that make up the chord (NULL-terminated) */
    kc_callback action;     /* A callback to be performed when the sequence is completed */
} KeyChord_t;

/*** Callbacks ***/

static void *insert_mode() {
    curr_mode = INSERT;
    return NULL;
}

static void *normal_mode() {
    curr_mode = NORMAL;
    return NULL;
}

static void *jump_to_top(pTableCtx_t restrict table, void *data) {
    table->offset_y = 0;
    redraw_table(table);

    return NULL;
}

static void *jump_to_bottom(pTableCtx_t restrict table, void *data) {
    table->offset_y = MAX_ROWS - table->vis_cols;
    redraw_table(table);

    return NULL;
}

/*** Bindings ***/

/* Normal mode bindings */
static KeyChord_t norm_bindings[] = {
    { .seq = { XK_i, NULL },       .action = insert_mode },
    { .seq = { XK_g, XK_g, NULL }, .action = jump_to_top },
    { .seq = { XK_g, XK_G, NULL }, .action = jump_to_bottom },
};

/* Insert mode bindings */
static KeyChord_t ins_bindings[] = {
    { .seq = { XK_Escape, NULL }, .action = normal_mode },
};

/* Visual mode bindings */
static KeyChord_t vis_bindings[] = {
};

#endif
