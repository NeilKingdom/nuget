#ifndef MISC_H
#define MISC_H

/* Functions */
int get_year(void);
int nuget_itoa(int value, char *str, int base, size_t len);
void nuget_perror(const char *file, const char *func, int line);

#endif
