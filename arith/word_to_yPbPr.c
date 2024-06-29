/******************************************************************************
 *
 *                               word_to_yPbPr.c
 *
 *     Assignment: arith
 *     Authors:    Marten Tropp and Matt Carey
 *     Date:       10/24/2023
 *      
 *     The purpose of this file is to implement the functions associated with
 *     unpacking values from a 32-bit codeword and extracting the values as  
 *     component video color space.
 *     
 *
 *****************************************************************************/

#include "word_to_yPbPr.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "bitpack.h"
#include "our_math.h"
#include "arith40.h"
#include "file_IO.h"

#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>


/******************************floatCosineCoeff********************************
 *
 * Structure that contains the four brightness values from a 2x2 block in a 2D
 * array as float-point representations
 * Stores:
 *         double a: The average brightness of the image
 *         double b: The degree to which the image gets brighter as we move
 *         from top to bottom
 *         double c: The degree to which the image gets brighter as we move
 *         from left to right
 *         double d: The degree to which the pixels on one diagonal are 
 *         brighter than the pixels on the other diagnoal
 *                      
 *****************************************************************************/
struct floatCosineCoeff {
        double a;
        double b;
        double c;
        double d;
};

/******************************lumaBlock***************************************
 *
 * Structure that contains the four brightness values from a 2x2 block in a 2D
 * array as float-point representations
 * Stores:
 *         double Y1: The luminance value of a pixel in the 2x2 block
 *         double Y2: The luminance value of a pixel in the 2x2 block
 *         double Y3: The luminance value of a pixel in the 2x2 block
 *         double Y4: The luminance value of a pixel in the 2x2 block
 *                      
 *****************************************************************************/
struct lumaBlock {
        double Y1;
        double Y2;
        double Y3;
        double Y4;
};

/********************************extractABCD***********************************
 *
 * Extract the binary representations of a, b, c, and d from a compressed code
 * word, and store them in a struct
 * Inputs:
 *         uint64_t word: a 32 bit integer code word with a value of width 9 
 *         packed into it with least significant bit 23, and 3 values of width 
 *         5 packed at lsb 8, 13, and 18.
 * Return: A struct cosineCoeff that contains the a, b, c, and d values that 
 *          are extracted from the code word
 * Expects:
 *         The code word has correct values packed in the correct indices: a of
 *         width 9 packed with a least significant bit 23, and d, c, and b have
 *         width of 5 and are packed in at lsb 8, 13, and 18, respectively.
 * Notes:
 *         none
 *                      
 *****************************************************************************/
struct cosineCoeff extractABCD(uint64_t word)
{
        /* Extarct the values from the 64-bit integer */
        uint64_t a = Bitpack_getu(word, 9, 23);
        int64_t b = Bitpack_gets(word, 5, 18);
        int64_t c = Bitpack_gets(word, 5, 13);
        int64_t d = Bitpack_gets(word, 5, 8);

        struct cosineCoeff abcd = {a, b, c, d};

        return abcd;
}

/********************************floatABCD***********************************
 *
 * Convert the binary representations of a, b, c, and d from a cosineCoeff into
 * their double representations 
 * Inputs:
 *         cosineCoeff abcd: struct containing the binary integer 
 *         representations of the cosine coefficients a, b, c, and d.
 * Return: A struct floatcosineCoeff that contains the a, b, c, and d values 
 *         in their float value representations
 * Expects:
 *         The abcd is not null, abcd has all of its values defined
 * Notes:
 *         CRE if abcd is null
 *                      
 *****************************************************************************/
struct floatCosineCoeff floatABCD(cosineCoeff abcd)
{
        assert(abcd != NULL);

        double a = setRange((double) abcd->a / (double) 511, 0.0, 1.0);
        double b = setRange((double) abcd->b / (double) 50, -0.3, 0.3);
        double c = setRange((double) abcd->c / (double) 50, -0.3, 0.3);
        double d = setRange((double) abcd->d / (double) 50, -0.3, 0.3);


        struct floatCosineCoeff f_abcd = {a, b, c, d};     

        return f_abcd;
}

/***********************************toLuma*************************************
 *
 * Convert the double representations of a, b, c, and d to the cooresponding 
 * luma values
 * Inputs:
 *         floatCosineCoeff f_abcd: a struct containing the a, b, c, and d 
 *                                  values of a block of pixels
 * Return: A struct lumaBlock that contains the Y1, Y2, Y3, and Y4 values that 
 *          are computed from the cosine coefficients
 * Expects:
 *         The cosineCoeff struct to not be null and to have all of its 
 *         elements to be defined
 * Notes:
 *         CRE if f_abcd is null
 *                      
 *****************************************************************************/
struct lumaBlock toLuma(floatCosineCoeff f_abcd)
{
        assert(f_abcd != NULL);
        double Y1 = f_abcd->a - f_abcd->b - f_abcd->c + f_abcd->d;
        setRange(Y1, 0.0, 1.0);
        
