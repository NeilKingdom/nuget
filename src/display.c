/**
 * Briefing: Initialize and display the ncurses GUI
 * 
 * @author Neil Kingdom
 * @version 1.0
 * @since 10-25-2021
*/

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
dimensions *init_display(void) {
	extern WINDOW content_win;
	static dimensions sdims = { 0 }; /* stdscr dimensions */
	dimensions *sdims_p = NULL; 
	page layout; 
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

	/* Color pairs */
	start_color();
	assume_default_colors(COLOR_WHITE, -1); /* Keeps attributes from terminal such as transparency on */
	attrset(COLOR_PAIR(COL_PAIR2));  		 /* Apply color pair the the entire window (stdwin is assumed) */

	calc_cell_dimensions(&sdims);
	year = get_year();

	/* Print year */
	mvprintw(0, (sdims.win_width/2) - (strlen(year)/2), year);

	/* Load config data */
	load_config(&layout);	
	write_to_screen(&layout, sdims);
	
	/* Initialize the content window */
	content_win = create_win(CWIN_HEIGHT * sdims.cell_height, sdims.win_width, sdims.win_height, 0);
	mvprintw(content_win, 0, 0, "Testing the new window...");
	refresh();

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
	getmaxyx(stdscr, (sdims_p->sheight), (sdims_p->swidth)); 
	sdims_p->win_height -= CWIN_HEIGHT;
	sdims_p->cell_height = floor(sdims_p->sheight); 
	sdims_p->cell_width  = floor(sdims_p->swidth / MAX_ONSCR_COLS);
}

WINDOW *create_win(int height, int width, int starty, int startx) {
	WINDOW *win;
	win = newwin(height, width, starty, startx);
	box(win, 0, 0);
	wrefresh(win);

	return win;
}
