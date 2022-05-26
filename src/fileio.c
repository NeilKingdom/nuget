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
	page_t def_layout = { 0 };

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

	/* Allocate Memory (could use page_init(), but we only need to allocate top row and first column) */

 	/* First column */
	for (i = 0; i < DEF_ROWS; i++) 
   {
		def_layout.page_cells[0][i] = calloc(1, sizeof(char) * MAX_DATA);
		if (def_layout.page_cells[0][i] == NULL) 
      {
			fprintf(stderr, "Error: Could not allocate memory for cell data: %s", strerror(errno));
			nuget_perror(__FILE__, __FUNCTION__, __LINE__);
		   page_cleanup(&def_layout);
			fclose(fp);
			return NUGET_ERR;
		}
	}

	/* Top row */
	for (i = 1; i < DEF_COLS; i++) 
   {
		def_layout.page_cells[i][0] = calloc(1, sizeof(char) * MAX_DATA);
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
	   strcpy(def_layout.page_cells[0][i], first_col[i]);
   
	/* Define top row data */
   top_row_size = (sizeof(top_row) / sizeof(top_row[0]));
   for (i = 1; i < top_row_size; i++)  
	   strcpy(def_layout.page_cells[i][0], top_row[i-1]);

	/* Write first column to default.conf */
	fwrite(PG_START, strlen(PG_START), 1, fp);
	fputc('\n', fp);
	fputc('\t', fp);
	fwrite(COL_START, strlen(COL_START), 1, fp);

	for (i = 0; i < DEF_ROWS; i++) 
   {
      fputc('\n', fp);
      fputc('\t', fp);
      fputc('\t', fp);
      fputc('\t', fp);
		if (def_layout.page_cells[0][i] == 0) 
      {
			fwrite(NUL_ENTRY, strlen(NUL_ENTRY), 1, fp);
      }
		else
      {
			fwrite(def_layout.page_cells[0][i], strlen(def_layout.page_cells[0][i]), 1, fp);
      }
	}

   fputc('\n', fp);
   fputc('\t', fp);
	fwrite(COL_END, strlen(COL_END), 1, fp);

	/* Write top row to default.conf */
	for (i = 0; i < DEF_COLS; i++) 
   {
      fputc('\n', fp);
      fputc('\t', fp);
	   fwrite(COL_START, strlen(COL_START), 1, fp);
		fputc('\n', fp);
		fputc('\t', fp);
		fputc('\t', fp);
      fputc('\t', fp);

		if (def_layout.page_cells[i][0] == 0)
      {
			fwrite(NUL_ENTRY, strlen(NUL_ENTRY), 1, fp);
      }
		else
      {
			fwrite(def_layout.page_cells[i][0], strlen(def_layout.page_cells[i][0]), 1, fp);
      }

		fputc('\n', fp);
		fputc('\t', fp);
		fwrite(COL_END, strlen(COL_END), 1, fp);
	}

   fputc('\n', fp);
	fwrite(PG_END, strlen(PG_END), 1, fp);

   /* Data has now been loaded into config file, so free allocated memory */
   /* TODO: Doesn't appear to crash anymore, but should add test cases */
	page_cleanup(&def_layout);
	fclose(fp);

	return 0;
}

/* TODO: implement */
int create_config(dimensions_t *dims_p, char *year) 
{
	printf("I dont do anything yet %s %ld", year, dims_p->onscr_cols);
	return 0;
}

