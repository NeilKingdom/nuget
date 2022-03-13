/**
 * Briefing: Starts the program and handles key-press events
 *
 * @author Neil Kindom
 * @version 1.0
 * @since 10-25-2021
*/
#include <ncurses.h>

#include "nuget.h"
#include "display.h"

int main(int argc, char *argv[]) {
	char c;
	int tb, rb, bb, lb; 							 /* Boundaries: [top, right, bottom, left] */
	unsigned x, y, cell_width, cell_height;
	WINDOW *content_win = NULL;
	dimensions sdims;

	init_nuget_ui(&sdims);
	cell_width  = sdims.cell_width;
	cell_height = sdims.cell_height;
	/* Start cursor at correct position (x: 1, y: 3) */
	x = cell_width;
	y = cell_height * 2;
	move(y, x);

	mvchgat(cell_height, x, cell_width, A_BOLD, 1, NULL); 
	mvchgat(y, 0, cell_width, A_BOLD, 1, NULL);
	mvchgat(y, x, cell_width, A_BOLD, 2, NULL);
	refresh();

	while((c = getch()) != K_QUIT) {
		switch(c) {
			case K_ENTER:
				;;
				break;

			case K_CURSOR_LEFT:
					/* Set previous attrs back to normal */
					mvchgat(cell_height, x, cell_width, A_NORMAL, 2, NULL); /* Row selected */
					mvchgat(y, 0, cell_width, A_NORMAL, 2, NULL); 			  /* Column selected */
					mvchgat(y, x, cell_width, A_NORMAL, 0, NULL); 			  /* Current cell */
					x -= cell_width;
				break;

			case K_CURSOR_DOWN:
					mvchgat(cell_height, x, cell_width, A_NORMAL, 2, NULL);
					mvchgat(y, 0, cell_width, A_NORMAL, 2, NULL);
					mvchgat(y, x, cell_width, A_NORMAL, 0, NULL);
					y += cell_height;
				break;

			case K_CURSOR_UP:
					mvchgat(cell_height, x, cell_width, A_NORMAL, 2, NULL);
					mvchgat(y, 0, cell_width, A_NORMAL, 2, NULL);
					mvchgat(y, x, cell_width, A_NORMAL, 0, NULL);
					y -= cell_height;
				break;

			case K_CURSOR_RIGHT:
					mvchgat(cell_height, x, cell_width, A_NORMAL, 2, NULL);
					mvchgat(y, 0, cell_width, A_NORMAL, 2, NULL);
					mvchgat(y, x, cell_width, A_NORMAL, 0, NULL);
					x += cell_width;
				break;

			case K_SAVE:
				;;
				break;
		}

		move(y, x);
		/* Update currently selected attrs */
		mvchgat(cell_height, x, cell_width, A_BOLD, 1, NULL); 
		mvchgat(y, 0, cell_width, A_BOLD, 1, NULL);
		mvchgat(y, x, cell_width, A_BOLD, 2, NULL);
		refresh();
	} 

	delwin(content_win);
	endwin();
	return 0;
}
