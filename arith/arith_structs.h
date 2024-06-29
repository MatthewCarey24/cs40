/******************************************************************************
 *
 *                               arith_structs.h
 *
 *     Assignment: arith
 *     Authors:    Marten Tropp and Matt Carey
 *     Date:       10/24/2023
 *      
 *     The purpose of this file is to typedef the names some of the structs
 *     used in the program that compresses and decompresses images since some
 *     of them are used in multiple stages of the compression/decompression.
 *
 *    
 *
 *****************************************************************************/

#include "a2methods.h"
#include <inttypes.h>

#ifndef ARITH_STRUCTS_H
#define ARITH_STRUCTS_H


/******************************A2Methods_cl************************************
 *
 * Closue for apply functions to pass in A2Methods_UArray2, A2Methods_T, and a
 * denominator value
 * Stores:
 *         A2Methods_UArray2 pixels: A 2D array storing the pixels of an image
 *         A2Methods_T methods: A pointer to a A2Methods_T struct that contains
 *         the methods used for A2Methods_UArray2 pixels
 *         unsigned denominator: The denominator value of image stored in
 *         A2Methods_UArray2 pixels
 *                      
 *****************************************************************************/
typedef struct A2Methods_cl {
        A2Methods_UArray2 pixels;
        A2Methods_T methods;
        unsigned denominator;
} *A2Methods_cl;

/******************************Pnm_yPbPr***************************************
 *
 * Structure that contains component video color space values of a pixel
 * Stores:
 *         double y: luminance value of the pixel
 *         double pb: color-difference signal of the pixel
 *         double pr: color-difference signal of the pixel
 *                      
 *****************************************************************************/
typedef struct Pnm_yPbPr {
        double y;
        double pb;
        double pr;
} *Pnm_yPbPr;

/******************************cosineCoeff*************************************
 *
 * Structure that contains the four brightness values from a 2x2 block in a 2D
 * array
 * Stores:
 *         uint64_t a: The average brightness of the image
 *         int64_t b: The degree to which the image gets brighter as we move
 *         from top to bottom
 *         int64_t c: The degree to which the image gets brighter as we move
 *         from left to right
 *         int64_t d: The degree to which the pixels on one diagonal are 
 *         brighter than the pixels on the other diagnoal
 *                      
 *****************************************************************************/
typedef struct cosineCoeff {
        uint64_t a;
        int64_t b;
        int64_t c;
        int64_t d; 
} *cosineCoeff;

#endif
