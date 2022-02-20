#ifndef FILEIO_H
#define FILEIO_H

#include "display.h"

#define MAX_COLS 12
#define MAX_ROWS 15

typedef struct {
	const char *	ch_data;
	size_t 			cell_size;
} cell;

typedef struct {
	char* 	year;
	char** 	col_data;
	char** 	row_data;
	cell* 	cell_data;
} page;

/* Functions */
int load_defaults(page *data_p);
void write_defaults(page *data_p, dimensions sdims);
int check_existing(char* year);
int load_existing(void);
void write_existing(page *data_p, dimensions sdims);
char *get_year(void);

#endif
