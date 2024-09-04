#ifndef TABLE_H
#define TABLE_H

#include "common.h"

#define MAX_ROWS    USHRT_MAX
#define MAX_COLS    (pow(26, 2))

extern uint8_t cell_cwidth; /* Character width of a cell */

typedef char *cell_t;
typedef Point_t cursor_t;

typedef struct {
    uint8_t  vis_cols;  /* The number of currently visible columns */
    uint8_t  vis_rows;  /* The number of currently visible rows */
    uint64_t offset_x;  /* The cell's absolute x-offset */
    uint64_t offset_y;  /* The cell's absolute y-offset */
    cursor_t cursor;    /* The cursor location (relative to top left corner) */
    cell_t  *cells;     /* 2D array of cells */
} TableCtx_t;

/* Forward function decls */

TableCtx_t      *create_table_ctx(void);
void             destroy_table_ctx(TableCtx_t *table);
cell_t          *get_cell_value(TableCtx_t *table, const Point_t location);
void             set_cell_value(TableCtx_t *table, const char *value, const Point_t location);
void             draw_row_ids(TableCtx_t *table);
void             draw_col_ids(TableCtx_t *table);
void             draw_cell(TableCtx_t *table, const Point_t location, const Align_t align, const bool selected);
void             redraw_table(TableCtx_t *table);
void             scroll_table(TableCtx_t *table, const Direction_t direction);

#endif /* TABLE_H */
