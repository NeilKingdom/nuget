#ifndef CURSES_HELPERS_H
#define CURSES_HELPERS_H

#include "common.h"
#include "table.h"

#define ELIPSES "..."

typedef enum NugetColor {
    DEFAULT = 1,    /* Default/background color pair */
    PRIMARY,        /* Primary color pair */
    PRIMARY_INV,    /* Primary color pair (inverted) */
    SECONDARY,      /* Secondary color pair */
    SECONDARY_INV,  /* Secondary color pair (inverted) */
    CURSOR,         /* Cursor color pair */
} nuget_col_t;

#define TEXT_COLOR          COLOR_WHITE
#define BG_COLOR            COLOR_BLACK
#define PRIMARY_COLOR       COLOR_YELLOW
#define SECONDARY_COLOR     COLOR_RED
#define CURSOR_COLOR        PRIMARY_COLOR

#define DEFAULT_PAIR        TEXT_COLOR,      BG_COLOR
#define PRIMARY_PAIR        TEXT_COLOR,      PRIMARY_COLOR
#define PRIMARY_INV_PAIR    PRIMARY_COLOR,   BG_COLOR
#define SECONDARY_PAIR      TEXT_COLOR,      SECONDARY_COLOR
#define SECONDARY_INV_PAIR  SECONDARY_COLOR, BG_COLOR
#define CURSOR_PAIR         TEXT_COLOR,      CURSOR_COLOR

/* Forward function decls */

color_t hex_to_rgb(const char *hex);
void color_cell(pTableCtx_t table, const point_t location, const nuget_col_t col_pair, const uint64_t attrs);

#endif /* CURSES_HELPERS_H */
