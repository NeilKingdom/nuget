#ifndef TABLE_H
#define TABLE_H

#include "common.h"

#define MAX_ROWS    USHRT_MAX
#define MAX_COLS    26

extern uint8_t cell_cwidth; /* Current character width of a cell */
typedef char *cell_t;
typedef point_t cursor_t;

typedef struct {
    uint8_t  vis_cols;  /* The number of currently visible columns */
    uint8_t  vis_rows;  /* The number of currently visible rows */
    uint64_t offset_x;  /* The cell's absolute x-offset */
    uint64_t offset_y;  /* The cell's absolute y-offset */
    cursor_t cursor;    /* The cursor location (relative to top left corner) */
    cell_t  *cells;     /* 2d array of cells */
} tableCtx_t, *pTableCtx_t;

/* Forward function decls */

pTableCtx_t create_table_ctx(void);
void destroy_table_ctx(pTableCtx_t table);
cell_t *get_cell(pTableCtx_t table, const point_t location);
void draw_row_ids(pTableCtx_t table);
void draw_col_ids(pTableCtx_t table);
void draw_cell(pTableCtx_t table, const point_t location, const align_t align, const bool selected);
void redraw_table(pTableCtx_t table);
void scroll_table(pTableCtx_t table, const direction_t direction);
void move_cursor(pTableCtx_t table, const direction_t direction);
void update_cell_value(pTableCtx_t table, const char *value, const point_t location);

#endif /* TABLE_H */
