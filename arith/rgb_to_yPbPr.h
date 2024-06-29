/******************************************************************************
 *
 *                               rgb_to_yPbPr.h
 *
 *     Assignment: arith
 *     Authors:    Marten Tropp and Matt Carey
 *     Date:       10/24/2023
 *      
 *     The purpose of this file is to declare the functions associated with
 *     converting RGB values in a ppm image into the component video color 
 *     space for the arith program that compresses and decompresses images.
 *     
 *
 *****************************************************************************/

#include "arith_structs.h"
#include "pnm.h"
#include "a2methods.h"


#ifndef RGB_TO_YPBPR_H
#define RGB_TO_YPBPR_H

A2Methods_UArray2 mapToDoublePoint(Pnm_ppm image, A2Methods_T methods);
void convertToDoublePointApply(int col, int row, A2Methods_UArray2 arr,
                               void *elem, void *cl);
struct Pnm_yPbPr convertToDoublePoint(unsigned red, unsigned green, 
                                      unsigned blue, unsigned denominator);

#endif