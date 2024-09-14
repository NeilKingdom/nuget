#ifndef HELPERS_H
#define HELPERS_H

#include "common.h"
#include "ncsv.h"
#include "table.h"

/* Forward function declarations */

char *n_itoa(const uint64_t n);
uint64_t n_atoi(const char* const a);
char *n_dtoa(const double d);
double n_atod(const char* const a);
Color_t hex_to_rgb(const char* const hex);
void quit_nuget(TableCtx_t *table, ncsv_t *csv, int status);

#endif /* HELPERS_H */
