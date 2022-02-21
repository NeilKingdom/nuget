/**
 * Briefing: Initialize and display the ncurses GUI
 * 
 * @author Neil Kingdom
 * @version 1.0
 * @since 10-25-2021
*/
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "display.h"
#include "nuget.h"
#include "fileio.h"

/**
 * Briefing: Initialize the GUI and other data
 * 
 * @returns dimensions* Screen dimensions struct
*/
dimensions *init_stdscr(void) {
	static dimensions sdims = { 0 }; /* stdscr dimensions */
	dimensions *sdims_p = NULL; 
	WINDOW *content_win;
	page layout; 
	int iyear, i;
	char *year;

	/* TODO: Change to check for parameter eg. -n = no color */
	/* Check for color support in terminal */
	/* Seems to return FALSE for ST */
	/*if(has_colors() == FALSE) {
		endwin();
		fprintf(stderr, "Error: %d ", errno);
		return NUGET_ERR;
	}*/

	/* Initialization */
	initscr(); /* Start in curses mode */
	curs_set(INVISIBLE);
	keypad(stdscr, TRUE);
	raw(); /* Take raw input from user ie. interpret special chars as raw bytes */
	noecho(); /* Hide user input on stdscr */
	calc_cell_dimensions(&sdims); /* Calculate the size of each cell */

	/* Color pairs */
	start_color();
	assume_default_colors(COLOR_WHITE, -1); /* Keeps attributes from terminal such as transparency on */
	attrset(COLOR_PAIR(COL_PAIR2));  		 /* Apply color pair the the entire window (stdwin is assumed) */
	apply_attrs(1, 1, 0, 2, COL_PAIR2, sdims);

	/* Print year */
	iyear = get_year();
	year = malloc(5 * sizeof(char));
	for (i = 0; i < 4; i++) {
		year[4 - i - 1] = iyear % 10;
		iyear %= 10;
	}
	year[4] = '\0';
	mvprintw(0, (sdims.win_width/2) - (strlen(year)/2), year);

	/* Load config data */
	load_config(&layout, sdims, year);	
	write_to_screen(&layout, sdims);
	
	/* Initialize the content window */
	content_win = create_win(CWIN_HEIGHT * sdims.cell_height, sdims.win_width, sdims.win_height, 0);
	mvwprintw(content_win, 0, 0, "Testing the new window...");

	refresh();
	free(year);
	sdims_p = &sdims;
	return sdims_p;
}

/**
 * Briefing: Calculate the screen and cell width/height
 * 
 * @param sdims_p Pointer to screen dimensions struct
*/
void calc_cell_dimensions(dimensions *sdims_p) {
	/* getmaxyx is a macro so apply caution when passing arguments */
	getmaxyx(stdscr, (sdims_p->win_height), (sdims_p->win_width)); 
	sdims_p->win_height -= CWIN_HEIGHT; 										  /* Subtract CWIN_HEIGHT to make room for content window */

	sdims_p->cell_height = 1;   						                       /* Rows can take up all of the available space */
	sdims_p->cell_width  = floor(sdims_p->win_width / DEF_ONSCR_COLS);  /* Divide rows into reasonably sized sections  */

	sdims_p->onscr_rows  = sdims_p->win_height / sdims_p->cell_height;
	sdims_p->onscr_cols  = sdims_p->win_width / sdims_p->cell_width;  
}

void apply_attrs(int row_xbegin, int row_ybegin, int col_xbegin, int col_ybegin, int col_pair, dimensions sdims) {
	int i;
	for (i = 0; i < sdims.onscr_cols; i++) 
		mvchgat(row_ybegin, row_xbegin + i, sdims.cell_width, A_NORMAL, col_pair, NULL);		
	for (i = 0; i < sdims.onscr_rows; i++) 
		mvchgat(col_ybegin + i, col_xbegin, sdims.cell_width, A_NORMAL, col_pair, NULL);
	refresh();
}

WINDOW *create_win(int height, int width, int starty, int startx) {
	WINDOW *win;
	win = newwin(height, width, starty, startx);
	box(win, 0, 0); /* Border */
	wrefresh(win);

	return win;
}
