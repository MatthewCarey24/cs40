/******************************************************************************
 *
 *                               yPbPr_to_word.c
 *
 *     Assignment: arith
 *     Authors:    Marten Tropp and Matt Carey
 *     Date:       10/24/2023
 *      
 *     The purpose of this file is to implement the functions associated with
 *     taking values in component video color space and packing them into a 
 *     single 32 bit code word for the arith program.
 *     
 *     
 *
 *****************************************************************************/

#include "a2methods.h"
#include "a2blocked.h"
#include "pnm.h"
#include "bitpack.h"
#include "yPbPr_to_word.h"
#include "our_math.h"
#include "arith40.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>



/**********************************toWord**************************************
 *
 * Convert the component video color space values of a 2x2 block in a 2D array
 * to 32-bit integer values and print them out
 * Inputs:
 *         int col: The column position of elem in A2Methods_UArray2 arr
 *         int row: The row position of elem in A2Methods_UArray2 arr
 *         A2Methods_UArray2 arr: A 2D array that contains elem at position
 *         (col, row)
 *         void *elem: The element at location (col, row) in 
 *         A2Methods_UArray2 arr that is put into the result Pnm_ppm struct
 *         passed in through void *cl
 *         void *cl: Closure pointer that in this case contains a pointer to a
 *         yPbPrBlock struct that contains the other three Pnm_yPbPr structs
 *         from the current 2x2 block
 * Return: none
 * Expects:
 *         none
 * Notes:
 *         This is an apply function for mapping through a A2Methods_UArray2
 *                      
 *****************************************************************************/
void toWord(int col, int row, A2Methods_UArray2 arr, void *elem, void *cl)
{       
        /* Storing values of current pixel */
        yPbPrBlock block = cl;
        if (block->size == 0) {
                block->pixel1 = (Pnm_yPbPr) elem;
        }
        else if (block->size == 1) {
                block->pixel2 = (Pnm_yPbPr) elem;
        }
        else if (block->size == 2) {
                block->pixel3 = (Pnm_yPbPr) elem;
        }
        else if (block->size == 3) {
                Pnm_yPbPr pixel4 = elem;

                /* Packing the values in one variable */
                uint32_t word = packBlock(block->pixel1, block->pixel2, 
                                          block->pixel3, pixel4);
                
                /* Printing out the values */
                printWord(word);
        }

        /* Keeping track of number of elements stored */
        block->size = (block->size + 1) % 4;
        (void) col;
        (void) row;
        (void) arr;
}

/**********************************packReverseOrder****************************
 *
 * Pack the 8-bits stored in a 32-bit integer into a 64-bit integer in reverse
 * order. They are given in little Endian order, but the packing reverses them
 * and puts them in big Endian order
 * Inputs:
 *         uint32_t word1: The first value that is stored
 *         uint32_t word2: The second value that is stored
 *         uint32_t word3: The third value that is stored
 *         uint32_t word4: The fourth value that is stored
 * Return: 64-bit integer that has the provided values stored in reverse order
 * Expects:
 *         none
 * Notes:
 *         This function use Bitpacking to store the values
 *                      
 *****************************************************************************/
uint64_t packReverseOrder(uint32_t word1, uint32_t word2, 
                          uint32_t word3, uint32_t word4)
{
        uint64_t rearranged = 0;

        /* Pack the words into a variable */
        rearranged = Bitpack_newu(rearranged, 8, 24, word1);
        rearranged = Bitpack_newu(rearranged, 8, 16, word2);
        rearranged = Bitpack_newu(rearranged, 8, 8, word3);
        rearranged = Bitpack_newu(rearranged, 8, 0, word4);

        return rearranged;
}

/**********************************printWord***********************************
 *
 * Print the 8-bits stored in a 32-bit integer in big Endian order
 * Inputs:
 *         uint32_t word: The 32-bit integer containing the four 8-bit values
 * Return: none
 * Expects:
 *         none
 * Notes:
 *         Prints out the 8-bit values as a character
 *                      
 *****************************************************************************/
void printWord(uint32_t word)
{
        uint64_t firstBlock = Bitpack_getu(word, 8, 0);
        uint64_t secBlock = Bitpack_getu(word, 8, 8);
        uint64_t thirdBlock = Bitpack_getu(word, 8, 16);
        uint64_t fourthBlock = Bitpack_getu(word, 8, 24);

        putchar(fourthBlock);
        putchar(thirdBlock);
        putchar(secBlock);
        putchar(firstBlock);        
}

