/**
 * Briefing: Read/Write data to the configuration files
 *
 * @author Neil Kingdom
 * @version 1.0
 * @since 10-25-2021
*/

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>

#include "fileio.h"
#include "nuget.h"

/**
 * Briefing: Load the default page data into page struct
 *
 * @param data_p Page struct containing info about page layout
 * @return Returns exit status of the function
*/
int load_defaults(page *data_p) {
	int i;
	const char *top = "A";
	const char *left = "0";

	if (!data_p->year) {
		fprintf(stderr, "Error!! File: %s, Function: %s, Line: %d\n", __FILE__, __FUNCTION__, __LINE__);
		fprintf(stderr, "Member variable 'year' must be initialized in struct data_p\n");
		return NUGET_ERR;
	}

	data_p->col_data = malloc(MAX_COLS * 2 * sizeof(top));
	data_p->row_data = malloc(MAX_ROWS * sizeof(left));

	for (i = 0; i < MAX_COLS * 2; i++) 
		*(data_p->col_data + i) = "A";

	for (i = 0; i < MAX_ROWS; i++) 
		*(data_p->row_data + i) = "0";

	return 0;
}

/**
 * Briefing: Write data onto the screen
 * 
 * @param data_p Page struct containing info about page layout
 * @param sdims Struct containing info about screen dimensions
*/
void write_defaults(page *data_p, dimensions sdims) {
	int i;
	unsigned x, y, cell_width, cell_height;	

	cell_width = sdims.cell_width;
	cell_height = sdims.cell_height;

	/* Print topbar data */
  	for (i = 0, x = cell_width, y = cell_height; x < (MAX_COLS * cell_width) * 2; x += cell_width, i++) {
		mvprintw(y, x, "%s", *(data_p->col_data + i));
		nuget_mvchgat(y, x, cell_width, A_NORMAL, 2);
	}

	/* Print sidebar data */
	for (i = 0, y = cell_height * 2; y < (MAX_ROWS * cell_height); y += cell_height, i++) {
		mvprintw(y, 0, "%s", *(data_p->row_data + i));	
		nuget_mvchgat(y, 0, cell_width, A_NORMAL, 2);
	}

	refresh();
	return;
}

/**
 * Briefing: Check if a configuration file for the current year
 * 			 exists in order to decide if it should be loaded or not
 *
 * @param year String representing the current year
 * @return Returns true if file exists, otherwise returns false
*/
bool check_existing(char *year) {

}

/**
 * Briefing: Load an existing config file into page struct 
 * 
 * @return Returns exit status of the function
*/
int load_existing(void) {
	FILE* fp;
	fp = fopen("../tmp/defaults.conf", "r");
	
	if(fp == NULL) {
		perror("Failed to open defaults.conf");
		fclose(fp);
		return NUGET_ERR;
	}

	fclose(fp);
	return 0;	
}

/**
 * Briefing: Write existing data onto the screen
 * 
 * @param data_p Page struct containing info about the layout of the page
 * @param sdims Struct containing info about screen dimensions
*/
void write_existing(page *data_p, dimensions sdims) {

	return;
}

/**
 * Briefing: Return the current year
*/
char *get_year(void) {
	time_t t;
	struct tm *time_info;
	char *year;

	time(&t);
	time_info = localtime(&t);
	/* I don't like this */
	year = asctime(time_info)+20;

	return year;
}
