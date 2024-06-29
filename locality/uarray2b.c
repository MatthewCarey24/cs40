
/******************************************************************************
 *
 *                               uarray2b.c
 *
 *     Assignment: locality
 *     Authors:    Marten Tropp and Matt Carey
 *     Date:       10/10/2023
 *      
 *     The purpose of this file is to implement the UArray2b ADT. This ADT 
 *     serves as a 2 dimensional array that supports block major mapping. The 
 *     block structure can be imagined as an outer 2D arrays of smaller 2D 
 *     arrays called blocks, of a given block size. Elements indices are still 
 *     represented by their column and row in the greater 2D array, but when 
 *     mapping block major maps through elements by the block. 
 *    
 *
 *****************************************************************************/
#include "stdlib.h"
#include "uarray2b.h"
#include "uarray2.h"
#include "uarray.h"
#include "mem.h"
#include "math.h"
#include "assert.h"

/******************************UArray2b_T**************************************
 *
 * Blocked two dimensional array
 * Stores:
 *         int width: The width of the array
 *         int height: The height of the array
 *         int blocksize: The size of a block in the array
 *         int size: The size of an element in the array
 *         UArray2_T data: The two dimensional array that stores the blocks
 *                      
 *****************************************************************************/
struct UArray2b_T {
        int width;
        int height;
        int blocksize;
        int size;
        UArray2_T data;
};

typedef struct UArray2b_T *T;

UArray2_T allocateBlocks(int width, int height, int blocksize, int size);

/******************************UArray2b_new************************************
*
 * Allocate a new instance of a UArray2b
 * Inputs:
 *         int width: integer describing the number of cells in the width of 
 *         the 2D array
 *         int height: integer describing the number of cells in the height of 
 *         the 2D array
 *         int size: An int representing the size of each element in bytes. 
 *         int blocksize: an integer representing the number of cells across
 *         and down that each block is.
 * Return: A pointer to a UArray2b_T struct that contains the information given
 *         by the caller 
 * Expects:
 *         width and height to be greater than or equal to 0, size and 
 *         blocksize to be greater than or equal to 1.
 * Notes:
 *         CRE if width or height is less than 0
 *         CRE if size or blocksize is less than 1
 *                      
 *****************************************************************************/
extern T UArray2b_new (int width, int height, int size, int blocksize)
{
        /* Make sure provided values are correct */
        assert(width >= 0 && height >= 0 && size >= 1 && blocksize >= 1);

        /* Allocate memory for a UArray2b_T */
        T curr;
        NEW(curr);

        /* Assign variables initial values */
        curr->blocksize = blocksize;
        curr->height = height;
        curr->width = width;
        curr->size = size;

        /* Allocate memory for storage */
        UArray2_T data = NULL;
        data = allocateBlocks(width, height, blocksize, size);
        curr->data = data;

        return curr;
}

/***************************UArray2b_new_64k_block*****************************
 * Allocate a new instance of a UArray2b, with a block size as large as 
 * possible to fit within 64KB of RAM
 * Inputs:
 *         int width: integer describing the number of cells in the width of 
 *         the 2D array
 *         int height: integer describing the number of cells in the height of 
 *         the 2D array
 *         int size: An int representing the size of each element in bytes. 
 * Return: A pointer to a UArray2b_T struct that contains the information given
 *         by the caller, with a block size as large as possible to fit within 
 *         64KB of RAM
 * Expects:
 *         width and height and size to be greater than or equal to 0
 * Notes:
 *         CRE if width, size or height is less than 0
 *         If a single cell will not fit within 64KB of RAM, blocksize is set
 *         to 1. 
 *                      
 *****************************************************************************/
extern T UArray2b_new_64K_block(int width, int height, int size)
{
        /* Make sure provided values are correct */
        assert(width > 0 && height > 0 && size > 0);

        /* Allocate memory for a UArray2b_T */
        T curr;
        NEW(curr);

        /* Assign variables initial values */
        if (size >= 1024) {
                curr->blocksize = 1;
        }
        else {
                curr->blocksize = sqrt(65536 / size);
        }
        curr->height = height;
        curr->width = width;
        curr->size = size;

        /* Allocate memory for storage */
        UArray2_T data = NULL;
        data = allocateBlocks(width, height, curr->blocksize, size);
        curr->data = data;

        return curr;
}

