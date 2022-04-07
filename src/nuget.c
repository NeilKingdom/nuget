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
#include "display.h"
#include "fileio.h"

int main(int argc, char *argv[]) 
{
	char c;
   char *year = NULL;
	unsigned rb, bb, lb = 0, tb = 0;  /* Boundaries: [Right, Bottom, Left, Top] */
	unsigned x, y, cell_width, cell_height;
	WINDOW *content_win = NULL;
	dimensions sdims;

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

	init_nuget_ui(&sdims);
   uint8_t cell_size = (uint8_t)(sdims.cell_width - 2);
	cell_width        = sdims.cell_width;
	cell_height       = sdims.cell_height;
   rb                = sdims.onscr_cols * cell_width;
   bb                = sdims.onscr_rows * cell_height;

	/* Start cursor at correct position (x: 1, y: 3) */
	x = cell_width;
	y = cell_height * 2;
	move(y, x);

   /* Set initial color/font attributes */
	mvchgat(cell_height, x, cell_width, A_BOLD, 1, NULL); 
	mvchgat(y, 0, cell_width, A_BOLD, 1, NULL);
	mvchgat(y, x, cell_width, A_BOLD, 2, NULL);
	refresh();

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
                  /* TODO: sdims should be page */
                  #if 0
                  if (sdims.col_offset > 0)
                     sdims.col_offset -= 1; 
                  #endif 
                  /* TODO: call redraw */
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
                  /* TODO: sdims should be page */
                  #if 0
                  if (sdims.row_offset < MAX_OFSCR_ROWS)
                     sdims.row_offset += 1;
                  #endif 
                  /* TODO: call redraw */
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
                  /* TODO: sdims should be page */
                  #if 0
                  if (sdims.row_offset > 0)
                     sdims.row_offset -= 1;
                  #endif 
                  /* TODO: call redraw */
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
                  /* TODO: sdims should be page */
                  #if 0
                  if (sdims.col_offset < MAX_OFSCR_COLS)
                     sdims.col_offset += 1; 
                  #endif 
                  /* TODO: call redraw */
               }
				break;

			case K_SAVE:
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
