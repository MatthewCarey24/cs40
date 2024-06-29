/*bit2.c
* written by Matthew Carey and Kaelen Song
* HW02: iii
*
* Summary: implements the Bit2 ADT, a 2 dimensional bit array
*/

#include "bit2.h"
#include <stdlib.h> 
#include <assert.h>
#include <stdbool.h>

#define T Bit2_T
 
/********** extern Bit2_T Bit2_new ********
 *
 * initializes a new array of integers/bits
 *
 * Parameters:
 *       const int width: the width of the new array
 *       const int height: the height of the new array
 * Return: a pointer to where the array is stored in memory
 *
 * Expects 
 *      width and height must be greater than 0
 ************************/
extern Bit2_T Bit2_new(const int width, const int height){
    assert(width > 0 && height > 0);

    struct T *T = malloc(sizeof(struct T));
    T->width = width;
    T->height = height;

/* dont need elementSize, but need to put it here */
    T->data = UArray_new(width*height, 8);
    
    return T;
}

/********** Bit2_width ********
 *
 * gets the width of the UArray
 *
 * Parameters:
 *       T array: a pointer to the UArray
 * Return: the width of the UArray, as a integer variable
 *
 * Expects
 *      T, the pointer to array not to be NULL
 * Notes:
 *      Will CRE if array is NULL
 ************************/
int Bit2_width(T array){
    assert(array);
    assert(array->width > 0);
    return array->width;
}

 /********** Bit2_height ********
 *
 * gets the width of the UArray
 *
 * Parameters:
 *       T array: a pointer to the UArray
 * Return: the height of the UArray, as a integer variable
 *
 * Expects
 *      T, the pointer to array not to be NULL, height is greater than 1
 * Notes:
 *      Will CRE if array is NULL, or height it less than 1
 ************************/
int Bit2_height(T array){
    /*check that array exists*/
    assert(array);
    /*check that array is 1x1 or greater*/
    assert(array->height > 0);
    return array->height;
}


/********** Bit2_get ********
 *
 * gets the element at a specified position in the array
 *
 * Parameters:
 *       T array: a pointer to the UArray
 *       int col: the y value of the element to be obtained
 *       int row: the x value of the element to be obtained
 * Return: The element, in the form of an integer
 *
 * Expects
 *      array not to be NULL, col and row to be less than width and height, 
 *      respectively and greater than or equal to 0
 * Notes:
 *      Will CRE if the aforementioned expectations are not met
 ************************/
int Bit2_get(T array, int col, int row){
    assert(array != NULL);
    assert(col >= 0 && col < array->width);
    assert(row >= 0 && row < array->height);

    int index = row_major_index(array, col, row);
    return *(int *)UArray_at(array->data, index);
}

/********** Bit2_put ********
 *
 * sets the element at a specified position in the array
 *
 * Parameters:
 *       T array: a pointer to the UArray
 *       int col: the y value of the element to be set
 *       int row: the x value of the element to be set
 * Return: the old element that has just been replaced by the new one
 *
 * Expects
 *      array not to be NULL, col and row to be less than width and height, 
 *      respectively and greater than or equal to 0
 * Notes:
 *      Will CRE if the aforementioned expectations are not met
 ************************/
int Bit2_put(T array, int col, int row, int value){
    assert(array != NULL);
    assert(col >= 0 && col < array->width);
    assert(row >= 0 && row < array->height);

    int output = Bit2_get(array, col, row);

    int index = row_major_index(array, col, row);
    *(int *)UArray_at(array->data, index) = value; 
    return output;

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
int col_major_index(T array, int col, int row){
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
int row_major_index(T array, int col, int row){
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
int col_from_row_major(T array, int index){
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
int row_from_row_major(T array, int index){
    return index / array->width; 
}

/********** col_from_col_major ********
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
int col_from_col_major(T array, int index){
    return index / array->height;
}

/********** row_from_col_major ********
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
int row_from_col_major(T array, int index){
    return index % array->height;
}

/********** Bit2_map_row_major ********
 *
 * maps to each element in the array, allowing for an apply function to be 
 * called on each element, row major
 * 
 * Parameters:
 *       T array: a pointer to the UArray
 *       void apply: takes a function that will be applied to every element
 *       void *cl: a closure argument that can be used by the caller of the map
 *                 function as they desire
 * Return: no direct returns
 *      
 * Expects
 *      array not to be NULL
 ************************/
void Bit2_map_row_major(T array, 
void apply(int col, int row, T array, int value, void *cl), void *cl){

    int array_length = UArray_length(array->data);
    for (int i = 0; i < array_length; i++){
       /*return index of curr element*/
       int col = col_from_row_major(array, i);
       int row = row_from_row_major(array, i);
        /*pass to apply*/
       apply(col, row, array, Bit2_get(array, col, row), cl);
    }
}

/********** Bit2_map_col_major ********
 *
 * maps to each element in the array, allowing for an apply function to be 
 * called on each element, row major
 * 
 * Parameters:
 *       T array: a pointer to the UArray
 *       void apply: takes a function that will be applied to every element
 *       void *cl: a closure argument that can be used by the caller of the map
 *                 function as they desire
 * Return: no direct returns
 *      
 * Expects
 *      array not to be NULL
 ************************/
void Bit2_map_col_major(T array,
        void apply(int col, int row, T array, int value, void *cl), void *cl){
    int array_length = UArray_length(array->data);
    for (int i = 0; i < array_length; i++){
       /*return index of curr element */
       int col = col_from_col_major(array, i);
       int row = row_from_col_major(array, i);
        /*pass to apply*/
       apply(col, row, array, Bit2_get(array, col, row), cl);
    }
}

/********** Bit2_free ********
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
void Bit2_free(T *array){
    UArray_free(&((*array)->data));

    free(*array);
}

#undef T