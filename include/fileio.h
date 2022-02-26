/**
 * The page is a collection of columns
 * The columns are a collection of cells
 * Each cell contains a string, but can only display a limited number of characters
*/

#ifndef FILEIO_H
#define FILEIO_H

#include <stddef.h>

#include "display.h"

#define MAX_DATA  30
/* DEF_ROWS/DEF_COLS just refer to the number of cells that get populated in defaults.conf */
#define DEF_ROWS  15
#define DEF_COLS  25
#define DEF_CONF 	"defaults.conf"
#define PG_START 	"[Page Start]"
#define PG_END	  	"[Page End]"
#define COL_START "[Col Start]"
#define COL_END	"[Col End]"
#define NUL_ENTRY	'\n'

typedef struct {
	char				 *data;
	size_t 			 size;
} cell;

typedef struct {
	unsigned short	 offset;
	cell 	 			 cell_data[MAX_OFSCR_ROWS];	
} col_arr;

typedef struct {
	unsigned short	 offset;
	col_arr	 		 col_data[MAX_OFSCR_COLS];
} page;

/* Functions */
int  get_year(void);
int  load_config(page *page_p, dimensions sdims, char *year);
int  create_config(page *page_p, dimensions sdims, char *year);
int  create_def_config(dimensions sdims);
bool check_existing(char *year);

#endif
