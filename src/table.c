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
//static char *get_cell_padding(TableCtx_t *table, const Align_t align, const Cell_t cell) {
//    size_t pad_size = 0;
//
//    if (align == ALIGN_LEFT || cell.len >= table->cell_width) {
//        return strdup("");
//    } else if (align == ALIGN_CENTER) {
//        pad_size = (table->cell_width - cell.len) / 2;
//    } else if (align == ALIGN_RIGHT) {
//        pad_size = table->cell_width - cell.len;
//    }
//
//    char *padding = malloc((pad_size + 1) * sizeof(char));
//    if (padding == NULL) {
//        perror("Failed to allocate memory for padding");
//        return NULL;
//    }
//    memset((void*)padding, ' ', pad_size);
//    padding[pad_size] = '\0';
//
//    return padding;
//}

/**
 * @brief Helper function which applies colors and other attributes to a cell in the table.
 * @since 08-09-2024
 * @param location The location of the cell that will be updated
 * @param col_pair The color pair that shall be applied to the cell at location
 * @param attrs The attributes that will be applied to the cell at location
 */
static void set_cell_attrs(
    TableCtx_t *table,
    const Point_t location,
    const ColorPair_t col_pair,
    const CellAttrs_t attrs
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

/*==============================
        Public Functions
 =============================*/

/**
 * @brief Creates a new table context object and returns it.
 * @since 11-03-2024
 * @returns A reference to the table context object
 */
TableCtx_t *create_table_ctx(void) {
    // Allocate memory for table struct
    TableCtx_t *table = calloc(1, sizeof(TableCtx_t));
    if (table == NULL) {
        perror("Failed to allocate memory for table context");
        return NULL;
    }

    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);

    // Initialize members
    table->nvis_cols = 12; // TODO: User-defined cols
    table->nvis_rows = ws.ws_row;
    table->cell_width = ws.ws_col / table->nvis_cols;
    table->table_offset = (Point_t){ 1, 1 };
    table->cursor = table->table_offset;

    // Allocate memory for cell references
    table->arena = calloc(MAX_ROWS * MAX_COLS, sizeof(Cell_t));
    if (table->arena == NULL) {
        perror("Failed to allocate memory for table cells");
        free(table);
        return NULL;
    }

    return table;
}

/**
 * @brief Destroys a table context object.
 * @since 11-03-2024
 * @param table The table context object to be destroyed
 */
void destroy_table_ctx(TableCtx_t *table) {
    if (table == NULL) {
        return;
    }

    Cell_t *cell = table->arena;
    const uint64_t table_size = MAX_ROWS * MAX_COLS;
    for (uint64_t i = 0; i < table_size; ++i) {
        if (cell[i].str) {
            free(cell[i].str);
        }
    }

    if (table->arena) {
        free(table->arena);
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
Cell_t *get_cell(TableCtx_t *table, const Point_t location) {
    const uint64_t x = table->abs_offset.x + location.x;
    const uint64_t y = table->abs_offset.y + location.y;
    return table->arena + (y * (int)MAX_COLS) + x;
}

/**
 * @brief Update the value of the cell located at location.
 * @since 11-03-2024
 * @param table The table context object
 * @param text Text to update the cell with
 * @param location The location of the cell that shall be updated
 */
void set_cell_text(
    TableCtx_t *table,
    const Point_t location,
    const char* const text,
    const size_t text_len
) {
    Cell_t *cell = get_cell(table, location);

    if (!text || text_len == 0) {
        free(cell->str);
        cell->str = NULL;
        cell->len = 0;
        return;
    }

    char *new_str = realloc(cell->str, text_len);
    if (!new_str) {
        // realloc failed; old cell->str is still valid
        return;
    }

    cell->str = new_str;
    memcpy(cell->str, text, text_len);
    cell->len = text_len;
}

/**
 * @brief Draw the column labels AA-ZZ.
 * @since 11-03-2024
 * @param table The table context object
 */
void draw_col_labels(TableCtx_t *table) {
    const size_t base = 26;
    uint64_t msb = table->abs_offset.x;
    uint64_t lsb = table->abs_offset.x;

    char col_id[3] = { 0 };
    for (unsigned x = 1, i = 0; x < table->nvis_cols; ++x, ++i) {
        col_id[0] = ((msb + i) / base) + 'A';
        col_id[1] = ((lsb + i) % base) + 'A';
        mvprintw(0, x * table->cell_width, "%s", col_id);

        if (x == table->cursor.x) {
            set_cell_attrs(table, (Point_t){ x, 0 }, PRIMARY_INV, BOLD);
        } else {
            set_cell_attrs(table, (Point_t){ x, 0 }, PRIMARY, BOLD);
        }
    }
}

/**
 * @brief Draw the row labels 0-MAX_ROWS.
 * @since 11-03-2024
 * @param table The table context object
 */
void draw_row_labels(TableCtx_t *table) {
    for (unsigned y = 1, row_num = table->abs_offset.y + 1; y < table->nvis_rows; ++y, ++row_num) {
        // TODO: Could fail if column width is too small
        char *row_id = n_itoa(row_num);
        mvprintw(y, 0, "%s", row_id);
        free(row_id);

        if (y == table->cursor.y) {
            set_cell_attrs(table, (Point_t){ 0, y }, PRIMARY_INV, BOLD);
        } else {
            set_cell_attrs(table, (Point_t){ 0, y }, PRIMARY, BOLD);
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
void render_cell(
    TableCtx_t *table,
    const Point_t location,
    const Align_t align,
    const bool selected
) {
    const char *ellipses = "...";
    char *padding = NULL;

    const Cell_t cell = *get_cell(table, location);

    size_t final_len = MIN(cell.len, table->cell_width);
    if (final_len == 0) {
        goto set_attrs;
    }

    char *final_str = malloc(final_len);
    if (final_str == NULL) {
        goto set_attrs;
    }

    memcpy(final_str, cell.str, final_len);
    if (cell.len > table->cell_width && final_len >= strlen(ellipses)) {
        memcpy(final_str + final_len - strlen(ellipses), ellipses, strlen(ellipses));
    }

    mvaddnstr(location.y, location.x * table->cell_width, final_str, final_len);
    free(final_str);

set_attrs:
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
    clear();
    draw_row_labels(table);
    draw_col_labels(table);

    bool selected;
    for (uint64_t y = table->table_offset.y; y < table->nvis_rows; ++y) {
        for (uint64_t x = table->table_offset.x; x < table->nvis_cols; ++x) {
            selected = (x == table->cursor.x) && (y == table->cursor.y);
            render_cell(table, (Point_t){ x, y }, ALIGN_LEFT, selected);
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
