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
} *tableCtx_t;

/* Forward function decls */

tableCtx_t create_table_ctx(void);
void destroy_table_ctx(tableCtx_t table);
cell_t get_cell(tableCtx_t table, point_t location);
void draw_cell(tableCtx_t table, point_t location, bool selected);
void redraw_table(tableCtx_t table);
void scroll_table(tableCtx_t table, direction_t direction);
void move_cursor(tableCtx_t table, direction_t direction);
void update_cell_value(tableCtx_t table, const char *value, point_t location);
/*void load_csv_data(csv_data);*/
/*void export_data_to_csv()*/
double atod(const char *a);

#endif /* TABLE_H */
