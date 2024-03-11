#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "table.h"
#include <X11/keysym.h>

typedef int key_t;
typedef void (*keychrd_cb)(pTableCtx_t table, void *data);

typedef struct {
    key_t seq[2];       /* The sequence of keys that make up the chord */
    keychrd_cb action;  /* A callback to be performed when the sequence is completed */
} keychrd_t;

/* TODO: Think of better name */
enum KcKeys {
    KC_G
};

static void *jump_to_top(pTableCtx_t table, void *data) {
    return NULL;
}

/*
keychrd_t kc_gg = {
    .seq = { XK_g, XK_g },
    .action = jump_to_top
};

keychrd_t kc_gG = {
    .seq = { KX_g, XK_G },
    .action = jump_to_btm
};
*/

#endif
