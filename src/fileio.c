/**
 * Briefing: Read/Write data to the configuration files
 *
 * @author Neil Kingdom
 * @version 1.0
 * @since 10-25-2021
*/

#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>

#include "fileio.h"
#include "nuget.h"
#include "misc.h"

/*
 * Layout of the default file:
 * 
 * [Page Start]
 * 	[Col Start]
 *			data
 * 		data
 *			
 * 		data
 *		[Col End]
 *    [Col Start]
 *			data
 * 	
 *	
 * 		data
 * 	[Col End]
 *	[Page End]
 * ...
 *
 * Note: This function isn't concerned with populating all cells;
 * only those which it must write into defaults.conf. Normally, when
 * writing to a config file, all cells should be recorded
*/
int create_def_config(dimensions *dims) {
	int i;
	FILE *fp = NULL;		
	char fpath[256] = { 0 }; /* Hard path limit of 256 characters */
	page def_layout;

	strcat(fpath, TEMP_DIR);
	strcat(fpath, DEF_CONF);
	strcat(fpath, ".conf");
	
	fp = fopen(fpath, "w");
	if (fp == NULL) {
		fprintf(stderr, "Error: Could not open file %s for write: %s", fpath, strerror(errno));
		nuget_perror(__FILE__, __FUNCTION__, __LINE__);
		fclose(fp);
		return NUGET_ERR;
	}

	/* Allocate Memory */

 	/* First column */
	for (i = 0; i < DEF_ROWS; i++) {
		def_layout.page_cells[0][i].data = malloc(sizeof(char) * (MAX_DATA + 1));
		if (def_layout.page_cells[0][i].data == NULL) {
			fprintf(stderr, "Error: Could not allocate memory for cell data: %s", strerror(errno));
			nuget_perror(__FILE__, __FUNCTION__, __LINE__);
		   page_cleanup(&def_layout);
			fclose(fp);
			return NUGET_ERR;
		}
	}
	/* Top row */
	for (i = 1; i < DEF_COLS; i++) {
		def_layout.page_cells[i][0].data = malloc(sizeof(char) * (MAX_DATA + 1));
		if (def_layout.page_cells[i][0].data == NULL) {
			fprintf(stderr, "Error: Could not allocate memory for cell data: %s", strerror(errno));
			nuget_perror(__FILE__, __FUNCTION__, __LINE__);
			page_cleanup(&def_layout);
			fclose(fp);
			return NUGET_ERR;
		}
	}

	/* Ensure that there are an appropriate amount of entries */
	assert(sizeof(first_col) == sizeof(char*) * DEF_ROWS);

	/* Define first column data */
	strcpy(def_layout.page_cells[0][0].data , first_col[0]);
	strcpy(def_layout.page_cells[0][1].data , first_col[1]);
	strcpy(def_layout.page_cells[0][2].data , first_col[2]);
	strcpy(def_layout.page_cells[0][3].data , first_col[3]);
	strcpy(def_layout.page_cells[0][4].data , first_col[4]);
	strcpy(def_layout.page_cells[0][5].data , first_col[5]);
	strcpy(def_layout.page_cells[0][6].data , first_col[6]);
	strcpy(def_layout.page_cells[0][7].data , first_col[7]);
	strcpy(def_layout.page_cells[0][8].data , first_col[8]);
	strcpy(def_layout.page_cells[0][9].data , first_col[9]);
	strcpy(def_layout.page_cells[0][10].data, first_col[10]);
	strcpy(def_layout.page_cells[0][11].data, first_col[11]);
	strcpy(def_layout.page_cells[0][12].data, first_col[12]);
	strcpy(def_layout.page_cells[0][13].data, first_col[13]);
	strcpy(def_layout.page_cells[0][14].data, first_col[14]);

	/* Ensure that there are an appropriate amount of entries */
	assert(sizeof(top_row) == sizeof(char*) * (DEF_COLS - 1));

	/* Define top row data */
	strcpy(def_layout.page_cells[1][0].data , top_row[0]);
	strcpy(def_layout.page_cells[2][0].data , top_row[1]);
	strcpy(def_layout.page_cells[3][0].data , top_row[2]);
	strcpy(def_layout.page_cells[4][0].data , top_row[3]);
	strcpy(def_layout.page_cells[5][0].data , top_row[4]);
	strcpy(def_layout.page_cells[6][0].data , top_row[5]);
	strcpy(def_layout.page_cells[7][0].data , top_row[6]);
	strcpy(def_layout.page_cells[8][0].data , top_row[7]);
	strcpy(def_layout.page_cells[9][0].data , top_row[8]);
	strcpy(def_layout.page_cells[10][0].data, top_row[9]);
	strcpy(def_layout.page_cells[11][0].data, top_row[10]);
	strcpy(def_layout.page_cells[12][0].data, top_row[11]);
	strcpy(def_layout.page_cells[13][0].data, top_row[12]);
	strcpy(def_layout.page_cells[14][0].data, top_row[13]);
	strcpy(def_layout.page_cells[15][0].data, top_row[14]);
	strcpy(def_layout.page_cells[16][0].data, top_row[15]);
	strcpy(def_layout.page_cells[17][0].data, top_row[16]);
	strcpy(def_layout.page_cells[18][0].data, top_row[17]);
	strcpy(def_layout.page_cells[19][0].data, top_row[18]);
	strcpy(def_layout.page_cells[20][0].data, top_row[19]);
	strcpy(def_layout.page_cells[21][0].data, top_row[20]);
	strcpy(def_layout.page_cells[22][0].data, top_row[21]);
	strcpy(def_layout.page_cells[23][0].data, top_row[22]);
	strcpy(def_layout.page_cells[24][0].data, top_row[23]);

	/* Write first column to defaults.conf */
	fwrite(PG_START, strlen(PG_START), 1, fp);
	fputc('\n', fp);
	fputc('\t', fp);
	fwrite(COL_START, strlen(COL_START), 1, fp);
	fputc('\n', fp);
	for (i = 0; i < DEF_ROWS; i++) {
		fputc('\t', fp);
		fputc('\t', fp);
		if (def_layout.page_cells[0][i].data == NULL)
			fputc(NUL_ENTRY, fp);
		else
			fwrite(def_layout.page_cells[0][i].data, strlen(def_layout.page_cells[0][i].data), 1, fp);
		fputc('\n', fp);
	}
	fputc('\t', fp);
	fputc('\t', fp);
	fwrite(COL_END, strlen(COL_END), 1, fp);
	fputc('\n', fp);

	/* Write top row to defaults.conf */
	for (i = 1; i < DEF_COLS; i++) {
		fputc('\t', fp);
		fwrite(COL_START, strlen(COL_START), 1, fp);
		fputc('\n', fp);
		fputc('\t', fp);
		fputc('\t', fp);
		if (def_layout.page_cells[i][0].data == NULL)
			fputc(NUL_ENTRY, fp);
		else
			fwrite(def_layout.page_cells[i][0].data, strlen(def_layout.page_cells[i][0].data), 1, fp);
		fputc('\n', fp);
		fputc('\t', fp);
		fwrite(COL_END, strlen(COL_END), 1, fp);
		fputc('\n', fp);
	}
	fwrite(PG_END, strlen(PG_END), 1, fp);
	fputc('\n', fp);

	/*TODO: Figure out why this function segfaults*/
	/*page_cleanup(&def_layout);*/
	fclose(fp);
	return 0;
}

