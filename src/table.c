#include "table.h"
#include "helpers.h"

/**
 * @file table.c
 * @author Neil Kingdom
 * @since 05-09-2024
 * @version 1.0
 */

/*==============================
        Private Functions
 =============================*/

/**
 * @brief Returns a string of padding to be placed before the cell's contents.
 * Uses the cell_cwidth to determine how much space should be prepended to
 * the string contained in cell such that the text is aligned to the right of
 * the cell.
 * @param cell A cell containing the text that needs to be padded
 * @returns A string of whitespace, which is to be prepended before cell
 */
static char *get_right_pad(const cell_t cell) {
    size_t pad_size;
    char *padding = NULL;

    pad_size = cell_cwidth - strlen(cell);
    padding = malloc(pad_size + 1);
    if (padding == NULL) {
        perror("Failed to allocate memory for right pad");
        return NULL;
    }
    memset((void*)padding, ' ', pad_size);
    padding[pad_size] = '\0';

    return padding;
}

/**
 * @brief Returns a string of padding to be placed before the cell's contents.
 * Uses the cell_cwidth to determine how much space should be prepended to
 * the string contained in cell such that the text is centered.
 * @since 03-11-2024
 * @param cell A cell containing the text that needs to be padded
 * @returns A string of whitespace, which is to be prepended before cell
 */
static char *get_center_pad(const cell_t cell) {
    size_t pad_size;
    char *padding = NULL;

    pad_size = (cell_cwidth - strlen(cell)) / 2;
    padding = malloc(pad_size + 1);
    if (padding == NULL) {
        perror("Failed to allocate memory for right pad");
        return NULL;
    }
    memset((void*)padding, ' ', pad_size);
    padding[pad_size] = '\0';

    return padding;
}

/*==============================
        Public Functions
 =============================*/

/**
 * @brief Creates a new table context object and returns it.
 * @since 11-03-2024
 * @returns A reference to the table context object
 */
