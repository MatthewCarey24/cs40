
/******************************************************************************
 *
 *                               rgb_to_yPbPr.c
 *
 *     Assignment: arith
 *     Authors:    Marten Tropp and Matt Carey
 *     Date:       10/24/2023
 *      
 *     The purpose of this file is to implement the functions associated with
 *     converting RGB values in a ppm image into the component video color 
 *     space.
 *     
 *
 *****************************************************************************/

#include "rgb_to_yPbPr.h"
#include "a2blocked.h"
#include "arith40.h"
#include "arith_structs.h"

#include <stdio.h>
#include <stdlib.h>

/**********************************mapToDoublePoint****************************
 *
 * Convert image with RGB values to component video color space values and
 * store them in a 2D array
 * Inputs:
 *         Pnm_ppm img: A pointer to a Pnm_ppm struct that contains the data
 *         of the image
 *         A2Methods_T methods: A pointer to a A2Methods_T struct that contains
 *         the methods used for storing and handling data
 * Return: A pointer to a A2Methods_UArray2 struct containing the component
 *         video color space values
 * Expects:
 *         none
 * Notes:
 *         The memory allocated for doubleVals must be deallocated elsewhere
 *         using the free function in A2Methods_T methods
 *                      
 *****************************************************************************/
A2Methods_UArray2 mapToDoublePoint(Pnm_ppm img, A2Methods_T methods)
{
        /* Allocating memory, where a Pnm_yPbPr is 24 bytes */
        A2Methods_UArray2 doubleVals = methods->new_with_blocksize(img->width,
                                     img->height, 24, 2);
        
        /* Traversing through img and converting the values */
        struct A2Methods_cl closure = {doubleVals, methods, img->denominator};
        methods->map_default(img->pixels, convertToDoublePointApply, 
                             &closure);

        return doubleVals;
} 

/**********************************convertToDoublePointApply*******************
 *
 * Convert element in a A2Methods_UArray2 struct with RGB values to component
 * video color space values and store them in another A2Methods_UArray2 passed
 * through the cl pointer
 * Inputs:
 *         int col: The column position of elem in A2Methods_UArray2 arr
 *         int row: The row position of elem in A2Methods_UArray2 arr
 *         A2Methods_UArray2 arr: A 2D array that contains elem at position
 *         (col, row)
 *         void *elem: The element at location (col, row) in 
 *         A2Methods_UArray2 arr that is put into the result A2Methods_UArray2
 *         passed in through void *cl
 *         void *cl: Closure pointer that in this case contains a pointer to a
 *         A2Methods_cl struct that contains has the methods and 2D array that
 *         will store the converted element as a Pnm_yPbPr struct
 * Return: none
 * Expects:
 *         none
 * Notes:
 *         This is an apply function for mapping through a A2Methods_UArray2 
 *         that is called in mapToDoublePoint
 *                      
 *****************************************************************************/
void convertToDoublePointApply(int col, int row, A2Methods_UArray2 arr, 
                               void *elem, void *cl)
{
        /* Accessing location where elem needs to be put in result */
        A2Methods_cl closure = cl;
        Pnm_yPbPr currLoc = closure->methods->at(closure->pixels, col, 
                                                       row);
        
        /* Converting values and placing element in correct location */
        Pnm_rgb pix = elem;
        struct Pnm_yPbPr converted = convertToDoublePoint(pix->red, 
                                  pix->green, pix->blue, closure->denominator);
        *currLoc = converted;

        (void) arr;
}

/**********************************convertToDoublePoint************************
 *
 * Convert RGB values to component video color space values and store them in a
 * Pnm_yPbPr struct
 * Inputs:
 *         unsigned red: The red value of the current pixel
 *         unsigned green: The green value of the current pixel
 *         unsigned blue: The blue value of the current pixel
 *         unsigned denominator: The denominator value of the image
 * Return: A struct Pnm_yPbPr that contains the component video color space 
 *         values calculated using the provided values
 * Expects:
 *         none
 * Notes:
 *         none
 *                      
 *****************************************************************************/
struct Pnm_yPbPr convertToDoublePoint(unsigned red, unsigned green, 
                                      unsigned blue, unsigned denominator)
{
        /* Casting values to doubles */
        double fRed = (double) red / (double) denominator;
        double fGreen = (double) green / (double) denominator;
        double fBlue = (double) blue / (double) denominator;

        /* Calculating the float-point values */
        double y = 0.299 * fRed + 0.587 * fGreen + 0.114 * fBlue;
        double pb = -0.168736 * fRed - 0.331264 * fGreen + 0.5 * fBlue;
        double pr = 0.5 * fRed - 0.418688 * fGreen - 0.081312 * fBlue;

        /* Storing the values in a struct */
        struct Pnm_yPbPr converted = {y, pb, pr};

        return converted; 
}