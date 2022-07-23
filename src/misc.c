/**
 * @file misc.c
 * Contains miscellaneous functions that don't belong in dedicated source files.
 *
 * **Author:** Neil Kingdom
 * **Version:** 1.0
 * **Since:**
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "misc.h"
#include "nuget.h"

/**
 * Returns the current year.
*/
int get_year(void)
{
	time_t t = time(NULL);
	struct tm tlocal = *localtime(&t);
	return (tlocal.tm_year + 1900);
}

/**
 * Custom implementation of itoa. Turns an int into a string.
 *
 * @param[in] value The integer value to be converted
 * @param[in] str A char pointer set to NULL
 * @param[in] base The base number system e.g. 2, 8, 10, 16
 * @param[in] len The length of the string in bytes
*/
int nuget_itoa(int value, char *str, int base, size_t len)
{
	int i = 0;

   if (base == 0)
      return NUGET_ERR;

	while(value != 0)
   {
		str[len - i - 1] = (char)((value % base) + '0');
		value /= base;
		i++;
	}
	str[len] = '\0';

	return 0;
}

/**
 * Custom implementation of perror. Prints the file, function, and line
 * of the error.
 *
 * @param[in] file You ought to pass in __FILE__ here
 * @param[in] func You ought to pass in __FUNCTION__ here
 * @param[in] line You ought to pass in __LINE__ here
*/
void nuget_perror(const char *file, const char *func, int line)
{
	fprintf(stderr, "Traceback - File: %s, Function: %s, Line: %d\n\n", file, func, line);
}

