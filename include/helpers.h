#ifndef HELPERS_H
#define HELPERS_H

#include "common.h"
#include "ncsv.h"
#include "table.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Forward function declarations

char *n_itoa(const uint64_t n);
uint64_t n_atoi(const char* const a);
char *n_dtoa(const double d);
double n_atod(const char* const a);
RGB888_t hex_to_rgb(const char* const hex);
void quit_nuget(TableCtx_t *table, ncsv_t *csv, int status);

#endif
