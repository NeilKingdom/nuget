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
 * [Row Content]
 * Jan Est
 * Jan Act
 * Feb Est
 * Feb Act
 * ...
 * [Col Content] 
 * Profits
 * 
 * Job
 * Other
 * 
 * Expenses
 *
 * Food
 * Gas
 * Haircuts
 * ... 
 * [Page Content]
 * [Col #1]
 * 10.35
 * 
 * 910.23
 * [Col #3]
 * 56.5
 * 344.23
 * [Col #10]
 * 
 * 
 * 12343.35
 * ...
*/
int create_defaults(dimensions sdims) {
	/*int err, i;*/
	FILE *fp = NULL;		
	char fpath[256] = { 0 }; /* Hard path limit of 256 characters */
	/*
	const char *rc = "[Row Content]\n";
	const char *cc = "[Col Content]\n";
	const char *pc = "[Page Content]\n";
	*/

	strcat(fpath, TEMP_DIR);
	strcat(fpath, "/");
	strcat(fpath, DEF_CONF);

	/* This is only necessary if we want to create defaults.conf when it
      already exists, which pressumably isn't the case
	err = remove(fpath);
	if (-1 == err) {
		fprintf(stderr, "Failed to delete %s: %s", DEF_CONF, strerror(errno));
		return NUGET_ERR;
	}
	*/
	
	fp = fopen(fpath, "w");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file defaults.conf for write: %s", strerror(errno));
		fclose(fp);
		return NUGET_ERR;
	}

	/* TODO: Segfaults at the moment
	fwrite(rc, sizeof(rc), 1, fp);
	for (i = 0; i < sdims.onscr_rows; i++) {
		fwrite(def_row[i], sizeof(def_row[i]), 1, fp);
		fputc('\n', fp);
	}

	fwrite(cc, sizeof(cc), 1, fp);
	for (i = 0; i < sdims.onscr_cols; i++) {
		fwrite(def_col[i], sizeof(def_col[i]), 1, fp);
		fputc('\n', fp);
	}
	*/

	fclose(fp);
	return 0;
}

int load_config(page *page_p, dimensions sdims, char *year) {
	char c;
	/*
	int i;
	unsigned x, y;
	*/
	FILE *fp = NULL;
	long in = 0, out = 0;
	char fpath[256] = { 0 };
	char *csection = NULL;

	strcat(fpath, TEMP_DIR);
	strcat(fpath, "/");
			
	if (check_existing(year) == true) {
		strcat(fpath, year);
		strcat(fpath, ".conf");	
	}
	else {
		create_defaults(sdims);
		strcat(fpath, DEF_CONF);
	}

	fp = fopen(fpath, "r");	
	if (fp == NULL) {
		fprintf(stderr, "Could not open file %s for read: %s", fpath, strerror(errno));
		fclose(fp);
		return NUGET_ERR;
	}

	/* Parse contents of config file into page struct */
	while ((c = fgetc(fp)) != EOF) {
		/* Parse content section */
		switch (c) {
			case '[':
				in = ftell(fp);
				break;
			case ']':
				out = ftell(fp);
				csection = malloc(out - in);
				fseek(fp, in, SEEK_SET);
				fread(csection, sizeof(csection) - 1, 1, fp);
				fseek(fp, out, SEEK_SET);
	
				if (strcmp(csection, "Row Content") == 0) { printf("Row Content\n"); }
				else if (strcmp(csection, "Col Content") == 0) { printf("Col Content\n"); }
				else if (strcmp(csection, "Page Content") == 0) { printf("Page Content\n"); }
	
				free(csection);				
				break;
		}	
	}
	fclose(fp);
	return 0;
}

void write_to_screen(page *page_p, dimensions sdims) {
	do {} while(0);
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

