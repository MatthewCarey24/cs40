/******************************************************************************
 *
 *                               file_IO.h
 *
 *     Assignment: arith
 *     Authors:    Marten Tropp and Matt Carey
 *     Date:       10/24/2023
 *      
 *     The purpose of this file is to declare the functions associated with
 *     reading input from files for the arith program that compresses and 
 *     decompresses images. It also deals with formatting the file properly for
 *     compression.
 *
 *    
 *
 *****************************************************************************/

#include <pnm.h>

#ifndef FILE_IO_H
#define FILE_IO_H

Pnm_ppm Pnm_ppm_new(unsigned width, unsigned height, unsigned denominator,
                    A2Methods_T methods, int size);
Pnm_ppm trimEdges(Pnm_ppm image, A2Methods_T methods);
int isOdd(unsigned num);
void copyApply(int col, int row, A2Methods_UArray2 arr, void *elem, void *cl);
uint32_t readWord(FILE *input);
uint32_t readChar(FILE *input);


#endif