/**********************************packWord************************************
 *
 * Pack different sized values associated with a 2x2 block in an image into a
 * 64-bit integer
 * Inputs:
 *         uint64_t a: The average brightness of the 2x2 block
 *         int64_t b: The degree to which the image gets brighter as we move
 *         from top to bottom of the 2x2 block
 *         int64_t c: The degree to which the image gets brighter as we move
 *         from left to right of the 2x2 block
 *         int64_t d: The degree to which the pixels on one diagonal are 
 *         brighter than the pixels on the other diagnoal of the 2x2 block
 *         uint64_t indexPb: The index value of color-difference signal of the 
 *         2x2 block
 *         uint64_t indexPr: The index value of color-difference signal of the 
 *         2x2 block
 * Return: 32-bit integer that has the provided values stored
 * Expects:
 *         none
 * Notes:
 *         This function use Bitpacking to store the values
 *                      
 *****************************************************************************/
uint32_t packWord(uint64_t a, int64_t b, int64_t c, int64_t d, 
                  uint64_t indexPb, uint64_t indexPr)
{
        uint64_t word = 0;

        /* Packing in the value of a */
        word = Bitpack_newu(word, 9, 23, a);

        /* Packing in the value of b */
        word = Bitpack_news(word, 5, 18, b);

        /* Packing in the value of c */
        word = Bitpack_news(word, 5, 13, c);

        /* Packing in the value of d */
        word = Bitpack_news(word, 5, 8, d);

        /* Packing in the value of indexPb */
        word = Bitpack_newu(word, 4, 4, indexPb);
        
        /* Packing in the value of indexPr */
        word = Bitpack_newu(word, 4, 0, indexPr);

        /* Casting to a 32-bit value */
        uint32_t word32 = (uint32_t) word;


        return word32;
}

/**********************************packBlock***********************************
 *
 * Pack component video color space values of every pixel in a 2x2 block in a
 * 32-bit integer
 * Inputs:
 *         Pnm_yPbPr pixel1: Component video color space values of the first
 *         pixel in a 2x2 block
 *         Pnm_yPbPr pixel2: Component video color space values of the second
 *         pixel in a 2x2 block
 *         Pnm_yPbPr pixel3: Component video color space values of the third
 *         pixel in a 2x2 block
 *         Pnm_yPbPr pixel4: Component video color space values of the fourth
 *         pixel in a 2x2 block
 * Return: 32-bit integer that has the provided values stored
 * Expects:
 *         none
 * Notes:
 *         none
 *                      
 *****************************************************************************/
uint32_t packBlock(Pnm_yPbPr pixel1, Pnm_yPbPr pixel2, 
                   Pnm_yPbPr pixel3, Pnm_yPbPr pixel4)
{
        /* Getting average chroma values */
        double avgPb = averageOfFour(pixel1->pb, pixel2->pb, 
                                     pixel3->pb, pixel4->pb);
        double avgPr = averageOfFour(pixel1->pr, pixel2->pr, 
                                     pixel3->pr, pixel4->pr);
        
        /* Rounding chroma values to be within [-0.5, 0.5] */
        avgPb = setRange(avgPb, -0.5, 0.5);
        avgPr = setRange(avgPr, -0.5, 0.5);

        /* Converting to index values */
        uint64_t indexPb = (uint64_t) Arith40_index_of_chroma(avgPb);
        uint64_t indexPr = (uint64_t) Arith40_index_of_chroma(avgPr);
        
        /* Get a, b, c, and d from the Y values using DCT */
        struct cosineCoeff abcd = discreteCosTrans(pixel1->y, pixel2->y,
                                                   pixel3->y, pixel4->y);

        /* Packing the data into a single integer */
        uint32_t word = packWord(abcd.a, abcd.b, abcd.c, abcd.d, indexPb, 
                                 indexPr);

        return word;
}

/**********************************discreteCosTrans****************************
 *
 * Calculate the different brightness values associated with a 2x2 pixel in an
 * image and store them in a struct as 64-bit integers
 * Inputs:
 *         double y1: Luminance value of the first pixel in the 2x2 block
 *         double y2: Luminance value of the second pixel in the 2x2 block
 *         double y3: Luminance value of the third pixel in the 2x2 block
 *         double y4: Luminance value of the fourth pixel in the 2x2 block
 * Return: A struct cosineCoeff containing the different brightness values 
 * associated with a 2x2 pixel in an image as 64-bit integers
 * Expects:
 *         none
 * Notes:
 *         none
 *                      
 *****************************************************************************/
struct cosineCoeff discreteCosTrans(double y1, double y2, double y3, double y4)
{
        /* Calculating values */
        double fA = (y4 + y3 + y2 + y1) / 4.0;
        double fB = setRange((y4 + y3 - y2 - y1) / 4.0, -0.3, 0.3);
        double fC = setRange((y4 - y3 + y2 - y1) / 4.0, -0.3, 0.3);
        double fD = setRange((y4 - y3 - y2 + y1) / 4.0, -0.3, 0.3);
        
        /* Converting to 64-bit numbers */
        uint64_t a = (uint64_t) (511.0 * fA);
        int64_t b = (int64_t) (50.0 * fB);
        int64_t c = (int64_t) (50.0 * fC);
        int64_t d = (int64_t) (50.0 * fD);

        struct cosineCoeff abcd = {a, b, c, d};

        return abcd;
}