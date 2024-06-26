#include "table.h"

/*
 * TODO:
 * - Shift cells right and down 1 to accomodate row/col ids
 * - Proper error handling
 * - [out/in] tags for doxygen
 */

/*==============================
        Private Functions
 =============================*/

__attribute__((always_inline))
static void clamp_u64(uint64_t *x, uint64_t lo, uint64_t hi) {
    *x = ((*x < lo) ? lo : *x) > hi ? hi : *x;
}

__attribute__((always_inline))
static void clamp_s64(int64_t *x, int64_t lo, int64_t hi) {
    *x = ((*x < lo) ? lo : *x) > hi ? hi : *x;
}

static char *get_right_pad(cell_t restrict cell) {
    return NULL;
}

static char *get_center_pad(cell_t restrict cell) {
    size_t pad_size;
    char *padding = NULL;

    pad_size = (cell_cwidth - strlen(cell)) / 2;
    padding = malloc(pad_size + 1);
    memset((void*)padding, ' ', pad_size);
    padding[pad_size] = '\0';

    return padding;
}

/*==============================
        Public Functions
 =============================*/

/**
 * @brief Returns a reference to the cell located at location
 * @since 11-03-2024
 * @param table The table context object
 * @param location The location of the cell that will be returned
 * @returns A reference to the cell located at location
 */
cell_t *get_cell(pTableCtx_t restrict table, const Point_t location) {
    unsigned x, y;
    x = location.x + table->offset_x;
    y = (location.y + table->offset_y) * MAX_COLS;
    return table->cells + x + y;
}

/**
 * @brief Creates a new table context object and returns it
 * @since 11-03-2024
 * @returns A reference to the table context object
 */
pTableCtx_t create_table_ctx(void) {
    unsigned x, y, col_num = 0;
    pTableCtx_t table = NULL;

    /* Allocate memory for table struct */
    table = calloc(1, sizeof(tableCtx_t));
    if (table == NULL) {
        perror("Failed to allocate memory for table context");
        return NULL;
    }

    /* Initialize members */
    table->vis_cols = 12; /* TODO: User-defined cols */
    table->vis_rows = getmaxy(stdscr);
    cell_cwidth = getmaxx(stdscr) / table->vis_cols;

    /* Allocate memory for cell references */
    table->cells = calloc(MAX_ROWS * MAX_COLS, sizeof(cell_t));
    if (table->cells == NULL) {
        perror("Failed to allocate memory for table cells");
        return NULL;
    }

    return table;
}

/**
 * @brief Draw the column IDs e.g. A..Z, AA..ZZ
 * @since 11-03-2024
 * @param table The table context object
 */
void draw_col_ids(pTableCtx_t restrict table) {
    char col_id[3];
    char lnibble, rnibble;
    unsigned i, x, row_num;
    const size_t base = 26;

    /* TODO: Broken */
    lnibble = rnibble = table->offset_x;

    for (x = 1, i = 0, row_num = table->offset_x; x < table->vis_cols; ++x, ++i) {
        col_id[2] = '\0';
        col_id[1] = ((rnibble + i) % base) + 'A';
        col_id[0] = ((lnibble + i) / base) + 'A';
        mvprintw(0, x * cell_cwidth, "%s", col_id);

        if (x == table->cursor.x) {
            color_cell((Point_t){ x, 0 }, PRIMARY_INV, A_BOLD);
        } else {
            color_cell((Point_t){ x, 0 }, PRIMARY, A_BOLD);
        }
    }
}

/**
 * @brief Draw the row IDs e.g. 0..(MAX_ROWS - 1)
 * @since 11-03-2024
 * @param table The table context object
 */
void draw_row_ids(pTableCtx_t restrict table) {
    unsigned y, row_num;
    char *row_id = NULL;

    for (y = 1, row_num = table->offset_y; y < table->vis_rows; ++y, ++row_num) {
        /* TODO: Could fail if column width is too small */
        row_id = dtoa(row_num);
        mvprintw(y, 0, "%s", row_id);
        free(row_id);

        if (y == table->cursor.y) {
            color_cell((Point_t){ 0, y }, PRIMARY_INV, A_BOLD);
        } else {
            color_cell((Point_t){ 0, y }, PRIMARY, A_BOLD);
        }
    }
}

