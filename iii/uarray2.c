/*uarray2.c
* written by Matthew Carey and Kaelen Song
* HW02: iii
*
* Summary: implements the UArray2 ADT, a 2 dimensional unboxed array
*/

#include "uarray2.h"
#include <stdlib.h> 
#include <assert.h>
#include <stdbool.h>

#define T UArray2_T
 
/********** extern UArray2_T UArray2_new ********
 *
 * initializes a new array of elements of ElementSize
 *
 * Parameters:
 *       const int width: the width of the new array
 *       const int height: the height of the new array
 *       const int ElementSize: the size of the elements that will be stored in
 *        the array
 * Return: a pointer to where the array is stored in memory
 *
 * Expects 
 *      width and height to be greater than 0, ElementSize
 *      must be greater than 0
  * Notes:
 *      will CRE if aformentioned expectations are not met
 ************************/
extern UArray2_T UArray2_new(const int width, const int height, 
                            const int ElementSize) 
{
    assert(width > 0 && height > 0);
    assert(ElementSize > 0);

    struct T *T = malloc(sizeof(struct T));
    T->width = width;
    T->height = height;
    T->elementSize = ElementSize;

    T->data = UArray_new(width*height, ElementSize);
    
    return T;
}

/********** UArray2_width ********
 *
 * gets the width of the UArray
 *
 * Parameters:
 *       T array: a pointer to the UArray
 * Return: the width of the UArray, as a const integer variable
 *
 * Expects 
 *      array to be none null, width to be greater than 0
  * Notes:
 *      will CRE if aformentioned expectations are not met
 ************************/
int UArray2_width(T array) 
{
    assert(array != NULL);
    assert(array->width > 0);
    return array->width;
}

/********** UArray2_width ********
 *
 * gets the heigh of the UArray
 *
 * Parameters:
 *       T array: a pointer to the UArray
 * Return: the height of the UArray, as an integer variable
 *
 * Expects 
 *      array to be none null, height to be greater than 0
  * Notes:
 *      
 ************************/
int UArray2_height(T array) 
{
    /*check that array exists*/
    assert(array != NULL);
    /*check that array is 1x1 or greater*/
    assert(array->height > 0);
    return array->height;
}

/********** UArray2_size ********
 *
 * gets the size of the elements within the UArray2
 *
 * Parameters:
 *       T array: a pointer to the UArray
 * Return: the size of the elements stored within UArray as an integer
 *
 * Expects 
 *      array to be none null
  * Notes:
 *      will CRE if aformentioned expectations are not met
 ************************/
int UArray2_size(T array) 
{
    assert(array != NULL);
    return array->elementSize;
}

/********** UArray2_at ********
 *
 * gets/sets the element at a specified position in the array
 *
 * Parameters:
 *       T array: a pointer to the UArray2
 *       int col: the y value of the element to be set
 *       int row: the x value of the element to be set
 * Return: a pointer to the element
 *
 * Expects 
 *      array to be none null, column and row to be in bounds
  * Notes:
 *      will CRE if aformentioned expectations are not met
 ************************/
void *UArray2_at(T array, int col, int row) 
{
    assert(array != NULL);
    assert(col >= 0 && col < array->width);
    assert(row >= 0 && row < array->height);

    int index = row_major_index(array, col, row);
    return UArray_at(array->data, index);
}

/********** col_major_index ********
 *
 * returns the column-major index of the 1D array at a 
 * specified index in the representational 2D array
 *  
 * Parameters:
 *       T array: a pointer to the UArray
 *       int col: the y value of the index to be found
 *       int row: the x value of the index to be found
 * Return: the column major index of the specified element
 *
 * Expects
 *      array not to be NULL
 ************************/
int col_major_index(T array, int col, int row) 
{
    return array->height * col + row;
}

/********** row_major_index ********
 *
 * returns the row-major index of the 1D array at a 
 * specified index in the representational 2D array
 *  
 * Parameters:
 *       T array: a pointer to the UArray
 *       int col: the y value of the index to be found
 *       int row: the x value of the index to be found
 * Return: the row major index of the specified element
 *
 * Expects
 *      array not to be NULL
 ************************/
