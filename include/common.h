#ifndef COMMON_H
#define COMMON_H

#include "fileio.h"

/* fileio functions */
int  create_def_config(void);
int  create_config(dimensions_t *dims_p, char *year);
int  load_config(page_t *page_p, char *year);
int  page_init(page_t *page_p);
int  page_cleanup(page_t *page_p);
void redraw(page_t *page_p, dimensions_t *dims_p, char *year);
bool check_existing(char *fname);

/* display functions */
void  init_nuget_ui(dimensions_t *dims_p, page_t *page_p);
void  calc_cell_dimensions(dimensions_t *dims_p);
void  create_win(WINDOW *win, int height, int width, int starty, int startx);
void *sdims_chg_callback(void *args);

#endif /* COMMON_H */
