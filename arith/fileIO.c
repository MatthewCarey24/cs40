/******************************************************************************
 *
 *                               file_IO.c
 *
 *     Assignment: arith
 *     Authors:    Marten Tropp and Matt Carey
 *     Date:       10/24/2023
 *      
 *     The purpose of this file is to define the functions associated with
 *     reading input from files and formatting the file so that it has an even
 *     amount of rows and columns.
 *
 *    
 *
 *****************************************************************************/

#include "mem.h"
#include "assert.h"
#include "arith40.h"
#include "file_IO.h"
#include "yPbPr_to_word.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>


/**********************************Pnm_ppm_new*********************************
 *
 * Create a new Pnm_ppm struct with the given dimensions, denominator, and
 * methods
 * Inputs:
 *         unsigned width: The number of columns in the A2Methods_UArray2 
 *         pixels 2D array
 *         unsigned height: The number of rows in the A2Methods_UArray2 pixels
 *         2D array
 *         unsigned denominator: The maximum color value in the
 *         A2Methods_UArray2 pixels 2D array
 *         A2Methods_T methods: A pointer to a A2Methods_T struct that contains
 *         the methods used for storing and handling data
 * Return: A pointer to a Pnm_ppm struct with the provided dimensions,
 *         denominator, and methods
 * Expects:
 *         A2Methods_T methods to be non-null
 * Notes:
 *         CRE if A2Methods_T methods is NULL
 *                      
 *****************************************************************************/
Pnm_ppm Pnm_ppm_new(unsigned width, unsigned height, unsigned denominator,      
                    A2Methods_T methods, int size)
{
        assert(methods != NULL);

        /* Allocating memory */
        Pnm_ppm newResultA2;
        NEW(newResultA2);

        /* Assigning initial values */
        newResultA2->pixels = methods->new(width, height, size);
        newResultA2->methods = methods;
        newResultA2->height = height;
        newResultA2->width = width;
        newResultA2->denominator = denominator;
        
        return newResultA2;
}

/**********************************trimEdges***********************************
 *
 * Turn a Pnm_ppm image with odd width and height values to an image with even
 * width and height values by trimming a row or column if necessary
 * Inputs:
 *         Pnm_ppm image: A pointer to a Pnm_ppm struct that contains the data
 *         of the original image
 *         A2Methods_T methods: A pointer to a A2Methods_T struct that contains
 *         the methods used for storing and handling data
 * Return: A pointer to a Pnm_ppm struct with an even width and height value
 *         and the same pixels as Pnm_ppm image although the edges may be
 *         trimmed if the original width or height was dd
 * Expects:
 *         none
 * Notes:
 *         The memory allocated for trimmedEdges must be deallocated elsewhere
 *         using Pnm_ppmfree
 *                      
 *****************************************************************************/
Pnm_ppm trimEdges(Pnm_ppm image, A2Methods_T methods)
{
        /* Get the non-odd value of width and height */
        unsigned newWidth = isOdd(image->width);
        unsigned newHeight = isOdd(image->height);

        Pnm_ppm trimmedEdges = Pnm_ppm_new(newWidth, newHeight,  
                                           image->denominator, methods,
                                           sizeof(struct Pnm_rgb));
        
        methods->map_default(image->pixels, copyApply, trimmedEdges);

        return trimmedEdges;
}

/**********************************isOdd***************************************
 *
 * Determines if provided number is odd
 * Inputs:
 *         unsigned num: The number that will be checked
 * Return: The value of unsigned num if it was even or the numeric value that
 *         is one less than the provided number if odd
 * Expects:
 *         none
 * Notes:
 *         none
 *                      
 *****************************************************************************/
int isOdd(unsigned num)
{
        if (num % 2 == 1) {
                return (num - 1);
        }
        
        return num;
}

/**********************************copyApply***********************************
 *
 * Put an element from the original image to the same position in the result 
 * image
 * Inputs:
 *         int col: The column position of elem in A2Methods_UArray2 arr
 *         int row: The row position of elem in A2Methods_UArray2 arr
 *         A2Methods_UArray2 arr: A 2D array that contains elem at position
 *         (col, row)
 *         void *elem: The element at location (col, row) in 
 *         A2Methods_UArray2 arr that is put into the result Pnm_ppm struct
 *         passed in through void *cl
 *         void *cl: Closure pointer that in this case contains a pointer to a
 *         Pnm_ppm struct that will be used to store elem
 * Return: none
 * Expects:
 *         none
 * Notes:
 *         This function is called in trimEdges to copy over data while
 *         traversing through a A2Methods_UArray2 array
 *                      
 *****************************************************************************/
void copyApply(int col, int row, A2Methods_UArray2 arr, void *elem, void *cl)
{
        /* Accessing output image */
        Pnm_ppm image = cl;

        /* Copy elements within desired range */
        if (col < image->methods->width(image->pixels) && 
            row < image->methods->height(image->pixels)) {

                /* Accessing location where elem needs to be put in result */
                struct Pnm_rgb *currLoc = image->methods->at(image->pixels, 
                                                             col, row);

                /* Placing element in correct location */
                *currLoc = *(struct Pnm_rgb *)elem;
        }

        (void) arr;
}

/**********************************readWord************************************
 *
 * Read in four bytes from a file and store them in one four byte variable
 * Inputs:
 *         FILE *input: The file that intput is read from
 * Return: A 32-bit variable containing the bytes that were read in
 * Expects:
 *         FILE *input to be non-null
 * Notes:
 *         CRE if FILE *input is null
 *                      
 *****************************************************************************/
uint32_t readWord(FILE *input)
{
        /* Make sure there is input */
        assert(input != NULL);

        /* Reading in four bytes */
        uint32_t byte1 = readChar(input);
        uint32_t byte2 = readChar(input);
        uint32_t byte3 = readChar(input);
        uint32_t byte4 = readChar(input);

        uint32_t word = packReverseOrder(byte1, byte2, byte3, byte4);
        return word;
}

/**********************************readWord************************************
 *
 * Read in a byte from a file
 * Inputs:
 *         FILE *input: The file that intput is read from
 * Return: A 32-bit variable containing the byte that was read in
 * Expects:
 *         byte that is read in to be non-null
 * Notes:
 *         CRE if uint32_t byte is null
 *         It has been ensured in readWord that FILE *input is non-null
 *                      
 *****************************************************************************/
uint32_t readChar(FILE *input)
{
        uint32_t byte = getc(input);
        assert(byte != (uint32_t) EOF);

        return byte;
}