/******************************************************************************
 *
 *                               word_to_yPbPr.h
 *
 *     Assignment: arith
 *     Authors:    Marten Tropp and Matt Carey
 *     Date:       10/24/2023
 *      
 *     The purpose of this file is to declare the functions associated with
 *     unpacking values from a 32-bit codeword and extracting the values as  
 *     component video color space. This is for the arith program that 
 *     compresses and decompresses images.
 *     
 *
 *****************************************************************************/

#include "arith_structs.h"

#include <stdio.h>

#ifndef WORD_TO_YPBPR_H
#define WORD_TO_YPBPR_H

typedef struct floatCosineCoeff *floatCosineCoeff;
typedef struct lumaBlock *lumaBlock;

A2Methods_UArray2 toDoublePoint(FILE *input, unsigned width, unsigned height, 
                                A2Methods_T methods);
struct floatCosineCoeff floatABCD(cosineCoeff abcd);
struct lumaBlock toLuma(floatCosineCoeff f_abcd);
void storeFloatBlock(A2Methods_UArray2 doublePoints, A2Methods_T methods, 
                     int col, int row, lumaBlock lumaVals, float chromaPb, 
                     float chromaPr);
struct cosineCoeff extractABCD(uint64_t word);

#endif