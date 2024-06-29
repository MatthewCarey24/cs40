
/******************************************************************************
 *
 *                               our_math.c
 *
 *     Assignment: arith
 *     Authors:    Marten Tropp and Matt Carey
 *     Date:       10/24/2023
 *      
 *     The purpose of this file is to implement the functions associated with
 *     computing common mathematical equations in arith.
 *
 *****************************************************************************/

#include "our_math.h"
#include "assert.h"
#include <stdlib.h>

/**********************************averageOfFour*******************************
 *
 * Get the average of four numbers provided
 * Inputs:
 *         double num1: The first number used in the calculation
 *         double num2: The second number used in the calculation
 *         double num3: The third number used in the calculation
 *         double num4: The fourth number used in the calculation
 * Return: The value of the average of the provided files as a double
 * Expects:
 *         none
 * Notes:
 *         none
 *                      
 *****************************************************************************/
double averageOfFour(double num1, double num2, double num3, double num4) 
{
        return (num1 + num2 + num3 + num4) / 4.0;
}

/**********************************setRange************************************
 *
 * Check if a number is within provided bounds and change its value if needed
 * Inputs:
 *         double number: The value that is being checked
 *         double lowerBound: The smallest value the number can have
 *         double higherBound: The greatest value the number can have
 * Return: The value of the number if it was in provided bounds; otherwise, the
 *         value of one of the bounds, whichever one it was closest to
 * Expects:
 *         lowerBound to have a smaller value than higherBound
 * Notes:
 *         CRE if lowerBound is greater than higherBound
 *                      
 *****************************************************************************/
double setRange(double number, double lowerBound, double higherBound)
{
        assert(lowerBound <= higherBound);

        if (number < lowerBound) {
                number = lowerBound;
        }
        else if (number > higherBound) {
                number = higherBound;
        }

        return number;
}