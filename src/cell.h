#ifndef CELL_H
#define CELL_H

#include "dyn_array.h"

/*
 
Look of a cell:

 1  2
  AB 
  CD 
 4  3

- body cells:
  - A: pos from here
- flagella:
  - 
*/


typedef struct
{
    int pos_x, pos_y;
    int flag_1,
        flag_2,
        flag_3,
        flag_4;
} Cell;

DEFINE_DYN_ARRAY(Cell, DynArrayCell, dyn_array_cell)

#endif // CELL_H