int row_major_index(T array, int col, int row) 
{
    return array->width * row + col;
}

/********** col_from_row_major ********
 *
 * returns the column value of a specified index of 
 * a 2D array represented by a 1D array in row-major format
 * 
 * Parameters:
 *       T array: a pointer to the UArray
 *       int index: the index of the element in the 1D array
 * Return: the column value of the spcified index
 *      
 * Expects
 *      array not to be NULL
 ************************/
int col_from_row_major(T array, int index) 
{
    return index % array->width;
}

/********** row_from_row_major ********
 *
 * returns the row value of a specified index of 
 * a 2D array represented by a 1D array in row-major format
 * 
 * Parameters:
 *       T array: a pointer to the UArray
 *       int index: the index of the element in the 1D array
 * Return: the row value of the spcified index
 *      
 * Expects
 *      array not to be NULL
 ************************/
int row_from_row_major(T array, int index) 
{
    return index / array->width; 
}

/********** col_as_col_major ********
 *
 * returns the column value of a specified index of 
 * a 2D array represented by a 1D array in col-major format
 * 
 * Parameters:
 *       T array: a pointer to the UArray
 *       int index: the index of the element in the 1D array
 * Return: the column value of the spcified index
 *      
 * Expects
 *      array not to be NULL
 ************************/
int col_as_col_major(T array, int index) 
{
    return index / array->height;
}

/********** row_as_col_major ********
 *
 * returns the row value of a specified index of 
 * a 2D array represented by a 1D array in col-major format
 * 
 * Parameters:
 *       T array: a pointer to the UArray
 *       int index: the index of the element in the 1D array
 * Return: the row value of the spcified index
 *      
 * Expects
 *      array not to be NULL
 ************************/
int row_as_col_major(T array, int index) 
{
    return index % array->height;
}

/********** UArray2_map_row_major ********
 *
 * maps to each element in the array, allowing for an apply function to be 
 * called on each element, row major
 * 
 * Parameters:
 *       T array: a pointer to the UArray
 *       void apply: takes a function that will be applied to every element
 *       void *cl: closure to be tracked across each application
 * Return: void
 *      
 * Expects
 *      array not to be NULL
 ************************/
void UArray2_map_row_major(T array, 
void apply(int col, int row, T array, void *currValue, void *cl), void *cl) 
{
    int array_length = UArray_length(array->data);
    for (int i = 0; i < array_length; i++) {
       /*return index of curr element*/
       int col = col_from_row_major(array, i);
       int row = row_from_row_major(array, i);
        /*pass to apply*/
       apply(col, row, array, UArray2_at(array, col, row), cl);
    }
}

/********** UArray2_map_col_major ********
 *
 * maps to each element in the array, allowing for an apply function to be 
 * called on each element, col major
 * 
 * Parameters:
 *       T array: a pointer to the UArray
 *       void apply: takes a function that will be applied to every element
 *       void *cl: closure to be tracked across each application
 * Return: void
 *      
 * Expects
 *      array not to be NULL
 ************************/
void UArray2_map_col_major(T array, 
void apply(int col, int row, T array, void *currValue, void *cl), void *cl) 
{
    int array_length = UArray_length(array->data);
    for (int i = 0; i < array_length; i++) {
       /*return index of curr element*/
       int col = col_as_col_major(array, i);
       int row = row_as_col_major(array, i);
        /*pass to apply*/
       apply(col, row, array, UArray2_at(array, col,row), cl);
    }
}
/********** UArray2_free ********
 *
 * deallocates the array itself and the T structure, and clears its argument
 * 
 * Parameters:
 *       T array: a pointer to the UArray
 * Return: 
 *      none
 * Expects
 *      none
 ************************/
void UArray2_free(T *array) 
{
    UArray_free(&((*array)->data));
    free(*array);
}

#undef T