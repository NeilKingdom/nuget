#ifndef NCSV_H
#define NCSV_H

#include <csv.h>
#include <sys/stat.h>

#include "common.h"
#include "table.h"

typedef struct csv_parser ncsv_t;

/* Forward function declarations */

void read_csv_data(TableCtx_t *table, ncsv_t *csv_ctx, const char* const file);
void write_csv_data(TableCtx_t *table, ncsv_t *csv_ctx, const char* const file);

#endif /* NCSV_H */
