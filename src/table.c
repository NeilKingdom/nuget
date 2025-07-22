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
 * Uses cell_width to determine how much space should be prepended to the text
 * contained in cell such that the text is aligned to the right of the cell.
 * @param align The desired alignment of the text belonging to cell
 * @param cell A cell containing the text that needs to be padded
 * @returns A string of whitespace, which is to be prepended before cell
 */
static char *get_cell_padding(TableCtx_t *table, const Align_t align, const cell_t cell) {
    size_t pad_size;
    char *padding = NULL;

    if (align == ALIGN_LEFT || strlen(cell) >= table->cell_width) {
        padding = strdup("");
        if (padding == NULL) {
            perror("Failed to allocate memory for padding");
            return NULL;
        }
        return padding;
    } else if (align == ALIGN_CENTER) {
        pad_size = (table->cell_width - strlen(cell)) / 2;
    } else if (align == ALIGN_RIGHT) {
        pad_size = table->cell_width - strlen(cell);
    }

    padding = malloc((pad_size + 1) * sizeof(char));
    if (padding == NULL) {
        perror("Failed to allocate memory for padding");
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
    struct winsize ws;
    TableCtx_t *table = NULL;

    /* Allocate memory for table struct */
    table = calloc(1, sizeof(TableCtx_t));
    if (table == NULL) {
        perror("Failed to allocate memory for table context");
        return NULL;
    }

    /* Allocate memory for cell references */
    table->data = calloc(MAX_ROWS * MAX_COLS, sizeof(cell_t));
    if (table->data == NULL) {
        perror("Failed to allocate memory for table cells");
        free(table);
        return NULL;
    }

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);

    /* Initialize members */
    table->nvis_cols = 12; /* TODO: User-defined cols */
    table->nvis_rows = ws.ws_row;
    table->cell_width = ws.ws_col / table->nvis_cols;
    table->table_offset = (Point_t){ 1, 1 };
    table->cursor = table->table_offset;

    return table;
}

/**
 * @brief Destroys a table context object.
 * @since 11-03-2024
 * @param table The table context object to be destroyed
 */
void destroy_table_ctx(TableCtx_t *table) {
    const uint64_t table_size = MAX_ROWS * MAX_COLS;
    uint64_t i = 0;
    cell_t *cell = NULL;

    if (table == NULL) {
        return;
    }

    /* Too slow to call get_cell_value(), so need a bit of optimization */
    cell = table->data;
    while (i++ < table_size) {
        if (*(cell + i)) {
            free(*(cell + i));
        }
    }

    if (table->data) {
        free(table->data);
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
    const uint64_t x = table->abs_offset.x + location.x;
    const uint64_t y = table->abs_offset.y + location.y;
    return table->data + (y * (int)MAX_COLS) + x;
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
        *cell = malloc((table->cell_width + 1) * sizeof(char));
        if (*cell == NULL) {
            /* Attempt to simply make blank cell as last resort */
            *cell = strdup("");
            if (*cell == NULL) {
                perror("Failed to allocate memory for cell");
                quit_nuget(table, NULL, EXIT_FAILURE);
            }
        }
    }
    strncpy(*cell, text, table->cell_width + 1);
}

/**
 * @brief Helper function which applies colors and other attributes to a cell in the table.
 * @since 08-09-2024
 * @param location The location of the cell that will be updated
 * @param col_pair The color pair that shall be applied to the cell at location
 * @param attrs The attributes that will be applied to the cell at location
 */
void set_cell_attrs(
    TableCtx_t *table,
    const Point_t location,
    const NugetCol_t col_pair,
    const uint64_t attrs
) {
    mvchgat(
        location.y,
        location.x * table->cell_width,
        table->cell_width,
        attrs,
        col_pair,
        NULL
    );
}

/**
 * @brief Draw the column labels AA-ZZ.
 * @since 11-03-2024
 * @param table The table context object
 */