int create_config(dimensions *dims, char *year) {
	printf("I dont do anything yet");
	return 0;
}

int load_config(page *page_p, dimensions *dims, char *year) {
	unsigned x = 0, y = 0;
	int pos, err, c;
	FILE *fp = NULL;
	char fpath[256] = { 0 };
	char line[MAX_DATA];

	strcat(fpath, TEMP_DIR);
	/* Decide whether to use default.conf or year.conf */
	if (check_existing(year)) {
		strcat(fpath, year);
	}
	else if(check_existing(DEF_CONF)) {
		strcat(fpath, DEF_CONF);			
	}
	/* defaults.conf got deleted. Re-create it */
	else {
		create_def_config(dims);
		strcat(fpath, DEF_CONF);
	}
	strcat(fpath, ".conf");
	
	fp = fopen(fpath, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error: Could not open file %s for read: %s", fpath, strerror(errno));
		nuget_perror(__FILE__, __FUNCTION__, __LINE__);
		fclose(fp);
		return NUGET_ERR;
	}

	err = page_init(page_p, dims);
	if (NUGET_ERR == err) {
		fprintf(stderr, "Error: Page failed to initialize\n");
		nuget_perror(__FILE__, __FUNCTION__, __LINE__);
		fclose(fp);
		return NUGET_ERR;
	}

	/* Check for anything else */
	while (1) {
		/* Check for tabs and discard them (and EOF while we're at it) */
		pos = ftell(fp);	
		c = fgetc(fp);
		if (c == EOF) {
			fprintf(stderr, "Error: Did not find PG_END in %s\n", fpath);
			nuget_perror(__FILE__, __FUNCTION__, __LINE__);
			fclose(fp);
			return NUGET_ERR;
		}
		else if (c == '\t') {
			continue;
		}
		else {
			fseek(fp, pos, SEEK_SET);
		}
	
		fgets(line, MAX_DATA, fp);
		line[strcspn(line, "\n")] = '\0';	
		
		if (strcmp(line, PG_START) == 0) {
			x = 0, y = 0;
		}	
		else if (strcmp(line, COL_START) == 0) {
			y = 0;	
		}
		else if (strcmp(line, COL_END) == 0) {
			x++;
		}
		else if (strcmp(line, PG_END) == 0) {
			break;
		}
		/* Data */
		else {
			strcpy(page_p->page_cells[x][y].data, line);
			y++;
		}
	}
	
	fclose(fp);
	return 0;
}

