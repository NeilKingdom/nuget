/**
 * Briefing: Starts the program and handles key-press events
 *
 * @author Neil Kindom
 * @version 1.0
 * @since 10-25-2021
*/
#include <ncurses.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "nuget.h"
#include "misc.h"
#include "common.h"

int main(int argc, char *argv[]) 
{
	char c;
   char *year = NULL;
	unsigned tb, rb, bb, lb;  /* Boundaries: [Top, Right, Bottom, Left] */
	unsigned x, y, cell_width, cell_height;
	WINDOW *content_win = NULL;
   page_t layout = { 0 };
	dimensions_t sdims = { 0 };

   #if 0
   pthread_t watch_thread;
   int err = pthread_create(&watch_thread, NULL, sdims_watchdog(NULL), NULL);
   if (err != 0)
   {
      fprintf(stderr, "Failed to create pthread\n"); 
      nuget_perror(__FILE__, __FUNCTION__, __LINE__);
      return err;
   } 
   #endif

	init_nuget_ui(&sdims, &layout);

   /* Sub 2 to leave breathing room between cells */
   cell_size         = (uint8_t)(sdims.cell_width - 2); 
	cell_width        = sdims.cell_width;
	cell_height       = sdims.cell_height;
   tb                = TOP_ROW_GAP * cell_height;
   rb                = sdims.onscr_cols * cell_width;
   bb                = (sdims.onscr_rows * cell_height) - cell_height;
   lb                = FIRST_COL_GAP * cell_width;

	/* Start cursor at correct position (x: 1, y: 3) */
	x = cell_width;
	y = cell_height * 2;
	move(y, x);

   /* Set initial color/font attributes */
	mvchgat(cell_height, x, cell_width, A_BOLD, 1, NULL); 
	mvchgat(y, 0, cell_width, A_BOLD, 1, NULL);
	mvchgat(y, x, cell_width, A_BOLD, 2, NULL);
	refresh();

   /* Get current year */
   year = malloc(sizeof(char) * (4 + 1));
   if (year == NULL)
   {
      fprintf(stderr, "Failed to allocate memory: %s\n", strerror(errno));
      nuget_perror(__FILE__, __FUNCTION__, __LINE__);
      endwin();
      exit(NUGET_ERR); 
   }

   /* Convert year to a string */
   if (NUGET_ERR == nuget_itoa(get_year(), year, 10, 4))
   {
      fprintf(stderr, "Divide by zero is not acceptable\n");
      year = "ERROR";
   }

   /* Main loop */
	while((c = getch()) != K_QUIT) 
   {
		switch(c) 
      {
			case K_ENTER:
				;;
				break;

			case K_CURSOR_LEFT:
               if (x > lb)
               {
                  /* Set previous attrs back to normal */
                  mvchgat(cell_height, x, cell_width, A_NORMAL, 2, NULL); /* Row selected */
                  mvchgat(y, 0, cell_width, A_NORMAL, 2, NULL); 			  /* Column selected */
                  mvchgat(y, x, cell_width, A_NORMAL, 0, NULL); 			  /* Current cell */
                  x -= cell_width;
               }
               else 
               {
                  if (layout.col_offset > 0)
                     layout.col_offset -= 1; 
                  redraw(&layout, &sdims, year);
               }
				break;

			case K_CURSOR_DOWN:
               if (y < bb)
               {
                  mvchgat(cell_height, x, cell_width, A_NORMAL, 2, NULL);
                  mvchgat(y, 0, cell_width, A_NORMAL, 2, NULL);
                  mvchgat(y, x, cell_width, A_NORMAL, 0, NULL);
                  y += cell_height;
               }
               else 
               {
                  if (layout.row_offset < MAX_OFSCR_ROWS)
                     layout.row_offset += 1;
                  redraw(&layout, &sdims, year);
               }
				break;

			case K_CURSOR_UP:
               if (y > tb)
               {
                  mvchgat(cell_height, x, cell_width, A_NORMAL, 2, NULL);
                  mvchgat(y, 0, cell_width, A_NORMAL, 2, NULL);
                  mvchgat(y, x, cell_width, A_NORMAL, 0, NULL);
                  y -= cell_height;
               }
               else 
               {
                  if (layout.row_offset > 0)
                     layout.row_offset -= 1;
                  redraw(&layout, &sdims, year);
               }
				break;

			case K_CURSOR_RIGHT:
               if (x < rb)
               {
                  mvchgat(cell_height, x, cell_width, A_NORMAL, 2, NULL);
                  mvchgat(y, 0, cell_width, A_NORMAL, 2, NULL);
                  mvchgat(y, x, cell_width, A_NORMAL, 0, NULL);
                  x += cell_width;
               }
               else 
               {
                  if (layout.col_offset < MAX_OFSCR_COLS)
                     layout.col_offset += 1; 
                  redraw(&layout, &sdims, year);
               }
				break;

			case K_SAVE:
            create_config(&sdims, year);
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
   #if 0
   (void) pthread_join(watch_thread, NULL);
   #endif
   
	return 0;
}
