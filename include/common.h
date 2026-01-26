#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <stdint.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <ctype.h>
#include <ncurses.h>
#include <panel.h>
#include <signal.h>
#include <sys/ioctl.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB888_t;

typedef struct {
    uint64_t x;
    uint64_t y;
} Point_t;

typedef enum {
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT
} Align_t;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction_t;

typedef enum {
    NORM = A_NORMAL,
    BOLD = A_BOLD,
    ITAL = A_ITALIC,
    BLINK = A_BLINK
} CellAttrs_t;

// Color Scheme
#define FG_COLOR            COLOR_WHITE
#define BG_COLOR            COLOR_BLACK
#define PRIMARY_COLOR       COLOR_YELLOW
#define SECONDARY_COLOR     COLOR_RED
#define CURSOR_COLOR        PRIMARY_COLOR

// Color pairs              Foreground       Background
#define DEFAULT_PAIR        FG_COLOR,        BG_COLOR
#define PRIMARY_PAIR        FG_COLOR,        PRIMARY_COLOR
#define PRIMARY_INV_PAIR    PRIMARY_COLOR,   BG_COLOR
#define SECONDARY_PAIR      FG_COLOR,        SECONDARY_COLOR
#define SECONDARY_INV_PAIR  SECONDARY_COLOR, BG_COLOR
#define CURSOR_PAIR         FG_COLOR,        CURSOR_COLOR

typedef enum {
    DEFAULT = 1,    // Default/background color pair
    PRIMARY,        // Primary color pair
    PRIMARY_INV,    // Primary color pair (inverted)
    SECONDARY,      // Secondary color pair
    SECONDARY_INV,  // Secondary color pair (inverted)
    CURSOR,         // Cursor color pair
} ColorPair_t;

#endif
