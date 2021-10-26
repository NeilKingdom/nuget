#include <ncurses.h>

#include "../deps/nuget.h"
#include "../deps/actions.h"
#include "../deps/display.h"

int main(int argc, char* argv[]) {

	int x = 0, y = 0, c, cell_width, cell_height;

	dimensions* sDimensions = init_display();
	cell_width = sDimensions->cell_width;
	cell_height = sDimensions->cell_height;

/*******************************************
					Main Loop
*******************************************/

	while((c = getch()) != QUIT) {
		switch(c) {
			case ENTER:
				break;

			case CURSOR_LEFT:
				/*if(x > 0) {*/
					nuget_mvchgat(cell_height, x, cell_width, A_NORMAL, 0);
					nuget_mvchgat(y, 0, cell_width, A_NORMAL, 0);

					nuget_mvchgat(y, x, cell_width, A_NORMAL, 0);
					x -= cell_width;
				/*}*/
				break;

			case CURSOR_DOWN:
				/*if(y < sDimensions->f_sHeight) {*/
					nuget_mvchgat(cell_height, x, cell_width, A_NORMAL, 0);
					nuget_mvchgat(y, 0, cell_width, A_NORMAL, 0);

					nuget_mvchgat(y, x, cell_width, A_NORMAL, 0);
					y += cell_height;
				/*}*/
				break;

			case CURSOR_UP:
				/*if(y > cell_height) {*/
					nuget_mvchgat(cell_height, x, cell_width, A_NORMAL, 0);
					nuget_mvchgat(y, 0, cell_width, A_NORMAL, 0);

					nuget_mvchgat(y, x, cell_width, A_NORMAL, 0);
					y -= cell_height;
				/*}*/
				break;

			case CURSOR_RIGHT:
				/*if(x < sDimensions->f_sWidth) {*/
					nuget_mvchgat(cell_height, x, cell_width, A_NORMAL, 0);
					nuget_mvchgat(y, 0, cell_width, A_NORMAL, 0);

					nuget_mvchgat(y, x, cell_width, A_NORMAL, 0);
					x += cell_width;
				/*}*/
				break;

			case SAVE:
				break;
		}

		move(y, x);

		nuget_mvchgat(cell_height, x, cell_width, A_BOLD, 1);
		nuget_mvchgat(y, 0, cell_width, A_BOLD, 1);

		nuget_mvchgat(y, x, cell_width, A_BLINK, 2);
	} 

	endwin();
	return 0;
}
