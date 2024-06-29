/******************************************************************************
 *
 *                               compress40.c
 *
 *     Assignment: arith
 *     Authors:    Marten Tropp and Matt Carey
 *     Date:       10/24/2023
 *      
 *     The purpose of this file is to implement the compress40 and decompress40
 *     functions defined by compress40.h. These functions are used in a 
 *     program that compresses and decompresses PPM images.
 *
 *    
 *
 *****************************************************************************/

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <pnm.h>
#include <assert.h>

#include "a2methods.h"
#include "a2blocked.h"
#include "compress40.h"
#include "arith40.h"
#include "file_IO.h"
#include "rgb_to_yPbPr.h"
#include "yPbPr_to_rgb.h"
#include "yPbPr_to_word.h"
#include "word_to_yPbPr.h"
#include "arith_structs.h"

/**********************************compress40**********************************
 *
 * Reads in a PPM image, and writes a compressed image to standard output
 * Inputs:
 *         FILE *input: a file that contains an image in PPM format
 * Return: 
 *         none
 * Expects:
 *         Input to be non-null
 * Notes:
 *         CRE if uarray2_methods_blocked, methods->map_default is NULL
 *                      
 *****************************************************************************/
extern void compress40 (FILE *input)
{
        /* Default to UArray2b methods */
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods);

        /* Default to best map */
        A2Methods_mapfun *map = method  flags & (UNDERGRADUATE | DROPPED)
s->map_default;
        assert(map);

        /* Reading contents from the image file and trim odd col/row */
        Pnm_ppm RGBimage = Pnm_ppmread(input, methods);

        /* Trim edges from the image if odd number of rows/cols */
        Pnm_ppm trimmed = trimEdges(RGBimage, methods);
        Pnm_ppmfree(&RGBimage);

        /* Convert elements of RGB image file to double point values */
        A2Methods_UArray2 doublePoints = mapToDoublePoint(trimmed, methods);
        Pnm_ppmfree(&trimmed);

        /* Printing out the header and file */
        printf("COMP40 Compressed image format 2\n%u %u\n", 
               methods->width(doublePoints), 
               methods->height(doublePoints));
        struct yPbPrBlock closure = {0, NULL, NULL, NULL};
        methods->map_default(doublePoints, toWord, &closure);
        methods->free(&doublePoints);
}   

/**********************************decompress40********************************
 *
 * Reads in a compressed image, and writes a PPM image to standard output
 * Inputs:
 *         FILE *input: A file containing the output of the compress40 function
 * Return: 
 *         none
 * Expects:
 *         Input to be non-null
 * Notes:
 *         CRE if uarray2_methods_blocked, methods->map_default is NULL
 *                      
 *****************************************************************************/
extern void decompress40(FILE *input)
{
        /* Default to UArray2b methods */
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods);

        /* Default to best map */
        A2Methods_mapfun *map = methods->map_default;
        assert(map);

        /* Read in width and height from the header */
        unsigned height, width;
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u", 
                          &width, &height);
        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');

        /* Create a UArray and fill with float values */
        A2Methods_UArray2 doublePoints = toDoublePoint(input, width, 
                                                       height, methods);

        /* Convert the float array2 to a PPM, and write it to output */
        Pnm_ppm RGBs = mapToRGB(doublePoints, methods);
        Pnm_ppmwrite(stdout, RGBs);

        /* Free the heap allocated memory */
        methods->free(&doublePoints);
        Pnm_ppmfree(&RGBs);
}  
