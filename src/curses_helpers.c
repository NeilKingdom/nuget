#include "../include/curses_helpers.h"

color_t hex_to_rgb(const char *hex) {
    const size_t hb_width = 2; /* Hex byte width */
    size_t bad_range;
    unsigned i;
    color_t color = { 0 };
    char *r = alloca(hb_width + 1);
    char *g = alloca(hb_width + 1);
    char *b = alloca(hb_width + 1);
    char *hex_copy = strdup(hex);

    /* Validation */
    assert(strlen(hex_copy) == 7);
    assert(hex_copy[0] == '#');

    hex_copy++; /* Skip over octothorp */
    for (i = 0; i < strlen(hex_copy); ++i) {
        hex_copy[i] = (char)toupper(hex_copy[i]);
        assert(isdigit(hex_copy[i]) || (hex_copy[i] >= 'A' && hex_copy[i] <= 'F'));
    }

    /* Split into slices */
    strncpy(r, hex_copy, hb_width);
    r[hb_width] = '\0';
    hex_copy += hb_width;
    strncpy(g, hex_copy, hb_width);
    g[hb_width] = '\0';
    hex_copy += hb_width;
    strncpy(b, hex_copy, hb_width);
    b[hb_width] = '\0';

    /* Compute rgb888 values */
    bad_range = 'A' - '9' - 1; /* Unused ASCII chars between digits and uppercase */
    color.r |= (r[0] <= '9') ? (r[0] - '0') << 4 : (r[0] - '0' - bad_range) << 4;
    color.r |= (r[1] <= '9') ? (r[1] - '0')      : (r[1] - '0' - bad_range);
    color.g |= (g[0] <= '9') ? (g[0] - '0') << 4 : (g[0] - '0' - bad_range) << 4;
    color.g |= (g[1] <= '9') ? (g[1] - '0')      : (g[1] - '0' - bad_range);
    color.b |= (b[0] <= '9') ? (b[0] - '0') << 4 : (b[0] - '0' - bad_range) << 4;
    color.b |= (b[1] <= '9') ? (b[1] - '0')      : (b[1] - '0' - bad_range);

    /* Cleanup */
    hex_copy -= (hb_width + hb_width + 1);
    free(hex_copy);

    return color;
}

void color_cell(
    pTableCtx_t restrict table,
    const point_t location,
    const nuget_col_t col_pair,
    const uint64_t attrs
) {
    mvchgat(location.y, location.x * cell_cwidth, cell_cwidth, attrs, col_pair, NULL);
}
