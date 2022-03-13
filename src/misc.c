#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "misc.h"
#include "nuget.h"

int get_year(void) {
	time_t t = time(NULL);
	struct tm tlocal = *localtime(&t);
	return tlocal.tm_year + 1900;
}

int nuget_itoa(int value, char *str, int base, size_t len) {
	int i = 0;

	if(str == NULL) {
		fprintf(stderr, "Error: Passed uninitialized string to nuget_itoa\n");
		return NUGET_ERR;		
	}

	while(value != 0) {
		str[len - i - 1] = (char)((value % base) + '0');
		value /= base;
		i++;
	}	
	str[len] = '\0';

	return 0;
}

void nuget_perror(const char *file, const char *func, int line) {
	fprintf(stderr, "Traceback - File: %s, Function: %s, Line: %d\n\n", file, func, line);
}

