#include "../include/table.h"
#include "../include/curses_helpers.h"

uint8_t cell_cwidth = 0;

static void clamp(int *x, int hi, int lo) {
    *x = ((*x < lo) ? lo : *x) > hi ? hi : *x;
}

cell_t get_cell(tableCtx_t table, point_t location) {
    return table->cells[(location.y * MAX_COLS) + location.x];
}

tableCtx_t create_table_ctx(void) {
    unsigned x, y;
    tableCtx_t table = NULL;

    /* TODO: Divide by number of columns user wants */
    cell_cwidth = getmaxx(stdscr) / 12;

    /* Allocate memory for table struct */
    table = calloc(1, sizeof(tableCtx_t));
    if (table == NULL) {
        perror("Failed to allocate memory for table context");
        return NULL;
    }

    /* Allocate memory for cell references */
    table->cells = malloc(MAX_ROWS * MAX_COLS * sizeof(cell_t));
    if (table->cells == NULL) {
        perror("Failed to allocate memory for table cells");
        return NULL;
    }

    /* Allocate memory for cell contents */
    for (y = 0; y < MAX_ROWS; ++y) {
        for (x = 0; x < MAX_COLS; ++x) {
            cell_t cell = get_cell(table, (point_t){ x, y });
            cell = calloc(1, cell_cwidth + 1);
            if (cell == NULL) {
                perror("Failed to allocate memory for cell");
                return NULL;
            }
        }
    }

    return table;
}

void destroy_table_ctx(tableCtx_t restrict table) {
    unsigned x, y;

    if (table == NULL) {
        return;
    }

    if (table->cells) {
        for (y = 0; y < MAX_ROWS; ++y) {
            for (x = 0; x < MAX_COLS; ++x) {
                cell_t cell = get_cell(table, (point_t){ x, y });
                if (cell) {
                    free(cell);
                }
            }
        }
        free(table->cells);
    }

    free(table);
    table = NULL;
}

void draw_cell(tableCtx_t table, point_t location, bool selected) {
    cell_t cell = get_cell(table, location);

    if (selected) {
        attron(A_BOLD);
    }
    mvprintw(location.y, location.x * cell_cwidth, cell);
    chgat(strlen(cell), A_NORMAL, 1, NULL);
    if (selected) {
        attroff(A_BOLD);
    }
}

void redraw_table(tableCtx_t restrict table) {
    unsigned x, y;

    for (y = 0; y < table->vis_rows; ++y) {
        for (x = 0; x < table->vis_cols; x += cell_cwidth) {
            bool selected = (y == table->cursor.y && x == table->cursor.x);
            draw_cell(table, (point_t){ x, y }, selected);
        }
    }

    refresh();
}

void scroll_table(tableCtx_t restrict table, direction_t direction) {
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
    clamp(&table->offset_x, table->vis_cols, 0);
    table->offset_y += y;
    clamp(&table->offset_y, table->vis_rows, 0);

    redraw_table(table);
}

void move_cursor(tableCtx_t table, direction_t direction) {
    int8_t x = 0, y = 0;
    cell_t prev_cell = get_cell(table, table->cursor);

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
    clamp(&table->cursor.x, table->vis_cols, 0);
    table->cursor.y += y;
    clamp(&table->cursor.y, table->vis_rows, 0);

    redraw_table(table);
}

void update_cell_value(tableCtx_t table, const char * restrict value, point_t location) {
    cell_t cell = get_cell(table, location);
    strncpy(cell, value, cell_cwidth);
    cell[cell_cwidth] = '\0';
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
