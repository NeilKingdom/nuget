#include <ncurses.h>
#include <errno.h>
#include <string.h>
#include <math.h>
	
#define COL_PAIR1 init_pair(1, COLOR_WHITE, -1) /* -1 means no color, hence the background color wont exist with this attribute */
#define COL_PAIR2 init_pair(2, COLOR_WHITE, COLOR_CYAN)

/* TODO: Remove these. They are for temporary testing purposes */
#define COL 12
#define ROW 15

/* TODO: Change stupid method of checking letter to actually check keycode->keysym with keysym.h */
#define SHIFT -32
#define CURSOR_UP 'k'
#define CURSOR_DOWN 'j'
#define CURSOR_RIGHT 'l'
#define CURSOR_LEFT 'h'
#define SAVE SHIFT+'s'
#define QUIT 'q'

extern int errno;

/*******************************************************
							 Functions
*******************************************************/

void calc_cell_dimensions(double *d_sWidth, double *d_sHeight, int *colNum, int *rowNum) {

	getmaxyx(stdscr, *d_sHeight, *d_sWidth);	
	*rowNum = floor(*d_sHeight / ROW);
	*colNum = floor(*d_sWidth / COL);	
}

int main(int argc, char *argv[]) {

	/* Variable definitions */
	int x, y, c;
	int colNum, rowNum;
	double d_sWidth, d_sHeight; 
	const char* title = "Nuget: The ncurses Budgeting Software";

	/* TODO: Change to check for parameter eg. -n = no color */
	/* Check for color support in terminal */
	/*if(has_colors() == FALSE) {
		endwin();
		fprintf(stderr, "Error: %d ", errno);
		perror("");
		return -1;
	} */

	/* Initialization */
	initscr();	/* Start in curses mode */
	curs_set(0); /* 0 = invisible, 1 = normal, 2 = blinking */
	keypad(stdscr, TRUE);	
	raw(); /* Take raw input from user ie. interpret special chars as raw bytes */
	noecho(); /* Hide user input on stdscr */

	/* TODO: Make color((s) pairs) easy to change via macros or enums */
	/* Create color pairs */
	start_color();
	assume_default_colors(COLOR_WHITE, -1); /* Keeps attributes from terminal such as transparancy on */
	attrset(COLOR_PAIR(COL_PAIR1)); /* Apply color pair to the entire window (stdwin is assumed) */

	calc_cell_dimensions(&d_sWidth, &d_sHeight, &colNum, &rowNum);
	mvprintw(1, d_sWidth/2 - strlen(title)/2, title);

/*******************************************************
							 Main Loop
*******************************************************/

	while((c = getch()) != QUIT) {
		switch(c) {
			case CURSOR_UP: 
					printw("UP\n");	
					refresh();
				break; 
			case CURSOR_DOWN:
					printw("DOWN\n");	
					refresh();
				break; 
			case CURSOR_RIGHT:
					printw("RIGHT\n");	
					refresh();
				break; 
			case CURSOR_LEFT:
					printw("LEFT\n");	
					refresh();
				break; 
			case SAVE:
					printw("Saved\n");	
					refresh();
				break; 
		}
	}

	endwin();
	return 0;
}
