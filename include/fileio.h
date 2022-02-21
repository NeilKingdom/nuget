#ifndef FILEIO_H
#define FILEIO_H

#include <stddef.h>

#include "display.h"

#define MAX_COLS 24
#define MAX_ROWS 50
#define DEF_CONF "defaults.conf"

typedef struct {
	char				 *cell_data;
	size_t 			 cell_size;
} cell;

typedef struct {
	unsigned short	 col_offset;
	cell 	 			 col_data[MAX_COLS];	
} col_arr;

typedef struct {
	unsigned short	 row_offset;
	col_arr	 		 row_arr[MAX_ROWS];
} page;

enum cSection = { SEC_COL, SEC_ROW, SEC_PAGE };
char *def_row[] = { "Jan Est", "Jan Act", "Feb Est", "Feb Act", "Mar Est", "Mar Act", "Apr Est", "Apr Act", "May Est", "May Act", "Jun Est", "Jun Act",
						  "Jul Est", "Jul Act", "Aug Est", "Aug Act", "Sep Est", "Sep Act", "Oct Est", "Oct Act", "Nov Est", "Nov Act", "Dec Est", "Dec Act" };
char *def_col[] = { "Profits", "", "Work", "Gifts", "Other", "", "Expenses", "", "Food", "Gas", "Haircut", "Bills", "Personal", "Other" };

/* Functions */
char *get_year(void);
int  create_defaults(void);
int  load_config(page *page_p);
bool check_existing(char *year);
void write_to_screen(page *page_p, dimensions dims);

#endif
