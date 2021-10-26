#ifndef ACTIONS_H
#define ACTIONS_H

/* TODO: Provide the ability to change keys with XK keycodes similar to suckless 
#include <keysym.h> */

#define SHIFT
#define CURSOR_UP 'k'
#define CURSOR_DOWN 'j'
#define CURSOR_RIGHT 'l'
#define CURSOR_LEFT 'h'
#define ENTER '\n'
#define SAVE 's'
#define QUIT 'q'

void nuget_mvchgat(int y, int x, int cell_width, int attr, int col);

#endif
