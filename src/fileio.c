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
int create_defaults(void) {
	int err, i;
	size_t row_len, col_len;
	FILE *fp = NULL;		
	char fpath[50];
	const char *rc = "[Row Content]\n";
	const char *cc = "[Col Content]\n";
	const char *pc = "[Page Content]\n";

	strcat(fpath, TEMP_DIR);
	strcat(fpath, "/");
	strcat(fpath, DEF_CONF);

	/* Remove file to start with a blank slate */
	err = remove(fpath);
	if (-1 == err) {
		fprintf(stderr, "Failed to delete %s: %s", DEF_CONF, strerror(err));
		return NUGET_ERR;
	}
	
	fp = fopen(fpath, "w");
	if (fp == NULL) {
		perror("Could not open file for write");
		return NUGET_ERR;
	}

	/* TODO: Perhaps best to test this */
	row_len = sizeof(def_row)/sizeof(def_row[0]);
	col_len = sizeof(def_col)/sizeof(def_col[0]);

	fwrite(rc, sizeof(rc), 1, fp);
	for (i = 0; i < row_len, i++) {
		fwrite(def_col[i], sizeof(def_col[i]), 1, fp);
		fputc('\n', fp);
	}

	fwrite(cc, sizeof(cc), 1, fp);
	for (i = 0; i < col_len, i++) {
		fwrite(def_row[i], sizeof(def_row[i]), 1, fp);
		fputc('\n', fp);
	}

	return 0;
}

int load_config(page *page_p, char *year) {
	char c;
	int i;
	long in, out;
	unsigned x, y;
	FILE *fp = NULL;
	char fname[20];
	char *csection = NULL;
			
	if (check_existing(year) == true) {
		strcat(fname, year);
		strcat(fname, ".conf");	
	}
	else {
		strcat(fname, DEF_CONF);
	}

	fp = fopen(fname, "r");	
	if (fp == NULL) {
		perror("Could not open file for read");
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
				/* TODO: Test length of out - in */
				csection = malloc(out - in);
				fseek(fp, in, SEEK_SET);
				fread(csection, sizeof(csection) - 1, 1, fp);
				fseek(fp, out, SEEK_SET);
	
				if (strcmp(csection, "Row Content") == 0) { printf("Row Content\n"); }
				else if (strcmp(csection, "Col Content") == 0) { printf("Col Content\n"); }
				else if (strcmp(csection, "Page Content") == 0) { printf("Page Content\n"); }
					
				break;
		}	
	}
}

bool check_existing(char *year) {
	char fname[20];
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

void write_to_screen(page *page_p, dimensions dims) {
	
}

char *get_year(void) {
	char *year;
	time_t t = time(NULL);
	struct tm tlocal = *localtime(&t);

	year = itoa((tlocal.tm_year + 1900), year, 10);	
	assert(strcmp(year, "2022") == 0);

	return year;
}

