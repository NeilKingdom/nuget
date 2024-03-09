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
#include <ctype.h>
#include <ncurses.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_t;

typedef struct {
    uint64_t x;
    uint64_t y;
} point_t;

typedef enum CardinalDirection {
    UP,
    DOWN,
    LEFT,
    RIGHT
} direction_t;

#endif /* COMMON_H */
