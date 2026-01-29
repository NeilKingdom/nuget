#ifndef TABLE_H
#define TABLE_H

#include "common.h"

#define MAX_ROWS USHRT_MAX
#define MAX_COLS (pow(26, 2) + 1)

typedef struct {
    char *str;
    unsigned len;
} Cell_t;

typedef struct {
    uint8_t nvis_cols;     // The number of currently visible columns on the page
    uint8_t nvis_rows;     // The number of currently visible rows on the page
    uint8_t cell_width;    // Character width of an individual cell
    Point_t abs_offset;    // Offset relative to the absolute coordinates
    Point_t table_offset;  // Offset relative to the screen's coordinates
    Point_t cursor;        // The cursor location relative to the screen's coordinates
    Cell_t *arena;         // Table data
} TableCtx_t;

// Forward function declarations

TableCtx_t     *create_table_ctx(void);
void            destroy_table_ctx(TableCtx_t *table);
Cell_t         *get_cell(TableCtx_t *table, const Point_t location);
void            set_cell_text(TableCtx_t *table, const Point_t location, const char* const text, const size_t text_len);
void            render_cell(TableCtx_t *table, const Point_t location, const Align_t align, const bool selected);
void            render_row_labels(TableCtx_t *table);
void            render_col_labels(TableCtx_t *table);
void            scroll_table(TableCtx_t *table, const Direction_t direction);
void            refresh_table(TableCtx_t *table);

#endif
