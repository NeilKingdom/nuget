#ifndef DISPLAY_H
#define DISPLAY_H

/* -1 mean no color, therefore, the background color wont exist with this attribute */
#define COL_PAIR1 init_pair(1, COLOR_WHITE, (-1))
#define COL_PAIR2 init_pair(2, COLOR_WHITE, COLOR_CYAN)

typedef struct Dimensions {
	int cell_width;
	int cell_height;
	float f_sWidth;
	float f_sHeight;
} dimensions;

dimensions* init_display(void);
void calc_cell_dimensions(dimensions* p_sDimensions);
void nuget_refresh(void);

#endif