int load_config(page_t *page_p, char *year) 
{
	unsigned col = 0, row = 0;
	long pos;
	int err, c;
	FILE *fp = NULL;
	char fpath[PATH_LIM] = { 0 };
	char line[MAX_DATA];

	strcat(fpath, TEMP_DIR);
	/* Decide whether to use default.conf or year.conf */
	if (check_existing(year)) 
   {
		strcat(fpath, year);
	}
	else if (check_existing(DEF_CONF)) 
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

   /* Initialize page */
	err = page_init(page_p);
	if (NUGET_ERR == err) 
   {
		fprintf(stderr, "Error: Page failed to initialize\n");
		nuget_perror(__FILE__, __FUNCTION__, __LINE__);
		fclose(fp);
		return NUGET_ERR;
	}

   /* Load file contents into page */
   while (1) 
   {
      if (feof(fp)) 
      {
         fprintf(stderr, "Error: Did not find PG_END in %s\n", fpath);
         nuget_perror(__FILE__, __FUNCTION__, __LINE__);
         fclose(fp);
         return NUGET_ERR;
      }

      /* Check for tabs and discard them */
      pos = ftell(fp);  
      c = fgetc(fp);
      if (c == '\t') 
      {
         continue;
      }
      else 
      {
         fseek(fp, pos, SEEK_SET);
      }
   
      /* Read in text until newline is encountered or MAX_DATA is reached */
		/* TODO: Handle case where data actually is more than MAX_DATA (discard rest of line and append NULL terminator) */
      fgets(line, MAX_DATA, fp);
      /* Replace newline feed with nul terminator */
      line[strcspn(line, "\n")] = '\0'; 

		if (strcmp(line, PG_START) == 0) 
      {
			col = 0, row = 0;
		}	
		else if (strcmp(line, COL_START) == 0) 
      {
			row = 0;	
		}
		else if (strcmp(line, COL_END) == 0) 
      {
			col++;
		}
		else if (strcmp(line, PG_END) == 0) 
      {
			break;
		}
		/* Data */
		else {
         strncpy((char *)(page_p->page_cells[col][row]), (char *)line, (size_t)(sizeof(char) * MAX_DATA));
         row++;
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

int page_init(page_t *page_p) 
{
	int col = 0, row = 0;	

	page_p->col_offset = 0;
	page_p->row_offset = 0;

	for (col = 0; col < MAX_OFSCR_COLS; col++) 
	{
		for (row = 0; row < MAX_OFSCR_ROWS; row++) 
		{
			page_p->page_cells[col][row] = calloc(1, sizeof(char) * MAX_DATA);
			if (page_p->page_cells[col][row] == NULL) 
			{
				fprintf(stderr, "Error: Could not allocate memory for cell data: %s", strerror(errno));
				nuget_perror(__FILE__, __FUNCTION__, __LINE__);
				page_cleanup(page_p);
				return NUGET_ERR;
			}
		}
	}
	
	return 0;
}

void redraw(page_t *page_p, dimensions_t *dims_p, char *year) 
{
   const char* elipses = "...";
	size_t col, row;
	unsigned x, y, cell_width, cell_height;

	cell_width  = dims_p->cell_width;
	cell_height = dims_p->cell_height;

	/* TODO: Clear content window as well */
	clear();

	/* Print year */
	mvprintw(0, (dims_p->win_width/2) - (strlen(year)/2), year);

   /* Print first col */
	for (row = (TOP_ROW_GAP * cell_height), col = 0; row < dims_p->onscr_rows; row += cell_height)
	{
		x = col / cell_width;
		y = row / cell_height;	
		
		if (page_p->page_cells[x][y])
		{
			mvaddch(row, col, ' '); /* Spacing */
			addnstr(page_p->page_cells[x][y], cell_size - strlen(elipses));
			if (strlen(page_p->page_cells[x][y]) > cell_size - strlen(elipses))
				printw(elipses);
		}
		else 
		{
			continue;
		}
	} 

	/* TODO: Move this into the for loop above */
	/* Apply attributes to first column */
	for (col = 0, row = (TOP_ROW_GAP * cell_height); row < dims_p->onscr_rows; row += cell_height) 
		mvchgat(row, col, cell_width, A_BOLD, 2, NULL);	

	/* Print top row */
	/* TODO: During debugging found that page_p->page_cells[2][0] = "Jan Est", page_cells[3][0] = "Jan Act", etc. */
	for (col = cell_width, row = cell_height; col < dims_p->onscr_cols; col += cell_width)
	{
		x = col / cell_width;
		y = (row - cell_height) / cell_height;
		
		if (page_p->page_cells[x][y])
		{
			mvaddch(row, col, ' '); /* Spacing */
			addnstr(page_p->page_cells[x][y], cell_size - strlen(elipses));
			if (strlen(page_p->page_cells[x][y]) > cell_size - strlen(elipses))
				printw(elipses);
		}
		else 
		{
			continue;
		}
	} 

	/* TODO: Move this into the for loop above */
	/* Apply attributes to top row */
	for (col = cell_width, row = cell_height; col < dims_p->onscr_cols; col += cell_width) 
		mvchgat(row, col, cell_width, A_BOLD, 2, NULL);	

	/* Print cell data accounting for offsets */
   for (col = cell_width; col < dims_p->onscr_cols; col += cell_width)
   {
      for (row = (TOP_ROW_GAP * cell_height); row < dims_p->onscr_rows; row += cell_height)
      { 
			/* TODO: There may be a bug here. Not sure if we need to account for first col and top row */
			x = (page_p->col_offset * (dims_p->onscr_cols)) + (col / cell_width);
			y = (page_p->row_offset * (dims_p->onscr_rows)) + (row / cell_height);

			if (page_p->page_cells[x][y]) 
			{
				mvaddch(row, col, ' '); /* Spacing */
				addnstr(page_p->page_cells[x][y], cell_size - strlen(elipses));
				if (strlen(page_p->page_cells[x][y]) > cell_size - strlen(elipses))
         		printw(elipses);
			}
			else 
			{
				continue;
			}
      }
   }

	refresh();
}

int page_cleanup(page_t *page_p) 
{
	int col, row;
	for (col = 0; col < MAX_OFSCR_COLS; col++) 
   {
		for (row = 0; row < MAX_OFSCR_ROWS; row++) 
      {
			if (!page_p->page_cells[col][row]) 
				free(page_p->page_cells[col][row]);
		}
	}

	return 0;
}
