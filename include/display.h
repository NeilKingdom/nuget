#ifndef DISPLAY_H
#define DISPLAY_H

/* -1 mean no color, therefore, the background color wont exist with this attribute */
#define COL_PAIR1 init_pair(1, COLOR_BLACK, (-1))
#define COL_PAIR2 init_pair(2, COLOR_BLACK, COLOR_CYAN)

typedef struct {
	int 		cell_width;
	int 		cell_height;
	float 	swidth;
	float 	sheight;
} dimensions;

dimensions *init_display(void);
void calc_cell_dimensions(dimensions *sdims_p);

#endif