/***************************UArray2b_free**************************************
 *
 * Free all the heap allocated data within a UArray2b
 * Inputs:
 *         T *array2b: a pointer to a UArray2b_T struct that you wish to free  
 * Return: void
 * Expects:
 *         The argumement to not be null and to be to a UArray2b_T that is not 
 *         a null pointer.
 * Notes:
 *         CRE if a null pointer is passed to it, or if the UArray2b passed is 
 *         a null pointer. 
 *                      
 *****************************************************************************/
extern void UArray2b_free (T *array2b)
{
        /* Make sure provided value is correct */
        assert(array2b != NULL && *array2b != NULL); 

        /* Avoid freeing NULL */
        if ((*array2b)->data != NULL) {
                UArray2_T data = (*array2b)->data;

                /* Free every block in data */
                for (int row = 0; row < UArray2_height(data); row++) {
                        for (int col = 0; col < UArray2_width(data); col++) {
                                UArray_free(&(*((UArray_T *)UArray2_at(data, 
                                                                       col, 
                                                                       row))));
                        }
                }

                UArray2_free(&data);
        }
        
        FREE(*array2b);
}

/***************************UArray2b_width*************************************
 *
 * Get the width of the UArray2b
 * Inputs:
 *         T array2b: a UArray2b_T struct who's width you want
 * Return: an integer representing the width of the array
 * Expects:
 *         The given UArray2b to not be a null pointer
 * Notes:
 *         CRE if the UArray2b passed is a null pointer. 
 *                      
 *****************************************************************************/
extern int UArray2b_width (T array2b)
{
        /* Make sure provided value is correct */
        assert(array2b != NULL); 

        return array2b->width;
}

/***************************UArray2b_height************************************
 *
 * Get the height of the UArray2b
 * Inputs:
 *         T array2b: a UArray2b_T struct who's height you want
 * Return: an integer representing the height of the array
 * Expects:
 *         The given UArray2b to not be a null pointer
 * Notes:
 *         CRE if the UArray2b passed is a null pointer. 
 *                      
 *****************************************************************************/
extern int UArray2b_height (T array2b)
{
        /* Make sure provided value is correct */
        assert(array2b != NULL); 

        return array2b->height;
}

/***************************UArray2b_size**************************************
 *
 * Get the size of the elements in a UArray2b
 * Inputs:
 *         T array2b: a UArray2b_T struct who's element size you want
 * Return: an integer representing the size of the elements in the array
 * Expects:
 *         The given UArray2b to not be a null pointer
 * Notes:
 *         CRE if the UArray2b passed is a null pointer. 
 *                      
 *****************************************************************************/
extern int UArray2b_size (T array2b)
{
        /* Make sure provided value is correct */
        assert(array2b != NULL); 
        
        return array2b->size;
}

/***************************UArray2b_blocksize*********************************
 *
 * Get the blocksize of the UArray2b
 * Inputs:
 *         T array2b: a UArray2b_T struct who's blocksize you want
 * Return: an integer representing the width and height of the blocks in an 
 *         array
 * Expects:
 *         The given UArray2b to not be a null pointer
 * Notes:
 *         CRE if the UArray2b passed is a null pointer. 
 *                      
 *****************************************************************************/
extern int UArray2b_blocksize(T array2b)
{
        /* Make sure provided value is correct */
        assert(array2b != NULL); 

        return array2b->blocksize;
}

/***************************UArray2b_at****************************************
 *
 * return a pointer to the cell in the given column and row, for reading and 
 * writing
 * Inputs:
 *         T array2b: a UArray2b_T struct who's width you want
 *         int column: integer representing the column of the element you want 
 *                     to access
 *         int row: integer representing the row of the element you want 
 *                     to access
 * Return: void pointer to the element in the array at the given column and row
 * Expects:
 *         The given UArray2b to not be a null pointer
 *         row and width to be in bounds; ie column is greater than 0 and less 
 *         than the width of the UArray2b, row is greater than 0 and less than 
 *         the height of the UArray2b.
 * Notes:
 *         CRE if the UArray2b passed is a null pointer. 
 *         CRE if column is less than 0 or greater than or equal to the width 
 *         of the UArray2b, or if row is less than 0 or greater than or equal 
 *         to the height of the UArray2b. 
 *                      
 *****************************************************************************/
