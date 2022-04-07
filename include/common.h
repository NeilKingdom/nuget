/*
   This header is simply to alleviate undefined reference
   issues due to recursive #include(ing) between display.h
   and fileio.h
*/
#ifndef COMMON_H
#define COMMON_H

/* fileio.h includes display.h */
#include "fileio.h"

/* fileio functions */
int  create_def_config(void);
int  create_config(dimensions *dims_p, char *year);
int  load_config(page *page_p, char *year);
int  page_init(page *page_p);
int  page_cleanup(page *page_p);
void redraw(page *page_p, dimensions *dims_p, char *year);
bool check_existing(char *fname);

/* display functions */
void init_nuget_ui(dimensions *dims_p, page *page_p);
void calc_cell_dimensions(dimensions *dims_p);
void create_win(WINDOW *win, int height, int width, int starty, int startx);
void *sdims_watchdog(void *args);

#endif /* COMMON_H */
