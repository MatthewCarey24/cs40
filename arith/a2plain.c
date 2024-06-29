/******************************************************************************
 *
 *                               a2plain.c
 *
 *     Assignment: arith
 *     Authors:    Marten Tropp and Matt Carey
 *     Date:       10/24/2023
 *      
 *     The purpose of this file is to implement the methods of the A2Methods 
 *     suite using UArray2. It provides the polymorphism to the A2Methods suite
 *     to be able to handle both types of 2 dimenional array. 
 *
 *    
 *
 *****************************************************************************/

#include <string.h>
#include <a2plain.h>
#include "uarray2.h"
#include "assert.h"

typedef A2Methods_UArray2 A2;   // private abbreviation

/*********************************new******************************************
 *
 * Call the UArray2_new function with the provided parameters
 * Inputs:
 *         int width: The number of columns in the UArray2_T
 *         int height: The number of rows in the UArray2_T
 *         int size: The size of a single element in the UArray2_T in bytes
 * Return: an A2Methods_UArray2 instance
 * Expects:
 *         int width and int height to be at least zero and int size to be at
 *         least one
 * Notes:
 *         Expectations are checked in the UArray2_new function that is called
 *                      
 *****************************************************************************/
static A2 new(int width, int height, int size)
{
        return UArray2_new(width, height, size);
}

/*********************************new_with_blocksize***************************
 *
 * Call the UArray2_new function with the provided parameters
 * Inputs:
 *         int cols: The number of columns in the UArray2_T
 *         int rows: The number of rows in the UArray2_T
 *         int size: The size of a single element in the UArray2_T in bytes
 *         int blocksize: The number of blocks in the UArray2_T which is
 *         trivial since it is always one for a UArray2_T, but any value can be
 *         passed in for it as long as it is an integer
 * Return: an A2Methods_UArray2 instance
 * Expects:
 *         int width and int height to be at least zero and int size to be at
 *         least one
 * Notes:
 *         Expectations are checked in the UArray2_new function that is called
 *                      
 *****************************************************************************/
static A2 new_with_blocksize(int width, int height, int size, int blocksize)
{
        (void) blocksize;
        return UArray2_new(width, height, size);
}

/*********************************a2free***************************************
 *
 * Call the UArray2_free function with the provided parameter to free any data
 * allocated for A2 *array2p
 * Inputs:
 *         A2 *array2p: A pointer to a A2Methods_UArray2 that, in this case, is
 *         a UArray2_T that will be freed
 * Return: none
 * Expects:
 *         A2 *array2p to be non-null
 * Notes:
 *         The value of A2 *array2p is checked in UArray2_free
 *                      
 *****************************************************************************/
static void a2free(A2 * array2p)
{
        UArray2_free((UArray2_T *) array2p);
}

/*********************************width****************************************
 *
 * Call the UArray2_width function with the provided parameter to get the
 * width of A2 array2
 * Inputs:
 *         A2 array2: A pointer to a A2Methods_UArray2 that, in this case, is
 *         a UArray2_T
 * Return: Width of A2 array2 as an integer
 * Expects:
 *         A2 array2 to be non-null
 * Notes:
 *         The value of A2 array2 is checked in UArray2_width
 *                      
 *****************************************************************************/
static int width(A2 array2)
{
        return UArray2_width(array2);
}

/*********************************height***************************************
 *
 * Call the UArray2_height function with the provided parameter to get the
 * height of A2 array2
 * Inputs:
 *         A2 array2: A pointer to a A2Methods_UArray2 that, in this case, is
 *         a UArray2_T
 * Return: Height of A2 array2 as an integer
 * Expects:
 *         A2 array2 to be non-null
 * Notes:
 *         The value of A2 array2 is checked in UArray2_height
 *                      
 *****************************************************************************/
static int height(A2 array2)
{
        return UArray2_height(array2);
}

/*********************************size*****************************************
 *
 * Call the UArray2_size function with the provided parameter to get the size
 * of an element in A2 array2
 * Inputs:
 *         A2 array2: A pointer to a A2Methods_UArray2 that, in this case, is
 *         a UArray2_T
 * Return: Size of an element in A2 array2 as an integer
 * Expects:
 *         A2 array2 to be non-null
 * Notes:
 *         The value of A2 array2 is checked in UArray2_size
 *                      
 *****************************************************************************/