        double Y2 = f_abcd->a - f_abcd->b + f_abcd->c - f_abcd->d;
        setRange(Y2, 0.0, 1.0);

        double Y3 = f_abcd->a + f_abcd->b - f_abcd->c - f_abcd->d;
        setRange(Y3, 0.0, 1.0);

        double Y4 = f_abcd->a + f_abcd->b + f_abcd->c + f_abcd->d;
        setRange(Y4, 0.0, 1.0);

        struct lumaBlock lumaVals = {Y1, Y2, Y3, Y4};

        return lumaVals;
}

/*****************************storeFloatBlock**********************************
 *
 * Store the cooresponding double values to all four cells in a block
 * Inputs:
 *         A2Methods_UArray2 doublePoints: the UArray2 of doubles that we are 
 *         storing the values in.
 *         A2Methods_T methods: A pointer to the A2Methods suite that we are 
 *         using.
 *         int col: The column of the top left cell of the block we are 
 *         currently at   
 *         int row: The row of the top left cell of the block we are currently 
 *         at
 *         lumaBlock lumaVals: A struct containing all of the luma values for 
 *         the current block
 *         float chromaPb: The quantized average Pb chroma of the current block
 *         float chromaPr: The quantized average Pr chroma of the current block
 * Return: void
 * Expects:
 *         doublePoints to have space allocated, methods to not be null, col 
 *         and row to be in range within doublePoints, lumaVals to have all its
 *         values defined
 * Notes:
 *         none
 *                      
 *****************************************************************************/
void storeFloatBlock(A2Methods_UArray2 doublePoints, A2Methods_T methods, 
                     int col, int row, lumaBlock lumaVals, float chromaPb, 
                     float chromaPr)
{
        /* Accessing top left element in block */
        Pnm_yPbPr topL = methods->at(doublePoints, col, row);
        struct Pnm_yPbPr tempTopL = {lumaVals->Y1, chromaPb, chromaPr};
        *topL = tempTopL;

        /* Accessing bottom left element in block */
        Pnm_yPbPr botL = methods->at(doublePoints, col, row + 1);
        struct Pnm_yPbPr tempBotL = {lumaVals->Y2, chromaPb, chromaPr};
        *botL = tempBotL;
        
        /* Accessing top right element in block */
        Pnm_yPbPr topR = methods->at(doublePoints, col + 1, row);
        struct Pnm_yPbPr tempTopR = {lumaVals->Y3, chromaPb, chromaPr};
        *topR = tempTopR;
        
        /* Accessing bottom right element in block */
        Pnm_yPbPr botR = methods->at(doublePoints, col + 1, row + 1);
        struct Pnm_yPbPr tempBotR = {lumaVals->Y4, chromaPb, chromaPr};
        *botR = tempBotR;
}

/********************************toDoublePoint*********************************
 *
 * Build a Uarray2 with the component video space values it reads in from code 
 * words.
 * Inputs:
 *         FILE *input: A file containing "code words" that are bit packed ints
 *         containing the data of component video values.
 *         unsigned width: width of the UArray2 we wish to create.
 *         unsigned height: height of the UArray2 we wish to create.
 *         A2Methods_T methods: Pointer to the UArray2 methods suite to use
 * Return: An A2Methods_Uarray2 filled with the correct component video values
 * Expects:
 *         none
 * Notes:
 *         Allocates memory via the new_with_blocksize function that is later 
 *         freed in compress40.c by the compress40 function.
 *                      
 *****************************************************************************/
A2Methods_UArray2 toDoublePoint(FILE *input, unsigned width, unsigned height, 
                                A2Methods_T methods)
{
        A2Methods_UArray2 doublePoints = methods->new_with_blocksize(width, 
                                          height, sizeof(struct Pnm_yPbPr), 2);
        
        for (unsigned col = 0; col < width; col += 2) {
                for (unsigned row = 0; row < height; row += 2) {
                        uint32_t word = readWord(input);

                        /* Get values from rearranged word */
                        uint64_t indexPb = Bitpack_getu(word, 4, 4);
                        uint64_t indexPr = Bitpack_getu(word, 4, 0);

                        /*get a,b,c,d and convert to float values*/
                        struct cosineCoeff abcd = extractABCD(word);
                        struct floatCosineCoeff f_abcd = floatABCD(&abcd);
                        struct lumaBlock lumaVals = toLuma(&f_abcd);
                        
                        float chromaPb = Arith40_chroma_of_index(indexPb);
                        float chromaPr = Arith40_chroma_of_index(indexPr);

                        storeFloatBlock(doublePoints, methods, col, row, 
                                        &lumaVals, chromaPb, chromaPr);
                }
        }

        return doublePoints;
}