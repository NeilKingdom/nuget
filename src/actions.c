#include <ncurses.h>

#include "../deps/actions.h"

/********************************************
                 Functions
********************************************/

/********************************************
Name: nuget_mvchgat
Author: Neil Kingdom
Date: Oct 25, 2021
Return: N/A
Params: y - current y position
        x - current x position
		  cell_width - width of each cell
	     atrr - attributes to give cell
		  col - color pair to give cell
Purpose: wrapper function for ncurses 
	mvchgat() func 
********************************************/
void nuget_mvchgat(int y, int x, int cell_width, int attr, int col) {

	mvchgat(y, x, cell_width, attr, col, NULL);
	return;
}