TableCtx_t *create_table_ctx(void) {
    TableCtx_t *table = NULL;

    /* Allocate memory for table struct */
    table = calloc(1, sizeof(TableCtx_t));
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

/* TODO: This works for now, but is very slow. Use CSV to determine which cells should be freed */
/**
 * @brief Destroys a table context object
 * @since 11-03-2024
 * @param table The table context object to be destroyed
 */
void destroy_table_ctx(TableCtx_t *table) {
    unsigned x, y;

    if (table == NULL) {
        return;
    }

    table->offset_x = 0;
    table->offset_y = 0;

    for (y = 0; y < MAX_ROWS; ++y) {
        for (x = 0; x < MAX_COLS; ++x) {
            cell_t *cell = get_cell_value(table, (Point_t){ x, y });
            if (*cell) {
                free(*cell);
            }
        }
    }

    free(table);
    table = NULL;
}

/**
 * @brief Returns a reference to the cell located at location.
 * @since 11-03-2024
 * @param table The table context object
 * @param location The location of the cell that will be returned
 * @returns A reference to the cell located at location
 */
cell_t *get_cell_value(TableCtx_t *table, const Point_t location) {
    unsigned x, y;
    x = location.x + table->offset_x;
    y = (location.y + table->offset_y) * (MAX_COLS - 1);
    return table->cells + x + y;
}

/**
 * @brief Update the value of the cell located at location.
 * @since 11-03-2024
 * @param table The table context object
 * @param text Text to update the cell with
 * @param location The location of the cell that shall be updated
 */
void set_cell_value(
    TableCtx_t *table,
    const char* const text,
    const Point_t location
) {
    if (text == NULL) {
        return;
    }

    cell_t *cell = get_cell_value(table, location);
    if (*cell == NULL) {
        *cell = malloc((cell_cwidth + 1) * sizeof(char));
        if (*cell == NULL) {
            perror("Failed to allocate memory for cell");
            exit(EXIT_FAILURE);
        }
    }
    strncpy(*cell, text, cell_cwidth);
}

/**
 * @brief Helper function which applies colors and other attributes to a cell in the table.
 * @since 08-09-2024
 * @param location The location of the cell that will be updated
 * @param col_pair The color pair that shall be applied to the cell at location
 * @param attrs The attributes that will be applied to the cell at location
 */
void set_cell_attrs(const Point_t location, const NugetCol_t col_pair, const uint64_t attrs) {
    mvchgat(location.y, location.x * cell_cwidth, cell_cwidth, attrs, col_pair, NULL);
}

/**
 * @brief Draw the column labels AA-ZZ.
 * @since 11-03-2024
 * @param table The table context object
 */
void draw_col_labels(TableCtx_t *table) {
    unsigned i, x;
    uint64_t lb, rb;
    char col_id[3];
    const size_t base = 26;

    lb = rb = table->offset_x;

    for (x = 1, i = 0; x < table->vis_cols; ++x, ++i) {
        col_id[0] = ((lb + i) / base) + 'A';
        col_id[1] = ((rb + i) % base) + 'A';
        col_id[2] = '\0';
        mvprintw(0, x * cell_cwidth, "%s", col_id);

        if (x == table->cursor.x) {
            set_cell_attrs((Point_t){ x, 0 }, PRIMARY_INV, A_BOLD);
        } else {
            set_cell_attrs((Point_t){ x, 0 }, PRIMARY, A_BOLD);
        }
    }
}

/**
 * @brief Draw the row labels 0-MAX_ROWS.
 * @since 11-03-2024
 * @param table The table context object
 */
void draw_row_labels(TableCtx_t *table) {
    unsigned y, row_num;
    char *row_id = NULL;

    for (y = 1, row_num = table->offset_y; y < table->vis_rows; ++y, ++row_num) {
        /* TODO: Could fail if column width is too small */
        row_id = n_itoa(row_num);
        mvprintw(y, 0, "%s", row_id);
        free(row_id);

        if (y == table->cursor.y) {
            set_cell_attrs((Point_t){ 0, y }, PRIMARY_INV, A_BOLD);
        } else {
            set_cell_attrs((Point_t){ 0, y }, PRIMARY, A_BOLD);
        }
    }
}

/**
 * @brief Draw a single cell.
 * Updates the cell's location, padding, and styling before rendering.
 * @since 11-03-2024
 * @param table The table context object
 * @param location The new location at which the cell will be drawn
 * @param align The preferred alignment of the cell's text
 * @param selected Should be true if the cell is currently selected or false otherwise
 */
void draw_cell(
    TableCtx_t *table,
    const Point_t location,
    const Align_t align,
    const bool selected
) {
    char *padding = NULL;
    cell_t cell = *get_cell_value(table, location);

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
        set_cell_attrs(location, CURSOR, A_BOLD);
    } else {
        set_cell_attrs(location, DEFAULT, A_NORMAL);
    }
}

/**
 * @brief Refreshes the table by updating each visible cell.
 * @since 11-03-2024
 * @param table The table context object
 */
void refresh_table(TableCtx_t *table) {
    bool selected;
    unsigned x, y;

    for (y = 0; y < table->vis_rows; ++y) {
        for (x = 0; x < table->vis_cols; ++x) {
            selected = (y == table->cursor.y && x == table->cursor.x);
            draw_cell(table, (Point_t){ x, y }, ALIGN_CENTER, selected);
        }
    }

    draw_row_labels(table);
    draw_col_labels(table);

    refresh();
}

/**
 * @brief Scrolls the table by updating x and y offsets.
 * @since 11-03-2024
 * @param table The table context object
 * @param direction The cardinal direction in which to scroll the table
 */
void scroll_table(TableCtx_t *table, const Direction_t direction) {
    const uint64_t right_bound = MAX_COLS - table->vis_cols + 1;
    const uint64_t bottom_bound = MAX_ROWS - table->vis_rows + 1;

    switch (direction) {
        case LEFT:
            if (table->offset_x > 0) {
                --table->offset_x;
            }
            break;
        case RIGHT:
            if (table->offset_x < right_bound) {
                ++table->offset_x;
            }
            break;
        case UP:
            if (table->offset_y > 0) {
                --table->offset_y;
            }
            break;
        case DOWN:
            if (table->offset_y < bottom_bound) {
                ++table->offset_y;
            }
            break;
    }

    clear();
    refresh_table(table);
}
