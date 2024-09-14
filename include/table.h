#ifndef TABLE_H
#define TABLE_H

#include "common.h"

#define MAX_ROWS    USHRT_MAX
#define MAX_COLS    (pow(26, 2) + 1)

typedef char *cell_t;

typedef struct {
    uint8_t  nvis_cols;     /* The number of currently visible columns on the page */
    uint8_t  nvis_rows;     /* The number of currently visible rows on the page */
    uint8_t  cell_width;    /* Character width of an individual cell */
    Point_t  abs_offset;    /* Offset relative to the absolute top left coordinates*/
    Point_t  table_offset;  /* Offset relative to the screen's top left coordinates */
    Point_t  cursor;        /* The cursor location relative to the screen's coordinates */
    cell_t  *data;          /* Table data */
} TableCtx_t;

/* Forward function decls */

TableCtx_t     *create_table_ctx(void);
void            destroy_table_ctx(TableCtx_t *table);
cell_t         *get_cell_value(TableCtx_t *table, const Point_t location);
void            set_cell_value(TableCtx_t *table, const char *value, const Point_t location);
void            set_cell_attrs(TableCtx_t *table, const Point_t location, const NugetCol_t col_pair, const uint64_t attrs);
void            draw_cell(TableCtx_t *table, const Point_t location, const Align_t align, const bool selected);
void            draw_row_labels(TableCtx_t *table);
void            draw_col_labels(TableCtx_t *table);
void            refresh_table(TableCtx_t *table);
void            scroll_table(TableCtx_t *table, const Direction_t direction);

#endif /* TABLE_H */
