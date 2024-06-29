/******************************************************************************
 *
 *                                  uarray2.h
 *
 *     Assignment: iii
 *     Authors:    Marten Tropp and Mark Martirosian
 *     Date:       9/28/2023
 *      
 *     The purpose of this file is to declare a UArray2_T structure and also 
 *     declare any functions related to it. The structure is used in a program
 *     that detects whether a Sudoku puzzle is solved. The UArray2 structure is
 *     used to store a puzzle in that program. It has eight functions related
 *     to it which allow the user to initialize a new UArray2 variable, get its
 *     width, height, and element size, access data at specific locations, map
 *     every element of the UArray2 row by row or col by col, and a function
 *     that frees any memory related to an instance of the UArray2.
 *
 *    
 *
 *****************************************************************************/
#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED

#include "uarray.h"

typedef struct UArray2_T *UArray2_T;

UArray2_T UArray2_new(int cols, int rows, int size_elem);
int UArray2_width(UArray2_T arr);
int UArray2_height(UArray2_T arr);
int UArray2_size(UArray2_T arr);
void *UArray2_at(UArray2_T arr, int col, int row);
void UArray2_map_col_major(UArray2_T arr, void apply(int col, int row, 
                           UArray2_T arr, void *x, void *cl), void *cl);
void UArray2_map_row_major(UArray2_T arr, void apply(int col, int row, 
                           UArray2_T arr, void *x, void *cl), void *cl);
void UArray2_free(UArray2_T *arr);

#endif