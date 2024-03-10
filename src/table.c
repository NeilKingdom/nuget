#include "../include/table.h"

static void clamp_u64(uint64_t *x, uint64_t lo, uint64_t hi) {
    *x = ((*x < lo) ? lo : *x) > hi ? hi : *x;
}

static void clamp_s64(int64_t *x, int64_t lo, int64_t hi) {
    *x = ((*x < lo) ? lo : *x) > hi ? hi : *x;
}

cell_t *get_cell(pTableCtx_t table, point_t location) {
    return table->cells
        + ((location.y + table->offset_y) * MAX_COLS)
        + (location.x  + table->offset_x);
}

pTableCtx_t create_table_ctx(void) {
    unsigned x, y;
    pTableCtx_t table = NULL;

    /* Allocate memory for table struct */
    table = calloc(1, sizeof(tableCtx_t));
    if (table == NULL) {
        perror("Failed to allocate memory for table context");
        return NULL;
    }

    /* Initialize members */
    table->cursor.x = 0;
    table->cursor.y = 0;
    table->vis_cols = 12; /* TODO: User-defined cols */
    table->vis_rows = getmaxy(stdscr);
    cell_cwidth = getmaxx(stdscr) / table->vis_cols;

    /* Allocate memory for cell references */
    table->cells = malloc(MAX_ROWS * MAX_COLS * sizeof(cell_t));
    if (table->cells == NULL) {
        perror("Failed to allocate memory for table cells");
        return NULL;
    }

    /* Allocate memory for cell contents */
    for (y = 0; y < MAX_ROWS; ++y) {
        for (x = 0; x < MAX_COLS; ++x) {
            cell_t *cell = get_cell(table, (point_t){ x, y });
            *cell = malloc(cell_cwidth + 1);
            if (*cell == NULL) {
                perror("Failed to allocate memory for cell");
                return NULL;
            }
            strcpy(*cell, "");
        }
    }

    return table;
}

void destroy_table_ctx(pTableCtx_t restrict table) {
    unsigned x, y;

    if (table == NULL) {
        return;
    }

    if (table->cells) {
        for (y = 0; y < MAX_ROWS; ++y) {
            for (x = 0; x < MAX_COLS; ++x) {
                cell_t *cell = get_cell(table, (point_t){ x, y });
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

void draw_cell(pTableCtx_t table, point_t location, bool selected) {
    cell_t cell = *get_cell(table, location);

    mvprintw(location.y, location.x, cell);
    if (selected) {
        chgat(cell_cwidth, A_BOLD, 1, NULL);
    } else {
        chgat(cell_cwidth, A_NORMAL, 2, NULL);
    }
}

void update_cell_value(pTableCtx_t table, const char * restrict value, point_t location) {
    cell_t *cell = get_cell(table, location);
    *cell = realloc(*cell, cell_cwidth + 1);
    strncpy(*cell, value, cell_cwidth);
    *cell[cell_cwidth] = '\0';
}

void redraw_table(pTableCtx_t restrict table) {
    unsigned x, y;

    for (y = 0; y < table->vis_rows; ++y) {
        for (x = 0; x < table->vis_cols * cell_cwidth; x += cell_cwidth) {
            bool selected = (y == table->cursor.y
                && (x >= table->cursor.x && x < table->cursor.x + cell_cwidth));
            draw_cell(table, (point_t){ x, y }, selected);
        }
    }

    refresh();
}

void scroll_table(pTableCtx_t restrict table, direction_t direction) {
    int8_t x = 0, y = 0;

    switch (direction) {
        case LEFT:
            --x;
            break;
        case RIGHT:
            ++x;
            break;
        case UP:
            --y;
            break;
        case DOWN:
            ++y;
            break;
    }

    table->offset_x += x;
    clamp_u64(&table->offset_x, 0, MAX_COLS - 1);
    table->offset_y += y;
    clamp_u64(&table->offset_y, 0, MAX_ROWS - 1);

    clear();
    redraw_table(table);
}

void move_cursor(pTableCtx_t table, direction_t direction) {
    int8_t x = 0, y = 0;
    cell_t *prev_cell = get_cell(table, table->cursor);
    /* TODO: Remove attrs from prev_cell */

    switch (direction) {
        case LEFT:
            --x;
            break;
        case RIGHT:
            ++x;
            break;
        case UP:
            --y;
            break;
        case DOWN:
            ++y;
            break;
    }

    table->cursor.x += x;
    if (table->cursor.x == table->vis_cols) {
        scroll_table(table, RIGHT);
    } else if (table->cursor.x < 0) {
        scroll_table(table, LEFT);
    }
    clamp_u64(&table->cursor.x, 0, table->vis_cols - 1);

    table->cursor.y += y;
    if (table->cursor.y == table->vis_rows) {
        scroll_table(table, DOWN);
    } else if (table->cursor.y < 0) {
        scroll_table(table, UP);
    }
    clamp_u64(&table->cursor.y, 0, table->vis_rows - 1);

    redraw_table(table);
}

/*void load_csv_data(csv_data);*/
/*void export_data_to_csv()*/

double atod(const char *a) {
    char c;
    double d = 0.0;
    unsigned counter;

    const char *integer = strtok(strdup(a), ".");
    const char *fraction = strtok(NULL, ".");

    counter = strlen(integer) - 1;
    while ((c = *integer++) != '\0') {
        if (c == '.') {
            break;
        } else if (!isdigit(c)) {
            return NAN;
        }

        d += (double)(c - '0') * pow(10, counter--);
    }

    counter = 1;
    while ((c = *fraction++) != '\0') {
        if (!isdigit(c)) {
            return NAN;
        }

        d += ((double)(c - '0') / pow(10, counter++));
    }

    return d;
}
