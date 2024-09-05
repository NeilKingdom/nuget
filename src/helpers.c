#include "helpers.h"

char *itoa(const int n) {
    int i, tmp = n;
    unsigned digits = 0;
    char *a = NULL;

    do {
        digits++;
        tmp /= 10;
    } while (tmp);

    a = malloc((digits + 1) * sizeof(char));
    if (a == NULL) {
        perror("Failed to allocate memory for buffer");
        return NULL;
    }

    for (i = digits, tmp = n; i > 0; --i) {
        a[i - 1] = (tmp % 10) + '0';
        tmp /= 10;
    }

    a[digits] = '\0';

    return a;
}

int atoi(const char* const a) {
    return 0;
}

char *dtoa(const double d) {
    return NULL;
}

double atod(const char* const a) {
    return 0.0;
}

Color_t hex_to_rgb(const char *hex) {
    const size_t nibbles = 2;
    int bad_range;
    unsigned i;
    Color_t color = { 0 };
    char *r = alloca(nibbles + 1);
    char *g = alloca(nibbles + 1);
    char *b = alloca(nibbles + 1);
    char *hex_copy = NULL;

    hex_copy = malloc(strlen(hex) + 1);
    if (hex_copy == NULL) {
        /* TODO: Error */
        return color;
    }
    strncpy(hex_copy, hex, strlen(hex) + 1);

    /* Validation */
    assert(strlen(hex_copy) == 7);
    assert(hex_copy[0] == '#');

    for (i = 1; i < strlen(hex_copy); ++i) {
        hex_copy[i] = (char)toupper(hex_copy[i]);
        assert(isdigit(hex_copy[i]) || (hex_copy[i] >= 'A' && hex_copy[i] <= 'F'));
    }

    /* Split into color channels */
    strncpy(r, hex_copy, nibbles + 1);
    hex_copy += nibbles;
    strncpy(g, hex_copy, nibbles + 1);
    hex_copy += nibbles;
    strncpy(b, hex_copy, nibbles + 1);

    /* Compute rgb888 values */
    bad_range = 'A' - '9' - 1; /* Unused ASCII chars between digits and uppercase */
    color.r |= (r[0] <= '9') ? (r[0] - '0') << 4 : (r[0] - '0' - bad_range) << 4;
    color.r |= (r[1] <= '9') ? (r[1] - '0')      : (r[1] - '0' - bad_range);
    color.g |= (g[0] <= '9') ? (g[0] - '0') << 4 : (g[0] - '0' - bad_range) << 4;
    color.g |= (g[1] <= '9') ? (g[1] - '0')      : (g[1] - '0' - bad_range);
    color.b |= (b[0] <= '9') ? (b[0] - '0') << 4 : (b[0] - '0' - bad_range) << 4;
    color.b |= (b[1] <= '9') ? (b[1] - '0')      : (b[1] - '0' - bad_range);

    /* Cleanup */
    hex_copy -= (nibbles + nibbles + 1);
    free(hex_copy);

    return color;
}

void color_cell(const Point_t location, const NugetCol_t col_pair, const uint64_t attrs) {
    mvchgat(location.y, location.x * cell_cwidth, cell_cwidth, attrs, col_pair, NULL);
}
