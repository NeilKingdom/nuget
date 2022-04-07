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

#include "common.h"
#include "nuget.h"
#include "misc.h"

/*
 * Layout of the default file:
 * 
 * [Page Start]
 * 	[Col Start]
 *			data data data
 *		[Col End]
 *    [Col Start]
 *			data  data
 * 	[Col End]
 *	[Page End]/0
 * ...
 *
 * Note: This function isn't concerned with populating all cells;
 * only those which it must write into default.conf. Normally, when
 * writing to a config file, all cells should be recorded
*/
int create_def_config(void) 
{
   int top_row_size, first_col_size, i;
	FILE *fp = NULL;		
   /* Path character limit */
	char fpath[PATH_LIM] = { 0 };
	page def_layout;

	strcat(fpath, TEMP_DIR);
	strcat(fpath, DEF_CONF);
	strcat(fpath, ".conf");
	
	fp = fopen(fpath, "w");
	if (fp == NULL) 
   {
		fprintf(stderr, "Error: Could not open file %s for write: %s", fpath, strerror(errno));
		nuget_perror(__FILE__, __FUNCTION__, __LINE__);
		fclose(fp);
		return NUGET_ERR;
	}

	/**** Allocate Memory (could use page_init(), but we only need to allocate top row and first column) ****/

   /* first_col size and DEF_COLS should match */
   assert(sizeof(first_col) == sizeof(char*) * DEF_ROWS);

 	/* First column */
	for (i = 0; i < DEF_ROWS; i++) 
   {
		def_layout.page_cells[0][i] = calloc(1, sizeof(char) * (MAX_DATA + 1));
		if (def_layout.page_cells[0][i] == NULL) 
      {
			fprintf(stderr, "Error: Could not allocate memory for cell data: %s", strerror(errno));
			nuget_perror(__FILE__, __FUNCTION__, __LINE__);
		   page_cleanup(&def_layout);
			fclose(fp);
			return NUGET_ERR;
		}
	}

   /* top_row size and DEF_ROWS should match */
   assert(sizeof(top_row) == sizeof(char*) * (DEF_COLS - 1));

	/* Top row */
	for (i = 1; i < DEF_COLS; i++) 
   {
		def_layout.page_cells[i][0] = calloc(1, sizeof(char) * (MAX_DATA + 1));
		if (def_layout.page_cells[i][0] == NULL) 
      {
			fprintf(stderr, "Error: Could not allocate memory for cell data: %s", strerror(errno));
			nuget_perror(__FILE__, __FUNCTION__, __LINE__);
			page_cleanup(&def_layout);
			fclose(fp);
			return NUGET_ERR;
		}
	}

	/* Define first column data */
   first_col_size = sizeof(first_col) / sizeof(first_col[0]);
   for (i = 0; i < first_col_size; i++)
	   strcpy(def_layout.page_cells[0][i] , first_col[i]);
   
	/* Define top row data */
   top_row_size = (sizeof(top_row) / sizeof(top_row[0]));
   for (i = 1; i < top_row_size; i++)  
	   strcpy(def_layout.page_cells[i][0] , top_row[i-1]);

	/* Write first column to default.conf */
	fwrite(PG_START, strlen(PG_START), 1, fp);
	fputc('\n', fp);
	fputc('\t', fp);
	fwrite(COL_START, strlen(COL_START), 1, fp);
	fputc('\n', fp);
   fputc('\t', fp);
   fputc('\t', fp);

	for (i = 0; i < DEF_ROWS; i++) 
   {
		if (def_layout.page_cells[0][i] == 0) 
      {
			fputc(NUL_ENTRY, fp);
      }
		else
      {
			fwrite(def_layout.page_cells[0][i], strlen(def_layout.page_cells[0][i]), 1, fp);
      }
      fputc('\n', fp);
	}

	fputc('\t', fp);
	fputc('\t', fp);
	fwrite(COL_END, strlen(COL_END), 1, fp);
	fputc('\n', fp);

	/* Write top row to default.conf */
	for (i = 1; i < DEF_COLS; i++) 
   {
		fputc('\t', fp);
		fwrite(COL_START, strlen(COL_START), 1, fp);
		fputc('\n', fp);
		fputc('\t', fp);
		fputc('\t', fp);

		if (def_layout.page_cells[i][0] == 0)
      {
			fputc(NUL_ENTRY, fp);
      }
		else
      {
			fwrite(def_layout.page_cells[i][0], strlen(def_layout.page_cells[i][0]), 1, fp);
      }

		fputc('\n', fp);
		fputc('\t', fp);
		fwrite(COL_END, strlen(COL_END), 1, fp);
		fputc('\n', fp);
	}
	fwrite(PG_END, strlen(PG_END), 1, fp);
   fputc('\n', fp);
	fputc(EOF, fp);

   /* Data has now been loaded into config file, so free allocated memory */
   /* TODO: Doesn't appear to crash anymore, but should add test cases */
	page_cleanup(&def_layout);
	fclose(fp);

	return 0;
}

/* TODO: implement */
int create_config(dimensions *dims_p, char *year) 
{
	printf("I dont do anything yet %s %ld", year, dims_p->onscr_cols);
	return 0;
}

