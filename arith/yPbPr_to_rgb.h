/******************************************************************************
 *
 *                               yPbPr_to_rgb.h
 *
 *     Assignment: arith
 *     Authors:    Marten Tropp and Matt Carey
 *     Date:       10/24/2023
 *      
 *     The purpose of this file is to define the functions associated with
 *     converting values in component video color space to the associated RGB 
 *     values for the arith program, which compresses and decompresses images.
 *     
 *     
 *
 *****************************************************************************/

#include <pnm.h>


#ifndef YPBPR_TO_RGB_H
#define YPBPR_TO_RGB_H

Pnm_ppm mapToRGB(A2Methods_UArray2 doubleVals, A2Methods_T methods);
void convertToRGBApply(int col, int row, A2Methods_UArray2 arr, 
                       void *elem, void *cl);
struct Pnm_rgb convertToRGB(double y, double pb, double pr);

#endif