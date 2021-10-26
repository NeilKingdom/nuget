#include <ncurses.h>
#include <string.h>
#include <math.h>

#include "../deps/display.h"
#include "../deps/nuget.h"
#include "../deps/actions.h"
#include "../deps/fileio.h"

/********************************************
                 Functions
********************************************/

/********************************************
Name: init_display
Author: Neil Kingdom
Date: Oct 25, 2021
Return: dimensions* - return screen 
	dimensions struct
Params: N/A 
Purpose: initialize and display the ncurses
	GUI
********************************************/
dimensions* init_display(void) {

	dimensions sDimensions;
	dimensions* ret = NULL;
	page defLayout; 
	char* s_year;
	const char* s_banner = "Nuget: The ncurses Budgeting Software";

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

	calc_cell_dimensions(&sDimensions);

	s_year = get_year();
	set_year(&defLayout, s_year);

	/* Print title bar */
	mvprintw(0, 0, s_banner);
	mvprintw(0, (sDimensions.f_sWidth/2) - (strlen(s_year)/2), s_year);

	if(check_existing(s_year) == FALSE) {
		load_defaults(&defLayout);	
		write_defaults(&defLayout, sDimensions);
	}
	
	else {
		printf("Found the file %s.txt\n", defLayout.s_year);
	}

	ret = &sDimensions;
	return ret;
}

/********************************************
Name: calc_cell_dimensions
Author: Neil Kingdom
Date: Oct 25, 2021
Return: N/A 
Params: p_sDimensions - pointer to screen
	dimensions struct 
Purpose: calculate the screen width, height,
	and the cell width, height  
********************************************/
void calc_cell_dimensions(dimensions* p_sDimensions) {

	getmaxyx(stdscr, p_sDimensions->f_sHeight, p_sDimensions->f_sWidth);
	p_sDimensions->cell_height = floor(p_sDimensions->f_sHeight/MAX_ROWS);
	p_sDimensions->cell_width = floor(p_sDimensions->f_sWidth/MAX_COLS);

	return;
}

/********************************************
Name: nuget_refresh
Author: Neil Kingdom
Date: Oct 25, 2021
Return: N/A 
Params: N/A 
Purpose: wrapper for ncurses refresh() func 
********************************************/
void nuget_refresh(void) {

	refresh();
	return;
}
