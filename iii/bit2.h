/*bit2.h
* written by Matthew Carey and Kaelen Song
* HW02: iii
*
* Summary: defines the methods for Bit2, which will be implmented
*     in bit2.c. These methods create a 2D array represnted by a 1D array
*     in row-major that holds a file of integers or bits. 
*/
#ifndef Bit2_INCLUDED
#define Bit2_INCLUDED

#include <uarray.h>

#define T Bit2_T
typedef struct T *T;

struct T{
    int width;
    int height; 
    UArray_T data;
};

extern T Bit2_new(const int width, const int height);

int Bit2_width(T array);
int Bit2_height(T array);
int Bit2_size(T array);

int Bit2_put(T array, int col, int row, int value);
int Bit2_get(T array, int col, int row);

int row_major_index(T array, int col, int row);
int col_major_index(T array, int col, int row);

int col_from_row_major(T array, int index);
int row_from_row_major(T array, int index);
int col_from_col_major(T array, int index);
int row_from_col_major(T array, int index);

void Bit2_map_row_major(T array, void apply(int col, int row, T array, 
                        int val, void *cl), void *cl);
void Bit2_map_col_major(T array, void apply(int col, int row, T array, 
                        int val, void *cl), void *cl);

void Bit2_free(T *array);


#undef T
#endif
