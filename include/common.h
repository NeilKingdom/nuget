#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <ctype.h>
#include <ncurses.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color_t;

typedef struct {
    uint64_t x;
    uint64_t y;
} Point_t;

typedef enum {
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT
} Align_t;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction_t;

char        *dtoa(const double d);
double      atod(const char *a);

#endif /* COMMON_H */
