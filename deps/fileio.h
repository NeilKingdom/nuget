#ifndef FILEIO_H
#define FILEIO_H

#include "display.h"

#define MAX_COLS 12
#define MAX_ROWS 15

typedef struct Cell {
	const char* ch_data;
	size_t cell_size;
} cell;

typedef struct Page {

	char* s_year;
	char** s_col_data;
	char** s_row_data;
	cell* cellData;
} page;

/* Functions */
void set_year(page* p_data, char* year);
int load_defaults(page* p_data);
void write_defaults(page* p_data, dimensions sDimensions);
int check_existing(char* year);
int load_existing(void);
void write_existing(page* p_data, dimensions sDimensions);

/* Getters */
char* get_year(void);

#endif
