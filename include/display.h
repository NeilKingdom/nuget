#ifndef DISPLAY_H
#define DISPLAY_H

#include <ncurses.h>

#define MAX_ONSCR_COLS 10
#define CWIN_HEIGHT    5

/* -1 mean no color, therefore, the background color wont exist with this attribute */
#define COL_PAIR1 init_pair(1, COLOR_BLACK, (-1))
#define COL_PAIR2 init_pair(2, COLOR_BLACK, COLOR_CYAN)

typedef struct {
	int 		cell_width;
	int 		cell_height;
	float 	win_width;
	float 	win_height;
} dimensions;

extern WINDOW content_win;
enum cursorMode { INVISIBLE, NORMAL, BLINKING };

/* Functions */
dimensions *init_stdscr(void);
void calc_cell_dimensions(dimensions *dims_p);

#endif