int load_config(page *page_p, char *year) 
{
	unsigned x = 0, y = 0;
	int pos, err, c;
	FILE *fp = NULL;
	char fpath[PATH_LIM] = { 0 };
	char line[MAX_DATA];

	strcat(fpath, TEMP_DIR);
	/* Decide whether to use default.conf or year.conf */
	if (check_existing(year)) 
   {
		strcat(fpath, year);
	}
	else if(check_existing(DEF_CONF)) 
   {
		strcat(fpath, DEF_CONF);			
	}
	/* default.conf got deleted : re-create it */
	else 
   {
		create_def_config();
		strcat(fpath, DEF_CONF);
	}
	strcat(fpath, ".conf");
	
	fp = fopen(fpath, "r");
	if (fp == NULL) 
   {
		fprintf(stderr, "Error: Could not open file %s for read: %s", fpath, strerror(errno));
		nuget_perror(__FILE__, __FUNCTION__, __LINE__);
		fclose(fp);
		return NUGET_ERR;
	}

	err = page_init(page_p);
	if (NUGET_ERR == err) 
   {
		fprintf(stderr, "Error: Page failed to initialize\n");
		nuget_perror(__FILE__, __FUNCTION__, __LINE__);
		fclose(fp);
		return NUGET_ERR;
	}

	/* Check for anything else */
	while (1) 
   {
		/* Check for tabs and discard them (and EOF while we're at it) */
		pos = ftell(fp);	
		c = fgetc(fp);
		if (c == EOF) 
      {
			fprintf(stderr, "Error: Did not find PG_END in %s\n", fpath);
			nuget_perror(__FILE__, __FUNCTION__, __LINE__);
			fclose(fp);
			return NUGET_ERR;
		}
		else if (c == '\t') 
      {
			continue;
		}
		else 
      {
			fseek(fp, pos, SEEK_SET);
		}
	
      /* Reading text */
		fgets(line, MAX_DATA, fp);
      /* Replace newline feed with nul terminator */
      line[strcspn(line, "\n")] = '\0'; 

		if (strcmp(line, PG_START) == 0) 
      {
			x = 0, y = 0;
		}	
		else if (strcmp(line, COL_START) == 0) 
      {
			y = 0;	
		}
		else if (strcmp(line, COL_END) == 0) 
      {
			x++;
		}
		else if (strcmp(line, PG_END) == 0) 
      {
			break;
		}
		/* Data */
		else {
         strncpy(page_p->page_cells[x][y], line, cell_size);
         y++;
      }
   }
	fclose(fp);

	return 0;
}

bool check_existing(char *fname) 
{
	char file[20] = { 0 };
	struct stat stat_buf;

	strcat(file, fname);
	strcat(file, ".conf");

	if (stat(file, &stat_buf) == 0)
       return true;
   else  
       return false;
}

int page_init(page *page_p) 
{
	int x, y;	

	page_p->col_offset = 0;
	page_p->row_offset = 0;

	for (x = 0; x < MAX_OFSCR_COLS; x++) {
		for (y = 0; y < MAX_OFSCR_ROWS; y++) {
			page_p->page_cells[x][y] = calloc(1, sizeof(char) * (MAX_DATA + 1));
			if (page_p->page_cells[x][y] == NULL) {
				fprintf(stderr, "Error: Could not allocate memory for cell data: %s", strerror(errno));
				nuget_perror(__FILE__, __FUNCTION__, __LINE__);
				page_cleanup(page_p);
				return NUGET_ERR;
			}
		}
	}
	
	return 0;
}

void redraw(page *page_p, dimensions *dims_p, char *year) 
{
   char c;
   const char* elipses = "...";
	unsigned col, row, cell_width, cell_height;
	size_t x, y, i;
   /* TODO: Should either store or calculate this */
   const size_t h = 2; /* The gap before rows begin */

	cell_width  = dims_p->cell_width;
	cell_height = dims_p->cell_height;

	/* TODO: Clear content window as well */
	clear();

	/* Print year */
	mvprintw(0, (dims_p->win_width/2) - (strlen(year)/2), year);

   /* Print data contents of cells */
   for (x = 0, col = page_p->col_offset; x < dims_p->onscr_cols; x += cell_width, col++) 
   {
      for (y = (cell_height * (int)h), row = page_p->row_offset; y < (dims_p->onscr_rows - h); y += cell_height, row++) 
      {
         i = 0;
			while (i < cell_size - strlen(elipses)) 
         {
            if ((c = *(page_p->page_cells[x][y] + i)) == (char)0)
               break;
            mvaddch(y, x + i, c);
				i++;
			}	
         printw("%s", elipses);
		}
   }

	/* Apply attributes */

	/* First column attrs */
	for (x = page_p->row_offset, y = page_p->row_offset + (cell_height * h); y < dims_p->onscr_rows; y += cell_height) 
		mvchgat(y, x, cell_width, A_BOLD, 2, NULL);	

	/* Top row attrs*/
	for (x = page_p->row_offset + cell_width, y = cell_height; x < dims_p->onscr_cols; x += cell_width) 
		mvchgat(y, x, cell_width, A_BOLD, 2, NULL);	

	refresh();
}

int page_cleanup(page *page_p) 
{
	int x, y;
	for (x = 0; x < MAX_OFSCR_COLS; x++) 
   {
		for (y = 0; y < MAX_OFSCR_ROWS; y++) 
      {
			if (!page_p->page_cells[x][y]) 
         {
				free(page_p->page_cells[x][y]);
			}
		}
	}

	return 0;
}
