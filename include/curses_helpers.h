#ifndef CURSES_HELPERS_H
#define CURSES_HELPERS_H

#include "common.h"
#include "table.h"

#define TEXT_COLOR      "#112233"
#define PRIMARY_COLOR   "#AA2244"
#define SECONDARY_COLOR "#AA5566"
#define ELIPSES         "..."

/* Forward function decls */

color_t hex_to_rgb(const char *hex);
void draw_cell(tableCtx_t table, point_t location, bool selected);

#endif /* CURSES_HELPERS_H */
