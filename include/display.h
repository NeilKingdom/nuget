/**
 * 						
 *
 *		 		   col 1    col 2     col 3    col 4    col 5    col 6    col 7
 *		 		  --------------------------------------------------------------
 *	  row 1	 |			|			|			|			|			|			|			| ........
 *		 		  --------------------------------------------------------------
 *	  row 2	 |			|			|			|			|			|			|			| ........
 *		 		  --------------------------------------------------------------
 *	  row 3	 |			|			|			|			|			|			|			| ........
 *		 		  --------------------------------------------------------------
 *	  row 4 	 |			|			|			|			|			|			|			| ........
 *		 		  --------------------------------------------------------------
 * 											...............
 *											
 *				  --------------------------------------------------------------
 *           |								 content window								 | 
 *           |																					 | 
 *           |																					 | 
 *				  --------------------------------------------------------------
*/

#ifndef DISPLAY_H
#define DISPLAY_H

#include <ncurses.h>

#define DEF_ONSCR_COLS 10
/* Don't need DEF_ONSCR_ROWS;
   We let screen size determine the number of rows */
#define MAX_ONSCR_COLS 20 
#define MAX_ONSCR_ROWS 80
#define MAX_OFSCR_COLS 50		  
#define MAX_OFSCR_ROWS 150		  
#define CWIN_HEIGHT    5

/* -1 means no color, therefore, the background color wont exist with this attribute */
#define COL_PAIR1 init_pair(1, COLOR_BLACK, (-1))
#define COL_PAIR2 init_pair(2, COLOR_BLACK, COLOR_CYAN)

typedef struct {
	int 		cell_width;  /* Width of each cell    */
	int 		cell_height; /* Height of each cell   */
	float 	win_width;	 /* Width of stdscr       */
	float 	win_height;  /* Height of stdscr      */
	size_t 	onscr_rows;  /* Num of on-screen rows */
	size_t	onscr_cols;  /* Num of on-screen cols */
} dimensions;

extern dimensions sdims;
extern WINDOW 		*content_win;
enum cursorMode { INVISIBLE, NORMAL, BLINKING };

/* Functions */
void init_nuget_ui(void);
void calc_cell_dimensions(dimensions *dims_p);
/*void redraw(page *page, dimensions sdims);*/
WINDOW *create_win(int height, int width, int starty, int startx);
void *sdims_watchdog(void *);

#endif