/**
 * @brief Destroys a table context object
 * @since 11-03-2024
 * @param table The table context object to be destroyed
 */
void destroy_table_ctx(pTableCtx_t restrict table) {
    unsigned x, y;

    if (table == NULL) {
        return;
    }

    table->offset_x = 0;
    table->offset_y = 0;

    if (table->cells) {
        for (y = 0; y < MAX_ROWS; ++y) {
            for (x = 0; x < MAX_COLS; ++x) {
                cell_t *cell = get_cell(table, (Point_t){ x, y });
                if (*cell) {
                    free(*cell);
                }
            }
        }
        free(table->cells);
    }

    free(table);
    table = NULL;
}

/**
 * @brief Draw a single cell i.e. update a cell's location, padding, and styling
 * @since 11-03-2024
 * @param table The table context object
 * @param location The new location at which the cell will be drawn
 * @param align The preferred alignment of the cell's text
 * @param selected Should be true if the cell is currently selected or false otherwise
 */
void draw_cell(
    pTableCtx_t restrict table,
    const Point_t location,
    const Align_t align,
    const bool selected
) {
    char *padding = NULL;
    cell_t cell = *get_cell(table, location);

    move(location.y + 1, (location.x + 1) * cell_cwidth);

    if (cell != NULL) {
        switch (align) {
            case ALIGN_CENTER:
                padding = get_center_pad(cell);
                break;
            case ALIGN_RIGHT:
                /* TODO: Implement */
                fprintf(stderr, "Haven't implemented ALIGN_RIGHT");
                exit(EXIT_FAILURE);
                break;
            default:
                break;
        }

        if (padding) {
            printw("%s", padding);
            free(padding);
        }

        printw("%s", cell);
    }

    if (selected) {
        color_cell(location, CURSOR, A_BOLD);
    } else {
        color_cell(location, DEFAULT, A_NORMAL);
    }
}

/**
 * @brief Update the value of the cell located at location
 * @since 11-03-2024
 * @param table The table context object
 * @param value The new value (text) to update the cell with
 * @param location The location of the cell that shall be updated
 */
void update_cell_value(
    pTableCtx_t restrict table,
    const char * restrict value,
    const Point_t location
) {
    cell_t *cell = get_cell(table, location);
    if (*cell == NULL) {
        *cell = malloc((cell_cwidth + 1) * sizeof(char));
        if (*cell == NULL) {
            perror("Failed to allocate memory for cell");
            exit(EXIT_FAILURE);
        }
    }
    strncpy(*cell, value, cell_cwidth);
}

/**
 * @brief Re-render the entire table by updating each visible cell
 * @since 11-03-2024
 * @param table The table context object
 */
void redraw_table(pTableCtx_t restrict table) {
    bool selected;
    unsigned x, y;

    for (y = 0; y < table->vis_rows; ++y) {
        for (x = 0; x < table->vis_cols; ++x) {
            selected = (y == table->cursor.y && x == table->cursor.x);
            draw_cell(table, (Point_t){ x, y }, ALIGN_CENTER, selected);
        }
    }

    draw_row_ids(table);
    draw_col_ids(table);

    refresh();
}

/**
 * @brief Scrolls the table by updating x and y offsets
 * @since 11-03-2024
 * @param table The table context object
 * @param direction The cardinal direction in which to scroll the table
 */
void scroll_table(pTableCtx_t restrict table, const Direction_t direction) {
    switch (direction) {
        case LEFT:
            if (table->offset_x > 0) {
                --table->offset_x;
            }
            break;
        case RIGHT:
            if (table->offset_x < MAX_COLS - 1) {
                ++table->offset_x;
            }
            break;
        case UP:
            if (table->offset_y > 0) {
                --table->offset_y;
            }
            break;
        case DOWN:
            if (table->offset_y < MAX_ROWS - 1) {
                ++table->offset_y;
            }
            break;
    }

    clear();
    redraw_table(table);
}