static int size(A2 array2)
{
        return UArray2_size(array2);
}

/*********************************blocksize************************************
 *
 * Call the UArray2_size function with the provided parameter to get the
 * blocksize of A2 array2
 * Inputs:
 *         A2 array2: A pointer to a A2Methods_UArray2 that, in this case, is
 *         a UArray2_T
 * Return: 1 since the blocksize of a UArray2_T is always 1
 * Expects:
 *         A2 array2 to be non-null
 * Notes:
 *         CRE if A2 array2 is null
 *                      
 *****************************************************************************/
static int blocksize(A2 array2)
{
        /* Make sure a non-null A2 was passed */
        assert(array2 != NULL);
        
        return 1;
}

/*********************************at*******************************************
 *
 * Call the UArray2_at function with the provided parameter to get a pointer to
 * the element at provided location in A2 array2
 * Inputs:
 *         A2 array2: A pointer to a A2Methods_UArray2 that, in this case, is
 *         a UArray2_T
 *         int i: The column number of the element in the UArray2_T
 *         int j: The row number of the element in the UArray2_T
 * Return: A pointer to an A2Methods_Object at provided location
 * Expects:
 *         A2 array2 to be non-null, and int i and int j to be in the
 *         dimensions of A2 array2
 * Notes:
 *         The value of A2 array2, int i, and int j are checked in UArray2_size
 *                      
 *****************************************************************************/
static A2Methods_Object *at(A2 array2, int i, int j)
{
        return UArray2_at(array2, i, j);
}

typedef void applyfun(int i, int j, UArray2_T array2, void *elem, void *cl);

/***************************map_row_major**************************************
 *
 * Call the UArray2_map_row_major function with the given A2Methods_UArray2, 
 * apply 
 * Inputs:
 *         A2 uarray2: a A2Methods_UArray2 struct to pass to map through
 *         A2Methods_applyfun apply: A2Methods_applyfun function to pass to map
 *                                   as the apply function
 *         void *cl: void pointer to the closure to pass to map row major
 * Return: void
 * Expects:
 *         A2 uarray2 to not be null
 * Notes:
 *         the function it calls will CRE if the A2 passed is a null pointer. 
 *                      
 *****************************************************************************/
static void map_row_major(A2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_row_major(uarray2, (applyfun *) apply, cl);
}

/***************************map_col_major**************************************
 *
 * Call the UArray2_map_col_major function with the given A2Methods_UArray2, 
 * apply 
 * Inputs:
 *         A2 uarray2: a A2Methods_UArray2 struct to pass to map through
 *         A2Methods_applyfun apply: A2Methods_applyfun function to pass to map
 *                                   as the apply function
 *         void *cl: void pointer to the closure to pass to map row major
 * Return: void
 * Expects:
 *         A2 uarray2 to not be null
 * Notes:
 *         the function it calls will CRE if the A2 passed is a null pointer.
 *                      
 *****************************************************************************/
static void map_col_major(A2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_col_major(uarray2,(applyfun *) apply, cl);
}

/***************************small_closure**************************************
 *
 * we did not implement this function, so we do not provide a function contract
 *
 *****************************************************************************/
struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void                    *cl;
};

/***************************apply_small****************************************
 *
 * we did not implement this function, so we do not provide a function contract
 *                      
 *****************************************************************************/
static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)uarray2;
        cl->apply(elem, cl->cl);
}

/**************************small_map_row_major*********************************
 *
 * we did not implement this function, so we do not provide a function contract
 *                      
 *****************************************************************************/
static void small_map_row_major(A2 a2, A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}


/**************************small_map_col_major*********************************
 *
 * we did not implement this function, so we do not provide a function contract
 *                      
 *****************************************************************************/
static void small_map_col_major(A2 a2, A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}


static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize,
        a2free,
        width,
        height,
        size,
        blocksize,              // blocksize
        at,
        map_row_major,          // map_default
        map_col_major,
        NULL,                   // map_block_major
        map_row_major,          // map_default
        small_map_row_major,
        small_map_col_major,
        NULL,                   // small_map_block_major
        small_map_row_major,
};

// finally the payoff: here is the exported pointer to the struct

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;