extern void *UArray2b_at(T array2b, int column, int row)
{
        /* Make sure provided values are correct */
        assert(array2b != NULL);
        assert(column >= 0 && column < array2b->width);
        assert(row >= 0 && row < array2b->height);

        /* Access block where the element is located */
        int bsize = array2b->blocksize;
        UArray_T *curr = UArray2_at(array2b->data, column / bsize, 
                                    row / bsize);
        assert(curr != NULL);

        /* Calculate index of element and return it */
        int index = bsize * (column % bsize) + (row % bsize); 
        return UArray_at(*curr, index);
}

/***************************UArray2b_map***************************************
 *
 * iterate through the entire UArray2b. visits every cell in one block before 
 * moving to another block
 * Inputs:
 *         T array2b: a UArray2b_T struct who's width you want
 *         void apply(int col, int row, T array2b, void *elem, void *cl): 
 *                     function to apply to every element as they are visited
 *         void *cl: void pointer to the closure
 * Return: void
 * Expects:
 *         The given UArray2b to not be a null pointer
 * Notes:
 *         CRE if the UArray2b passed is a null pointer. 
 *         maps through blocks as row-major 
 *         maps through cells within each block as row-major
 *                      
 *****************************************************************************/
extern void UArray2b_map(T array2b, void apply(int col, int row, T array2b, 
                         void *elem, void *cl), void *cl)
{
        /* Make sure provided value is correct */
        assert(array2b != NULL); 
        int height = array2b->height;
        int width = array2b->width;
        int bSize = array2b->blocksize;

        /* Iterate through the start of each block */
        for (int bRow = 0; bRow < height; bRow += bSize) {
                for (int bCol = 0; bCol < array2b->width; bCol += bSize) {

                        /* Iterate through the block */
                        for (int row = bRow; row < bRow + bSize && 
                                           row < height; row++) {      

                                for (int col = bCol; col < bCol + bSize && 
                                                     col < width; col++) {
                                                        
                                        /* Calling apply on the element */
                                        apply(col, row, array2b, 
                                              UArray2b_at(array2b, col, row), 
                                              cl);
                                }
                        }

                }
        }
}

/***************************allocateBlocks*************************************
 *
 * Initializes the UArray2_T that represents our UArray2b and allocate the 
 * memory for each block, storing it in the UArray2
 * Inputs:
 *         int width: integer describing the number of cells in the width of 
 *         the 2D array
 *         int height: integer describing the number of cells in the height of 
 *         the 2D array
 *         int size: An int representing the size of each element in bytes. 
 *         int blocksize: an integer representing the number of cells across
 *         and down that each block is.
 * Return: UaArray2_T of given dimensions and blcok size, with a UArray in each
 *         block
 * Expects:
 *         width and height to be greater than or equal to 0, size and 
 *         blocksize to be greater than or equal to 1.
 * Notes:
 *         Expecations have been checked in the function that calls this one.
 *                      
 *****************************************************************************/
UArray2_T allocateBlocks(int width, int height, int blocksize, int size)
{
        /* Allocating memory for a UArray2_T */
        UArray2_T data = NULL;
        data = UArray2_new(ceil((double) width / blocksize), 
                           ceil((double) height / blocksize), 
                           sizeof(UArray_T));

        int data_height = UArray2_height(data);
        int data_width = UArray2_width(data);
        
        /* Allocate memory for the blocks and store them in data */
        UArray_T *elem = NULL;
        for (int row = 0; row < data_height; row++) {
                for (int col = 0; col < data_width; col++) {
                        elem = UArray2_at(data, col, row);
                        *elem = UArray_new(blocksize * blocksize, size);
                }
        }

        return data;
}