/**
 * Briefing: Initialize and display the ncurses GUI
 * 
 * @author Neil Kingdom
 * @version 1.0
 * @since 10-25-2021
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <time.h>

#include "misc.h"
#include "common.h"
#include "nuget.h"

uint8_t cell_size;
WINDOW *content_win;
/**
 * Briefing: Initialize the GUI and other data
 * 
 * @returns dimensions* Screen dimensions struct
*/
void init_nuget_ui(dimensions *sdims_p, page *page_p) 
{
	char *year = NULL;

	/* TODO: Change to check for parameter eg. -n = no color */

	/* Check for color support in terminal (seems to return false for ST */
   #if 0
	if(has_colors() == FALSE) 
   {
		fprintf(stderr, "No color support detected: %d ", errno);
      nuget_perror(__FILE__, __FUNCTION__, __LINE__);
		endwin();
      exit(NUGET_ERR);
	}
   #endif

	/* Initialization */
	initscr();                     /* Start in curses mode */
	curs_set(INVISIBLE);
	keypad(stdscr, TRUE);
	raw();                         /* Take raw input from user ie. interpret special chars as raw bytes */
	noecho();                      /* Hide user input on stdscr */
	calc_cell_dimensions(sdims_p); /* Calculate the size of each cell */

	/* Color pairs */
	start_color();
	assume_default_colors(COLOR_WHITE, -1); /* Keeps attributes from terminal such as transparency on */
	attrset(COLOR_PAIR(COL_PAIR2));  		 /* Apply color pair the the entire window (stdwin is assumed) */

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

	/* Load config data */
	load_config(page_p, year);
	redraw(page_p, sdims_p, year);
	
	/* Initialize the content window */
	create_win(content_win, CWIN_HEIGHT * sdims_p->cell_height, sdims_p->win_width, sdims_p->win_height, 0);

	free(year);
	refresh();
}

/**
 * Briefing: Calculate the screen and cell width/height
 * 
 * @param sdims_p Pointer to screen dimensions struct
*/
void calc_cell_dimensions(dimensions *sdims_p) 
{
	getmaxyx(stdscr, (sdims_p->win_height), (sdims_p->win_width)); 
	sdims_p->win_height -= CWIN_HEIGHT; 										 /* Subtract CWIN_HEIGHT to make room for content window */

	sdims_p->cell_height = 1;   						                      /* Rows can take up all of the available space */
	sdims_p->cell_width  = floor(sdims_p->win_width / DEF_ONSCR_COLS); /* Divide rows into reasonably sized sections  */

	sdims_p->onscr_rows  = sdims_p->win_height / sdims_p->cell_height;
	sdims_p->onscr_cols  = sdims_p->win_width / sdims_p->cell_width;  

   /* Arbitrary subtraction of 2 to leave breathing room between cells */
   cell_size = (uint8_t)(sdims_p->cell_width - 2);
}

void *sdims_watchdog(void *args) 
{
   struct timespec delay;
   delay.tv_nsec = 100000000; /* 100ms */
   for (;;)
   {
      nanosleep(&delay, NULL);

      /* TODO: Implement */
      #if 0
      /* R/W ops : Lock mutex */
      pthread_mutex_lock(&mutex)       

      int onscr_rows_save = sdims_p->onscr_rows;
      int onscr_cols_save = sdims_p->onscr_cols;

      calc_cell_dimensions(sdims_p);
      
      if (onscr_rows_save != sdims_p->onscr_rows ||
          onscr_cols_save != sdims_p->onscr_cols)
      {
	      redraw(page_p, sdims_p, year);
      }
      
      /* Finished R/W ops : Safe to unlock */
      pthread_mutex_unlock(&mutex);
      #endif
   }
}

void create_win(WINDOW *win, int height, int width, int starty, int startx) 
{
	win = newwin(height, width, starty, startx);
	box(win, 0, 0); /* Border */
	wrefresh(win);
}
