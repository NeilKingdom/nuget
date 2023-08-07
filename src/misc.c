/**
 * @file misc.c
 * Contains miscellaneous functions that don't belong in dedicated source files.
 *
 * @author Neil Kingdom
 * @version 1.0
 * @since
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <time.h>

#include "../include/misc.h"
#include "../include/nuget.h"

/**
 *
 * @returns Returns the current year.
 */
int get_year(void) {
	time_t t = time(NULL);
	struct tm tlocal = *localtime(&t);
	return (tlocal.tm_year + 1900);
}

/**
 * Convert integer to asciiz string.
 *
 * @param[in] value The integer value to be converted
 * @param[in] str A char pointer set to NULL
 * @param[in] len The length of the string in bytes
 */
char *nuget_itoa(int n, size_t len) {
	int i = 0;
   char *str = NULL;

   str = malloc(sizeof(*str) * (len + 1));
   if (str == NULL) {
      fprintf(stderr, "Failed to allocate memory: %s\n", strerror(errno));
      exit(NUGET_ERR);
   }

   /* TODO: Could do error checking to ensure len is correct by decrementing len */
	while (n != 0) {
		*str++ = (char)((n % 10) + '0');
		n /= 10;
	}
	*str = '\0';

	return str;
}

/**
 * Custom implementation of perror. Prints the file, function, and line
 * of the error.
 *
 * @param[in] file You ought to pass in __FILE__ here
 * @param[in] func You ought to pass in __func__ here
 * @param[in] line You ought to pass in __LINE__ here
 */
void nuget_perror(const char *file, const char *func, int line) {
	fprintf(stderr, "Traceback - File: %s, Function: %s, Line: %d\n\n", file, func, line);
}

