#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "../deps/fileio.h"
#include "../deps/nuget.h"

/********************************************
					  Functions
********************************************/

/********************************************
Name: set_year
Author: Neil Kingdom
Date: Oct 25, 2021
Return: N/A
Params: p_data - page struct containing info
	about the layout of the page
		  s_year - the current year
Purpose: set the s_year member variable in
	page struct 
********************************************/
void set_year(page* p_data, char* s_year) {

	p_data->s_year = s_year;
	return;
}

/********************************************
Name: load_defaults
Author: Neil Kingdom
Date: Oct 25, 2021
Return: returns exit status
Params: p_data - page struct containing info
	about the layout of the page
Purpose: load the default page data into page
	struct
********************************************/
int load_defaults(page* p_data) {

	int i;
	const char* top = "A";
	const char* left = "0";

	if(!p_data->s_year) {
		printf("Error in: %s, line: %d\n", __FILE__, __LINE__);
		printf("Member variable 'year' must be initialized in struct p_data\n");
		return NUGET_ERR;
	}

	p_data->s_col_data = malloc(MAX_COLS * 2 * sizeof(top));
	p_data->s_row_data = malloc(MAX_ROWS * sizeof(left));

	/* TODO: I'm thinking that it actually would be best to create a defaults.txt file to read from */
	for(i = 0; i < MAX_COLS * 2; i++) 
		*(p_data->s_col_data+i) = "A";

	for(i = 0; i < MAX_ROWS; i++) 
		*(p_data->s_row_data+i) = "0";

	return 0;
}

/********************************************
Name: write_defaults
Author: Neil Kingdom
Date: Oct 25, 2021
Return: N/A
Params: p_data - page struct containing info
	about the layout of the page
		  sDimensions - struct containing info
	about screen dimensions
Purpose: write default data onto the screen
********************************************/
void write_defaults(page* p_data, dimensions sDimensions) {

	int x, y, i, cell_width, cell_height;	

	cell_width = sDimensions.cell_width;
	cell_height = sDimensions.cell_height;

	/* Print header data */
  	for(i = 0, x = cell_width, y = cell_height; x < MAX_COLS * cell_width * 2; x += cell_width, i++) 
		mvprintw(y, x, "%s", *(p_data->s_col_data+i));

	/* Print sidebar data */
	for(i = 0, y = 2 * cell_height; y < MAX_ROWS * cell_height; y += cell_height, i++) 
		mvprintw(y, 0, "%s", *(p_data->s_row_data+i));	

	return;
}

/********************************************
Name: check_existing
Author: Neil Kingdom
Date: Oct 25, 2021
Return: returns boolean indicating if the 
	file for the current year exists 
Params: s_year - the current year
Purpose: check if the file for the current
	year exists in order to decide if it 
	should be loaded or not
********************************************/
int check_existing(char* s_year) {

	/* For now, the programs assumes that files are located in ../tmp/ */	
	char* s_test_file = "../tmp/";

	/* TODO: Fix bug here 
	strcat(test_file, year);
	strcat(test_file, ".txt");
	*/

	/* Check if file exists already */
	if(access(s_test_file, F_OK) != 0)
		return TRUE;
	else
		return FALSE;
}

/********************************************
Name: load_existing
Author: Neil Kingdom
Date: Oct 25, 2021
Return: returns exit status
Params: N/A
Purpose: load an existing file into page 
	struct 
********************************************/
int load_existing(void) {

	FILE* p_file;
	/* Avoid byte mode so that the text files are human readable/configurable */
	p_file = fopen("../tmp/defaults.txt", "r");
	
	if(p_file == NULL) {
		printf("Error in: %s, line: %d\n", __FILE__, __LINE__);
		printf("Member variable 'year' must be initialized in struct p_data\n");
		fclose(p_file);
		return NUGET_ERR;
	}

	fclose(p_file);
	return 0;	
}

/********************************************
Name: load_defaults
Author: Neil Kingdom
Date: Oct 25, 2021
Return: N/A
Params: p_data - page struct containing info
	about the layout of the page
		  sDimensions - struct containing info
	about screen dimensions
Purpose: write existing data onto the screen
********************************************/
void write_existing(page* p_data, dimensions sDimensions) {

	return;
}

/********************************************
Name: get_year
Author: Neil Kingdom
Date: Oct 25, 2021
Return: returns the current year
Params: N/A
Purpose: return the current year
********************************************/
char* get_year(void) {

	time_t myTime;
	struct tm* timeInfo;
	char* s_year;

	time(&myTime);
	timeInfo = localtime(&myTime);
	s_year = asctime(timeInfo)+20;

	return s_year;
}