void draw_col_labels(TableCtx_t *table) {
    unsigned i, x;
    uint64_t msb, lsb;
    char col_id[3];
    const size_t base = 26;

    msb = lsb = table->abs_offset.x;

    for (x = 1, i = 0; x < table->nvis_cols; ++x, ++i) {
        col_id[0] = ((msb + i) / base) + 'A';
        col_id[1] = ((lsb + i) % base) + 'A';
        col_id[2] = '\0';
        mvprintw(0, x * table->cell_width, "%s", col_id);

        if (x == table->cursor.x) {
            set_cell_attrs(table, (Point_t){ x, 0 }, PRIMARY_INV, A_BOLD);
        } else {
            set_cell_attrs(table, (Point_t){ x, 0 }, PRIMARY, A_BOLD);
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

    for (y = 1, row_num = table->abs_offset.y + 1; y < table->nvis_rows; ++y, ++row_num) {
        /* TODO: Could fail if column width is too small */
        row_id = n_itoa(row_num);
        mvprintw(y, 0, "%s", row_id);
        free(row_id);

        if (y == table->cursor.y) {
            set_cell_attrs(table, (Point_t){ 0, y }, PRIMARY_INV, A_BOLD);
        } else {
            set_cell_attrs(table, (Point_t){ 0, y }, PRIMARY, A_BOLD);
        }
    }
}

/**
 * @brief Draws a single cell at the position indicated by location.
 * Adds padding and attributes depending upon the values given for align and selected.
 * @since 11-03-2024
 * @param table The table context object
 * @param location The location of the cell being drawn
 * @param align The preferred alignment of the cell's text
 * @param selected True if the cell being drawn shares the position of the cursor, or false otherwise
 */
void draw_cell(
    TableCtx_t *table,
    const Point_t location,
    const Align_t align,
    const bool selected
) {
    const char *ellipses = "...";
    const uint64_t y_pos = location.y;
    const uint64_t x_pos = location.x * table->cell_width;

    cell_t cell;
    char *padding = NULL;
    char *final_str = NULL;

    cell = *get_cell_value(table, location);
    if (cell != NULL && table->cell_width >= strlen(ellipses)) {
        padding = get_cell_padding(table, align, cell);

        final_str = malloc((strlen(padding) + strlen(cell) + 1) * sizeof(char));
        if (final_str == NULL) {
            perror("Failed to allocate memory for temporary cell");
            goto set_attrs;
        }

        final_str[0] = '\0';
        strncat(final_str, padding, strlen(padding));
        strncat(final_str, cell, strlen(cell));

        if (strlen(final_str) > table->cell_width) {
            strncpy(final_str + table->cell_width - strlen(ellipses), ellipses, strlen(ellipses) + 1);
        }

        mvprintw(location.y, location.x * table->cell_width, "%s", final_str);
        free(padding);
        free(final_str);
    }

set_attrs:

    /* Set appropriate attributes for the cell */
    if (selected) {
        set_cell_attrs(table, location, CURSOR, A_BOLD);
    } else {
        set_cell_attrs(table, location, DEFAULT, A_NORMAL);
    }
}

/**
 * @brief Refreshes the table by updating each visible cell.
 * @since 11-03-2024
 * @param table The table context object
 */
void refresh_table(TableCtx_t *table) {
    bool selected;
    uint64_t x, y;

    clear();
    draw_row_labels(table);
    draw_col_labels(table);

    for (y = table->table_offset.y; y < table->nvis_rows; ++y) {
        for (x = table->table_offset.x; x < table->nvis_cols; ++x) {
            selected = (x == table->cursor.x) && (y == table->cursor.y);
            draw_cell(table, (Point_t){ x, y }, ALIGN_LEFT, selected);
        }
    }

    refresh();
}

/**
 * @brief Scrolls the table by updating x and y offsets.
 * @since 11-03-2024
 * @param table The table context object
 * @param direction The cardinal direction in which to scroll the table
 */
void scroll_table(TableCtx_t *table, const Direction_t direction) {
    const uint64_t right_bound = MAX_COLS - table->nvis_cols;
    const uint64_t bottom_bound = MAX_ROWS - table->nvis_rows;

    switch (direction) {
        case LEFT:
            if (table->abs_offset.x > 0) {
                --table->abs_offset.x;
            }
            break;
        case RIGHT:
            if (table->abs_offset.x < right_bound) {
                ++table->abs_offset.x;
            }
            break;
        case UP:
            if (table->abs_offset.y > 0) {
                --table->abs_offset.y;
            }
            break;
        case DOWN:
            if (table->abs_offset.y < bottom_bound) {
                ++table->abs_offset.y;
            }
            break;
    }

    refresh_table(table);
}
