/**
 * The page is a collection of columns
 * The columns are a collection of cells
 * Each cell contains a string, but can only display a limited number of characters
*/
#ifndef FILEIO_H
#define FILEIO_H

#include <stddef.h>
#include <stdint.h>

#include "display.h"

#define MAX_DATA  30
/* DEF_ROWS/DEF_COLS just refer to the number of cells that get populated in defaults.conf */
#define DEF_ROWS  15
#define DEF_COLS  25
#define RES_DIR   "../res/"
#define DEF_CONF 	"default"
#define PG_START 	"[Page Start]"
#define PG_END	  	"[Page End]"
#define COL_START "[Col Start]"
#define COL_END	"[Col End]"
#define NUL_ENTRY	" "
#define PATH_LIM  256

typedef char*  cell_data_t;
extern uint8_t cell_size;

typedef struct {
	uint16_t    col_offset;                                  /* Column offset of the offscreen page for drawing onscreen cells */
	uint16_t    row_offset;                                  /* Row offset of the offscreen page for drawing onscreen cells */
	cell_data_t page_cells[MAX_OFSCR_COLS][MAX_OFSCR_ROWS];  /* The entire offscreen page of cells loaded from conf file */
} page_t;

static const char *top_row[DEF_COLS] = {
   "Jan Est", "Jan Act", "Feb Est", "Feb Act",
   "Mar Est", "Mar Act", "Apr Est", "Apr Act",
   "May Est", "May Act", "Jun Est", "Jun Act",
   "Jul Est", "Jul Act", "Aug Est", "Aug Act",
   "Sep Est", "Sep Act", "Oct Est", "Oct Act",
   "Nov Est", "Nov Act", "Dec Est", "Dec Act",
};

static const char *first_col[DEF_ROWS] = {
   "Profits", "", "Work", "Gifts", "Other", "",
   "Expenses", "", "Food", "Gas", "Hair Cuts", "Clothing", "Personal", "Hobby", "Other"
};

#endif /* FILEIO_H */
