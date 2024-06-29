/******************************************************************************
 *
 *                               bitpack.c
 *
 *     Assignment: arith
 *     Authors:    Marten Tropp and Matt Carey
 *     Date:       10/24/2023
 *      
 *     The purpose of this file is to implement the Bitpack interface. This
 *     interface is used to allow its user to modify and access information in
 *     a 64-bit integer value that can be signed or unsigned.
 *    
 *
 *****************************************************************************/

#include "bitpack.h"
#include "stdbool.h"
#include <stdlib.h>
#include <assert.h>
#include <except.h>
#include <stdint.h>
#include "stdio.h"
#include <inttypes.h>

/* Error message for when the value does not fit */
Except_T Bitpack_Overflow = { "Overflow packing bits" };

static uint64_t makeGetMask(unsigned width, unsigned lsb);
static uint64_t makeClearMask(unsigned width, unsigned lsb);

/******************************Bitpack_fitsu***********************************
*
 * Determine whether an unsigned word can fit in a specific amount of bits 
 * provided.
 * Inputs:
 *         uint64_t n: word that will be determined if it can fit
 *         unsigned width: number of bits that can be used to store the word
 * Return: Boolean indicating whether unsigned width bits can fit uint64_t n
 * Expects:
 *         unsigned width to be between 0 and 64 (including 64, but not 0)
 * Notes:
 *         CRE if unsigned width is not between 0 and 64
 *                      
 *****************************************************************************/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        /* A 64-bit number will always fit in 64 or more bits */
        if (width >= 64) {
                return true;
        }

        /* Calculate range of numbers that can be represented */
        uint64_t range = (uint64_t) 1 << width;

        return (n < range);
}

/******************************Bitpack_fitss***********************************
*
 * Determine whether a signed word can fit in a specific amount of bits 
 * provided.
 * Inputs:
 *         int64_t n: word that will be determined if it can fit
 *         unsigned width: number of bits that can be used to store the word
 * Return: Boolean indicating whether unsigned width bits can fit int64_t n
 * Expects:
 *         unsigned width to be between 0 and 64 (including 64, but not 0)
 * Notes:
 *         CRE if unsigned width is not between 0 and 64
 *                      
 *****************************************************************************/
bool Bitpack_fitss(int64_t n, unsigned width)
{
        /* A 64-bit number will always fit in 64 or more bits */
        if (width >= 64) {
                return true;
        }
        /* Can not store signed value in 0 bits */
        else if (width == 0) {
                return false;
        }

        /* Getting range of numbers that can be stored */
        int64_t lowBound = 0 - ((int64_t) 1 << (width - 1));
        int64_t upBound =  (int64_t) 1 << (width - 1);

        return (n >= lowBound && n < upBound);
}

/******************************Bitpack_getu************************************
*
 * Extract a value of a specific size at a specific location from an unsigned
 * word.
 * Inputs:
 *         uint64_t word: word that the value will be extracted from
 *         unsigned width: width of the value that is desired
 *         unsigned lsb: location of least significant bit of value that is
 *         desired
 * Return: uint64_t of width unsigned width and has least significant bit at
 *         lsb in uint64_t word
 * Expects:
 *         unsigned width to be between 0 and 64 (including 64, but not 0)
 *         the sum of unsigned width and unsigned lsb to be less than equal to
 *         64
 * Notes:
 *         CRE if unsigned width is not between 0 and 64
 *         CRE if sum of unsigned width and unsigned lsb is not less than equal
 *         to 64
 *                      
 *****************************************************************************/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        assert(width + lsb <= 64);

        /* Return zero if desired width was zero */
        if (width == 0) {
                return 0;
        }

        /* Creating a mask to extract desired values */
        uint64_t mask = makeGetMask(width, lsb);

        /* Getting desired values */
        uint64_t result = word & mask;
        result = result >> lsb;

        return result;
}

/******************************Bitpack_gets************************************
*
 * Extract a value of a specific size at a specific location from a signed
 * word.
 * Inputs:
 *         int64_t word: word that the value will be extracted from
 *         unsigned width: width of the value that is desired
 *         unsigned lsb: location of least significant bit of value that is
 *         desired
 * Return: int64_t of width unsigned width and has least significant bit at
 *         lsb in int64_t word
 * Expects:
 *         unsigned width to be between 0 and 64 (including 64, but not 0)
 *         the sum of unsigned width and unsigned lsb to be less than equal to
 *         64
 * Notes:
 *         CRE if unsigned width is not between 0 and 64
 *         CRE if sum of unsigned width and unsigned lsb is not less than equal
 *         to 64
 *                      
 *****************************************************************************/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        assert(width + lsb <= 64);

        /* Return zero if desired width was zero */
        if (width == 0) {
                return 0;
        }

        /* Creating a mask to extract desired values */
        uint64_t mask = makeGetMask(width, lsb);

        /* Getting desired values */
        int64_t result = word & mask;
        result = result << (64 - lsb - width);
        
        /* Result is signed so right shift carries the sign bit across */
        result = result >> (64 - width);

        return result;
}

