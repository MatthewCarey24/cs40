/*uarray2.h
* written by Matthew Carey and Kaelen Song
* HW02: iii
* Summary: defines the methods for UArray2, which will be implmented
*     in uarray2.c. These methods create the UArray2 ADT, a 2 
*     dimensional unboxed array.
*/

#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED

#include <uarray.h>

#define T UArray2_T
typedef struct T *T;

struct T{
    int width;
    int height; 
    int elementSize;
    UArray_T data;
};

extern T UArray2_new(const int width, const int height, const int ElementSize);

int UArray2_width(T array);
int UArray2_height(T array);

int UArray2_size(T array);

void *UArray2_at(T array, int col, int row);

int row_major_index(T array, int col, int row);
int col_major_index(T array, int col, int row);

int col_from_row_major(T array, int index);
int row_from_row_major(T array, int index);
int col_as_col_major(T array, int index);
int row_as_col_major(T array, int index);

void UArray2_map_row_major(T array, void apply(int col, int row, T array, 
                            void *currValue, void *cl), void *cl);
void UArray2_map_col_major(T array, void apply(int col, int row, T array, 
                            void *currValue, void *cl), void *cl);

void UArray2_free(T *array);


#undef T
#endif
