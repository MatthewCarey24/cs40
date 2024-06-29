/******************************************************************************
 *
 *                               yPbPr_to_word.h
 *
 *     Assignment: arith
 *     Authors:    Marten Tropp and Matt Carey
 *     Date:       10/24/2023
 *      
 *     The purpose of this file is to declare the functions associated with
 *     taking values in component video color space and packing them into a 
 *     single 32 bit code word. This is done for the arith program, which 
 *     compresses and decompresses images.
 *     
 *     
 *
 *****************************************************************************/

#include "arith_structs.h"

#ifndef YPBPR_TO_WORD_H
#define YPBPR_TO_WORD_H

/******************************yPbPrBlock**************************************
 *
 * Structure that contains Pnm_yPbPr structs from a 2x2 block in a 2D array. It
 * is a helper struct for packing pixels in a block.
 * Stores:
 *         int size: Number of elements currently stored in the struct
 *         Pnm_yPbPr pixel1: Component video color space values of pixel1
 *         Pnm_yPbPr pixel2: Component video color space values of pixel2
 *         Pnm_yPbPr pixel3: Component video color space values of pixel3
 *                      
 *****************************************************************************/
typedef struct yPbPrBlock {
        int size;
        Pnm_yPbPr pixel1;
        Pnm_yPbPr pixel2;
        Pnm_yPbPr pixel3;
} *yPbPrBlock;

void toWord(int col, int row, A2Methods_UArray2 arr, void *elem, void *cl);
uint64_t packReverseOrder(uint32_t word1, uint32_t word2, 
                          uint32_t word3, uint32_t word4);
void printWord(uint32_t word);
uint32_t packWord(uint64_t a, int64_t b, int64_t c, int64_t d, 
                  uint64_t  indexPb, uint64_t indexPr);
uint32_t packBlock(Pnm_yPbPr pixel1, Pnm_yPbPr pixel2,
                   Pnm_yPbPr pixel3, Pnm_yPbPr pixel4);
struct cosineCoeff discreteCosTrans(double y1, double y2, 
                                    double y3, double y4); 

#endifs