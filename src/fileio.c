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
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>

#include "fileio.h"
#include "nuget.h"

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
int create_def_config(dimensions sdims) {
	int i;
	FILE *fp = NULL;		
	char fpath[256] = { 0 }; /* Hard path limit of 256 characters */
	page def_layout;

	strcat(fpath, TEMP_DIR);
	strcat(fpath, "/");
	strcat(fpath, DEF_CONF);
	
	fp = fopen(fpath, "w");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file defaults.conf for write: %s", strerror(errno));
		fclose(fp);
		return NUGET_ERR;
	}

	/* Allocate Memory */
	/* TODO: Error handling */
	for (i = 0; i < DEF_COLS; i++) /* First column */
		def_layout.col_data[0].cell_data[i].data = malloc(sizeof(char) * (MAX_DATA + 1));
	for(i = 1; i < DEF_ROWS; i++) /* Top row */ 
		def_layout.col_data[i].cell_data[0].data = malloc(sizeof(char) * (MAX_DATA + 1));

	/* Define Column Data */
	def_layout.col_data[0].cell_data[0].data = "Profits";
	def_layout.col_data[0].cell_data[1].data = "";
	def_layout.col_data[0].cell_data[2].data = "Work"; 
	def_layout.col_data[0].cell_data[3].data = "Gifts"; 
	def_layout.col_data[0].cell_data[4].data = "Other"; 
	def_layout.col_data[0].cell_data[5].data = "";
	def_layout.col_data[0].cell_data[6].data = "Expenses";
	def_layout.col_data[0].cell_data[7].data = "";
	def_layout.col_data[0].cell_data[8].data = "Food";
	def_layout.col_data[0].cell_data[9].data = "Gas";
	def_layout.col_data[0].cell_data[10].data = "Hair Cuts";
	def_layout.col_data[0].cell_data[11].data = "Clothing";
	def_layout.col_data[0].cell_data[12].data = "Personal";
	def_layout.col_data[0].cell_data[13].data = "Other";
	def_layout.col_data[0].cell_data[14].data = "Other";

	def_layout.col_data[1].cell_data[0].data = "Jan Est";
	def_layout.col_data[2].cell_data[0].data = "Jan Act";
	def_layout.col_data[3].cell_data[0].data = "Feb Est";
	def_layout.col_data[4].cell_data[0].data = "Feb Act";
	def_layout.col_data[5].cell_data[0].data = "Mar Est";
	def_layout.col_data[6].cell_data[0].data = "Mar Act";
	def_layout.col_data[7].cell_data[0].data = "Apr Est";
	def_layout.col_data[8].cell_data[0].data = "Apr Act";
	def_layout.col_data[9].cell_data[0].data = "May Est";
	def_layout.col_data[10].cell_data[0].data = "May Act";
	def_layout.col_data[11].cell_data[0].data = "Jun Est";
	def_layout.col_data[12].cell_data[0].data = "Jun Act";
	def_layout.col_data[13].cell_data[0].data = "Jul Est";
	def_layout.col_data[14].cell_data[0].data = "Jul Act";
	def_layout.col_data[15].cell_data[0].data = "Aug Est";
	def_layout.col_data[16].cell_data[0].data = "Aug Act";
	def_layout.col_data[17].cell_data[0].data = "Sep Est";
	def_layout.col_data[18].cell_data[0].data = "Sep Act";
	def_layout.col_data[19].cell_data[0].data = "Oct Est";
	def_layout.col_data[20].cell_data[0].data = "Oct Act";
	def_layout.col_data[21].cell_data[0].data = "Nov Est";
	def_layout.col_data[22].cell_data[0].data = "Nov Act";
	def_layout.col_data[23].cell_data[0].data = "Dec Est";
	def_layout.col_data[24].cell_data[0].data = "Dec Act";

	/* Write first column */
	fwrite(PG_START, (strlen(PG_START) + 1), 1, fp);

	fputc('\n', fp);
	fputc('\t', fp);
	fwrite(COL_START, (strlen(COL_START) + 1), 1, fp);
	fputc('\n', fp);
	for (i = 0; i < DEF_ROWS; i++) {
		fputc('\t', fp);
		fputc('\t', fp);
		fwrite(def_layout.col_data[0].cell_data[i].data, sizeof(def_layout.col_data[0].cell_data[i].data), 1, fp);
		fputc('\n', fp);
	}
	fputc('\t', fp);
	fputc('\t', fp);
	fwrite(COL_END, (strlen(COL_END) + 1), 1, fp);
	fputc('\n', fp);

	/* Write top row */
	for (i = 1; i < DEF_COLS; i++) {
		fputc('\t', fp);
		fwrite(COL_START, (strlen(COL_START) + 1), 1, fp);
		fputc('\t', fp);
		fputc('\t', fp);
		fwrite(def_layout.col_data[i].cell_data[0].data, sizeof(def_layout.col_data[i].cell_data[0].data), 1, fp);
		fputc('\n', fp);
		fputc('\t', fp);
		fwrite(COL_END, (strlen(COL_START) + 1), 1, fp);
		fputc('\n', fp);
	}

	fwrite(PG_END, (strlen(PG_END) + 1), 1, fp);
	fclose(fp);
	return 0;
}

int create_config(page *page_p, dimensions sdims, char *year) {
	printf("I dont do anything yet");
	return 0;
}

int load_config(page *page_p, dimensions sdims, char *year) {
	FILE *fp = NULL;
	/*char fpath[256];*/

	fclose(fp);
	return 0;
}

bool check_existing(char *year) {
	char fname[10] = { 0 };
	struct stat stat_buf;
   int exists;

	strcat(fname, year);
	strcat(fname, ".conf");

	exists = stat(fname, &stat_buf);
   if(exists == 0)
       return true;
   else  
       return false;
}

int get_year(void) {
	time_t t = time(NULL);
	struct tm tlocal = *localtime(&t);
	return tlocal.tm_year + 1900;
}

