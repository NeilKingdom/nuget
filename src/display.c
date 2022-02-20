/**
 * Briefing: Initialize and display the ncurses GUI
 * 
 * @author Neil Kingdom
 * @version 1.0
 * @since 10-25-2021
*/

#include <ncurses.h>
#include <string.h>
#include <math.h>

#include "display.h"
#include "nuget.h"
#include "actions.h"
#include "fileio.h"

/**
 * Briefing: Initialize the GUI and other data
 * 
 * @returns dimensions* Screen dimensions struct
*/
dimensions *init_display(void) {
	dimensions *sdims;
	page def_layout; 
	char *year;
	const char *header = "Nuget: The ncurses Budgeting Software";

	enum cursorMode { INVISIBLE, NORMAL, BLINKING };

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
	attrset(COLOR_PAIR(COL_PAIR2));  /* Apply color pair the the entire window (stdwin is assumed) */

	calc_cell_dimensions(sdims);

	year = get_year();
	/* Set year */

	/* Print header title */
	mvprintw(0, 0, header);
	mvprintw(0, (sdims->swidth/2) - (strlen(year)/2), year);

	if (!check_existing(year)) {
		load_defaults(&def_layout);	
		write_defaults(&def_layout, *sdims);
	}
	
	else {
		printf("Found the file %s.conf\n", def_layout.year);
	}

	return sdims;
}

/**
 * Briefing: Calculate the screen and cell width/height
 * 
 * @param sdims_p Pointer to screen dimensions struct
*/
void calc_cell_dimensions(dimensions *sdims_p) {
	getmaxyx(stdscr, sdims_p->sheight, sdims_p->swidth);
	sdims_p->cell_height = floor(sdims_p->sheight / MAX_ROWS);
	sdims_p->cell_width  = floor(sdims_p->swidth / MAX_COLS);
}
