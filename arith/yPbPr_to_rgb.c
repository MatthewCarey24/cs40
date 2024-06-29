
/******************************************************************************
 *
 *                               yPbPr_to_rgb.c
 *
 *     Assignment: arith
 *     Authors:    Marten Tropp and Matt Carey
 *     Date:       10/24/2023
 *      
 *     The purpose of this file is to implement the functions associated with
 *     converting values in component video color space to the associated RGB 
 *     values for the arith program, which compresses and decompresses images.
 *     
 *     
 *
 *****************************************************************************/

#include "yPbPr_to_rgb.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "pnm.h"
#include "arith_structs.h"
#include "our_math.h"
#include "file_IO.h"

#include <stdio.h>
#include <stdlib.h>

/**********************************mapToDoublePoint****************************
 *
 * Convert 2D array with component video color space values to RGB values and
 * store them in a Pnm_ppm image struct
 * Inputs:
 *         Pnm_ppm doubleVals: A pointer to a Pnm_ppm struct that contains the
 *         component video color space values of pixels in an image
 *         A2Methods_T methods: A pointer to a A2Methods_T struct that contains
 *         the methods used for storing and handling data
 * Return: A pointer to a Pnm_ppm struct containing the RGB values of an image
 * Expects:
 *         none
 * Notes:
 *         The memory allocated for image must be deallocated elsewhere
 *         using the Pnm_ppnfree function
 *                      
 *****************************************************************************/
Pnm_ppm mapToRGB(A2Methods_UArray2 doubleVals, A2Methods_T methods)
{
        /* Allocating space for pixels of the image */
        Pnm_ppm image = Pnm_ppm_new(methods->width(doubleVals),
                                    methods->height(doubleVals),
                                    255,
                                    methods,
                                    sizeof(struct Pnm_rgb));
        

        /* Converting float-point values to RGB values */
        struct A2Methods_cl closure = {image->pixels, methods, 255};
        methods->map_default(doubleVals, convertToRGBApply, &closure);

        return image;
} 

/**********************************convertToRGBApply***************************
 *
 * Convert element in a A2Methods_UArray2 struct with component video color 
 * space values to RGB values and store them in a A2Methods_UArray2 struct
 * passed through the cl pointer
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
 *         will store the converted element as a Pnm_rgb struct
 * Return: none
 * Expects:
 *         none
 * Notes:
 *         This is an apply function for mapping through a A2Methods_UArray2 
 *         that is called in mapToRGB
 *                      
 *****************************************************************************/
void convertToRGBApply(int col, int row, A2Methods_UArray2 arr, 
                              void *elem, void *cl)
{
        /* Accessing location where elem needs to be put in result */
        A2Methods_cl closure = cl;
        Pnm_rgb currLoc = closure->methods->at(closure->pixels, col, row);
        
        /* Converting values and placing element in correct location */
        Pnm_yPbPr pix = elem;

        /* Converting float-point values to RGB values */
        struct Pnm_rgb converted = convertToRGB(pix->y, pix->pb, pix->pr);
        *currLoc = converted;

        (void) arr;
}

/**********************************convertToRGB********************************
 *
 * Convert component video color space values to RGB values and store them in a
 * Pnm_rgb struct
 * Inputs:
 *         double y: luminance value of the pixel
 *         double pb: color-difference signal of the pixel
 *         double pr: color-difference signal of the pixel
 * Return: A struct Pnm_rgb that contains the RGB values calculated using the 
 *         provided values
 * Expects:
 *         none
 * Notes:
 *         none
 *                      
 *****************************************************************************/
struct Pnm_rgb convertToRGB(double y, double pb, double pr)
{
        /* Calculating the float-point values */
        double fRed = 1.0 * y + 0.0 * pb + 1.402 * pr; 
        fRed = setRange(fRed, 0.0, 1.0);

        double fGreen = 1.0 * y - 0.344136 * pb - 0.714136 * pr;
        fGreen = setRange(fGreen, 0.0, 1.0);

        double fBlue = 1.0 * y + 1.772 * pb + 0.0 * pr;
        fBlue = setRange(fBlue, 0.0, 1.0);

        /* Calculating the RGB values */
        unsigned red = (unsigned) (fRed *  255.0);
        unsigned green = (unsigned) (fGreen * 255.0);
        unsigned blue = (unsigned) (fBlue *  255.0);

        /* Storing the values in a struct */
        struct Pnm_rgb converted = {red, green, blue};

        return converted; 
}