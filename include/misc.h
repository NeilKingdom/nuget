#ifndef MISC_H
#define MISC_H

#include <stdlib.h>

/* Functions */
int   get_year(void);
char *nuget_itoa(int n, size_t len);
void  nuget_perror(const char *file, const char *func, int line);

#endif
