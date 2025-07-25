#include "ncsv.h"

static Point_t top_left = { 0 };

/**
 * @brief Callback invoked by csv_parse() when a field has been read.
 * @since 09-09-2024
 * @param field A buffer containing the field data that was read. It will not be NULL terminated unless
 * csv_init() was invoked with the CSV_APPEND_NULL flag.
 * @param field_len The length in bytes of field
 * @param data Optional data which is received from the last argument of csv_parse()
 */
static void cb1(void *field, size_t field_len, void *data) {
    TableCtx_t *table = (TableCtx_t*)data;

    set_cell_value(table, field, top_left);
    top_left.x++;
}

/**
 * @brief Callback invoked by csv_parse() when a record has been read.
 * @since 09-09-2024
 * @param eol The terminating character of the record as an unsigned char or -1 if csv_fini() was invoked
 * @param data Optional data which is received from the last argument of csv_parse()
 */
static void cb2(int eol, void *data) {
    TableCtx_t *table = (TableCtx_t*)data;
    /* TODO: validation on eol? */

    top_left.x = table->table_offset.x;
    top_left.y++;
}

/**
 * @brief Reads data from file and populates the table with it.
 * @since 09-09-2024
 * @param table A reference to the table context object
 * @param csv_ctx A reference to the CSV context object
 * @param file Absolute path to the CSV file that shall be parsed
 */
void read_csv_data(TableCtx_t *table, ncsv_t *csv_ctx, const char* const file) {
    size_t len, nb_read;
    void *buf = NULL;
    FILE *fp = NULL;
    struct stat info;

    fp = fopen(file, "r");
    if (fp == NULL) {
        perror("Failed to open CSV file for read");
        exit(EXIT_FAILURE);
    }

    /* TODO: Validate that it is a CSV file (mimetype?) */

    fstat(fileno(fp), &info);
    len = (size_t)info.st_size;
    if (len == 0) {
        fclose(fp);
        return;
    }

    buf = malloc(len * sizeof(char));
    if (buf == NULL) {
        perror("Failed to allocate space for temporary buffer");
        csv_free(csv_ctx);
        fclose(fp);
        exit(EXIT_FAILURE);
    }
    fread(buf, len, 1, fp);

    top_left = table->table_offset;
    nb_read = csv_parse(csv_ctx, buf, len, cb1, cb2, (void*)table);
    if (nb_read != len) {
        csv_error(csv_ctx);
        csv_free(csv_ctx);
        free(buf);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    free(buf);
    fclose(fp);
}

void write_csv_data(TableCtx_t *table, ncsv_t *csv_ctx, const char* const file) {
    /**
     * TODO: write_csv_data ought to be a wrapper around csv_write() which writes
     * individual fields to the output file. A structure monitors the right-most
     * and bottom-most cells that contain data within the table. This determines
     * the area/range that we need to loop through, rather than writing the
     * entire table, which wastes time and disk space.
     */
}