/******************************Bitpack_newu************************************
*
 * Insert a value of a specific size at a specific location into an unsigned
 * word.
 * Inputs:
 *         uint64_t word: word that the value will be put into
 *         unsigned width: width of the value that is inserted
 *         unsigned lsb: location of least significant bit of value that is
 *         inserted
 *         uint64_t value: word that will be inserted into uint64_t word
 * Return: uint64_t that has uint64_t value inserted into it at the location
 *         determined by unsigned width and unsigned lsb but otherwise is the
 *         same as word
 * Expects:
 *         unsigned width to be between 0 and 64 (including 64, but not 0)
 *         unsigned width to be large enough that uint64_t value can fit in 
 *         width number of bits
 *         the sum of unsigned width and unsigned lsb to be less than equal to
 *         64
 * Notes:
 *         CRE if unsigned width is not between 0 and 64
 *         CRE if sum of unsigned width and unsigned lsb is not less than equal
 *         to 64
 *         checking if unsigned width is large enough for uint64_t value is 
 *         checked using Bitpack_fitsu
 *                      
 *****************************************************************************/
uint64_t Bitpack_newu(uint64_t word, unsigned width, 
                      unsigned lsb, uint64_t value)
{
        assert(width <= 64);
        assert(width + lsb <= 64);

        /* Raise a checked runtime error if value does not fit in width */
        if(!Bitpack_fitsu(value, width)) {
                RAISE(Bitpack_Overflow);
        }

        /* Creating a masks to extract desired values */
        uint64_t mask_full = makeClearMask(width, lsb);
        
        /* Original values in word in desired location cleared */
        word &= mask_full;

        /* Shifts value to the right "index" */
        value = value << lsb;

        /* Inserting value to word */
        word |= value;

        return word;
}

/******************************Bitpack_news************************************
*
 * Insert a value of a specific size at a specific location into a signed word.
 * Inputs:
 *         uint64_t word: word that the value will be put into
 *         unsigned width: width of the value that is inserted
 *         unsigned lsb: location of least significant bit of value that is
 *         inserted
 *         int64_t value: word that will be inserted into uint64_t word
 * Return: uint64_t that has int64_t value inserted into it at the location
 *         determined by unsigned width and unsigned lsb but otherwise is the
 *         same as word
 * Expects:
 *         unsigned width to be between 0 and 64 (including 64, but not 0)
 *         unsigned width to be large enough that int64_t value can fit in 
 *         width number of bits
 *         the sum of unsigned width and unsigned lsb to be less than equal to
 *         64
 * Notes:
 *         CRE if unsigned width is not between 0 and 64
 *         CRE if sum of unsigned width and unsigned lsb is not less than equal
 *         to 64
 *         checking if unsigned width is large enough for int64_t value is 
 *         checked using Bitpack_fitss
 *                      
 *****************************************************************************/
uint64_t Bitpack_news(uint64_t word, unsigned width, 
                      unsigned lsb, int64_t value)
{       
        assert(width <= 64);
        assert(width + lsb <= 64);

       /* Raise a checked runtime error if value does not fit in width */
        if(!Bitpack_fitss(value, width)) {
                RAISE(Bitpack_Overflow);
        }
        
        /* Creating a masks to clear desired field */
        uint64_t mask_full = makeClearMask(width, lsb);

        /* Original values in word in desired location cleared */
        word &= mask_full;

        /* Shifts value to the right "index" */
        value = value << lsb;

        /* Getting rid of excessive ones in the signed value */
        uint64_t mask_left = ~0;
        mask_left = mask_left << (lsb + width);
        mask_left = ~mask_left;
        value &= mask_left;

        /* Inserting value to word */
        word |= value;

        return word;
}

/******************************makeClearMask***********************************
*
 * Create a mask that is used to clear values in desired location of specific
 * width in a word of 64-bits.
 * Inputs:
 *         unsigned width: width of the value that is cleared
 *         unsigned lsb: location of least significant bit of value that is
 *         cleared
 * Return: uint64_t that can be used to clear section in a word of width and
 *         with its least significant bit being at lsb
 * Expects:
 *         none
 * Notes:
 *         This is a helper function that gets called in functions that insert
 *         a value into a specific location in a uint64_t.
 *                      
 *****************************************************************************/
static uint64_t makeClearMask(unsigned width, unsigned lsb)
{
        /* Make a mask of all ones left of field of interest*/
        uint64_t mask_left = ~0;
        mask_left = mask_left << (lsb + width);

        /* Make a mask of all ones right of field of interest*/
        uint64_t mask_right = ~0;
        mask_right = mask_right >> (64 - lsb);

        /* Combining the masks, 1s except field of interest */
        uint64_t mask_full = mask_left | mask_right;

        return mask_full;
}

/******************************makeGetMask*************************************
*
 * Create a mask that is used to get values in desired location of specific
 * width in a word of 64-bits.
 * Inputs:
 *         unsigned width: width of the value that wants to be accessed
 *         unsigned lsb: location of least significant bit of value that can
 *         be accessed using this
 * Return: uint64_t that can be used to clear every section in a word except to
 *         one that will be accessed
 * Expects:
 *         none
 * Notes:
 *         This is a helper function that gets called in functions that get
 *         a value from a specific location in a uint64_t.
 *                      
 *****************************************************************************/
static uint64_t makeGetMask(unsigned width, unsigned lsb)
{
        /* Create a mask that is used to extract values from a location */
        uint64_t mask = ~0;
        mask = mask >> (64 - width);
        mask = mask << lsb;

        return mask;
}