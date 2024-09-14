#include "helpers.h"

/**
 * @brief Converts the positive integer n to a textual string representation.
 * @since 08-09-2024
 * @param n The digit that shall be converted to a string
 * @returns A string representation of the digit n
 */
char *n_itoa(const uint64_t n) {
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

/**
 * @brief Converts a valid string into a positive integer.
 * @since 08-09-2024
 * @param a A textual string representation of the digit that will be parsed
 * @returns A positive integer which is parsed from a
 */
uint64_t n_atoi(const char* const a) {
    return 0;
}

/**
 * @brief Converts the floating point number d to a textual string representation.
 * @since 08-09-2024
 * @param d The floating point number that shall be converted to a string
 * @returns A string representation of the floating point number d
 */
char *n_dtoa(const double d) {
    return NULL;
}

/**
 * @brief Converts a valid string into a floating point number.
 * @since 08-09-2024
 * @param a A textual string representation of the floating point number that will be parsed
 * @returns A floating point number which is parsed from a
 */
double n_atod(const char* const a) {
    return 0.0;
}

/**
 * @brief Converts a textual string representation of an RGB888 color in hexadecimal format to a Color_t.
 * @since 08-09-2024
 * @param hex The textual string representation of the RGB888 color
 * @returns A Color_t struct containing the values parsed from hex
 */
Color_t hex_to_rgb(const char* const hex) {
    const size_t nibbles = 2;
    int bad_range;
    unsigned i;
    Color_t color = { 0 };
    char *r = alloca(nibbles + 1);
    char *g = alloca(nibbles + 1);
    char *b = alloca(nibbles + 1);
    char *hex_copy = NULL;

    hex_copy = malloc((strlen(hex) + 1) * sizeof(char));
    if (hex_copy == NULL) {
        perror("Failed to allocate space for temporary buffer");
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

/* TODO: Is this the best spot for this? */
void quit_nuget(TableCtx_t *table, ncsv_t *csv, int status) {
    /* ncurses cleanup */
    if (table != NULL) {
        destroy_table_ctx(table);
    }
    endwin();

    /* libcsv cleanup */
    if (csv != NULL) {
        csv_free(csv);
    }

    exit(status);
}