bool check_existing(char *fname) {
	char file[20] = { 0 };
	struct stat stat_buf;

	strcat(file, fname);
	strcat(file, ".conf");

	if (stat(file, &stat_buf) == 0)
       return true;
   else  
       return false;
}

int page_init(page *page_p, dimensions *dims) {
	int x, y;	

	page_p->col_offset = 0;
	page_p->row_offset = 0;

	for (x = 0; x < MAX_OFSCR_COLS; x++) {
		for (y = 0; y < MAX_OFSCR_ROWS; y++) {
			page_p->page_cells[x][y].data = malloc(sizeof(char) * (MAX_DATA + 1));
			if (page_p->page_cells[x][y].data == NULL) {
				fprintf(stderr, "Error: Could not allocate memory for cell data: %s", strerror(errno));
				nuget_perror(__FILE__, __FUNCTION__, __LINE__);
				page_cleanup(page_p);
				return NUGET_ERR;
			}
			page_p->page_cells[x][y].size = dims->cell_width;
		}
	}
	
	return 0;
}

int page_cleanup(page *page_p) {
	int x, y;
	for (x = 0; x < MAX_OFSCR_COLS; x++) {
		for (y = 0; y < MAX_OFSCR_ROWS; y++) {
			if (page_p->page_cells[x][y].data) {
				free(page_p->page_cells[x][y].data);
			}
		}
	}

	return 0;
}

void print_onscr_conf(page pg, dimensions *dims) {
	char c;
	const char* elipse = "...";
	int x, y, i;

	for (x = 0; x < dims->onscr_cols; x++) {
		for (y = 0; y < dims->onscr_rows; y++) {
			i = 0;
			while ((c = pg.page_cells[x][y].data[i]) != '\0' 
				   && i != pg.page_cells[x][y].size - strlen(elipse)) {
				putchar(c);
				i++;
			}	
			printf("%s  ", elipse);
		}
		putchar('\n');
	}
}

void redraw(page pg, dimensions *dims, char *year) {
   char c;
   const char* elipse = "...";
	unsigned col, row, cell_width, cell_height, i;
	int x, y, h = 2;

	cell_width  = dims->cell_width;
	cell_height = dims->cell_height;

	/* TODO: Clear content window as well */
	clear();
	/* Print year */
	mvprintw(0, (dims->win_width/2) - (strlen(year)/2), year);

   for (x = 0, col = pg.col_offset; x < dims->onscr_cols; x += cell_width, col++) {
      for (y = (cell_height * h), row = pg.row_offset; y < (dims->onscr_rows - h); y += cell_height, row++) {
			/* Due to how the config file is loaded, some entries may be empty
				within our array. If empty entry is reached, skip to next col */
			/* TODO: Fix this
			if (strlen(pg.page_cells[col][row].data) == 0) {
				break; 
			} */
			if (y == (DEF_ROWS * cell_height) + (cell_height * h))
				break;
			else {
				i = 0;
				while ((c = pg.page_cells[col][row].data[i]) != '\0'
						&& i != pg.page_cells[col][row].size - strlen(elipse)) {
					mvaddch(y, x + i, c);
					i++;
				}
				if (strlen(pg.page_cells[col][row].data) >= cell_width - strlen(elipse))
					printw("%s", elipse);
			}
		}
   }

	/* Apply attributes */

	/* First column attrs */
	for (x = pg.row_offset, y = pg.row_offset + (cell_height * h); y < dims->onscr_rows; y += cell_height) 
		mvchgat(y, x, cell_width, A_BOLD, 2, NULL);	
	/* Top row attrs*/
	for (x = pg.row_offset + cell_width, y = cell_height; x < dims->onscr_cols; x += cell_width) 
		mvchgat(y, x, cell_width, A_BOLD, 2, NULL);	

	refresh();
}
