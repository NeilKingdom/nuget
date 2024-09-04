#ifndef HELPERS_H
#define HELPERS_H

#include "common.h"
#include "table.h"

#define ELIPSES "..."

typedef enum {
    DEFAULT = 1,    /* Default/background color pair */
    PRIMARY,        /* Primary color pair */
    PRIMARY_INV,    /* Primary color pair (inverted) */
    SECONDARY,      /* Secondary color pair */
    SECONDARY_INV,  /* Secondary color pair (inverted) */
    CURSOR,         /* Cursor color pair */
} NugetCol_t;

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

char       *itoa(const int d);
int         atoi(const char* const a);
char       *dtoa(const double d);
double      atod(const char* const a);
Color_t     hex_to_rgb(const char* const hex);
void        color_cell(const Point_t location, const NugetCol_t col_pair, const uint64_t attrs);

#endif /* HELPERS_H */
