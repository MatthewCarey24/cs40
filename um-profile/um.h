/******************************************************************************
 *
 *                                  um.h
 *
 *     Assignment: um
 *     Authors:    Marten Tropp and Matthew Carey
 *     Date:       11/20/2023
 *      
 *     The purpose of this file is to define the struct and functions 
 *     associated with the um data structure. These functions perform the 
 *     main operations of the um as well as any helper functions they need.
 *
 *    
 *
 *****************************************************************************/
#ifndef UM_INCLUDED
#define UM_INCLUDED

#include <stdint.h>
#include "segment.h"
#include "mem.h"
#include "bitpack.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>

/**********************************um_T****************************************
 *
 * Structure that contains an array of registers, a Segment_T of memory and a 
 * program counter as an int.
 * Stores:
 *         uint32_t registers[8]: array of 8 32-bit integers representing 
 *                                registers 
 *         Segment_T segments:    Segment_T object that represents the um's  
 *                                segmented memory
 *         int program_count:     Integer to track where the execution is in  
 *                                the program
 *                      
 *****************************************************************************/
typedef struct um_T {
        uint32_t registers[8];
        Segment_T segments;
        int program_count;
} *um_T;

static um_T um_new(uint32_t size);
static void um_free(um_T *um);
static void um_conditional(um_T um, int rA, int rB, int rC);
static void um_seg_load(um_T um, int rA, int rB, int rC);
static void um_seg_store(um_T um, int rA, int rB, int rC);
static void um_addition(um_T um, int rA, int rB, int rC);
static void um_multi(um_T um, int rA, int rB, int rC);
static void um_division(um_T um, int rA, int rB, int rC);
static void um_nand(um_T um, int rA, int rB, int rC);
static void um_halt(um_T um);
static void um_map_seg(um_T um, int rB, int rC);
static void um_unmap_seg(um_T um, int rC);
static void um_output(um_T um, int rC);
static void um_input(um_T um, int rC);
static void um_load_prog(um_T um, int rB, int rC);
static void um_load_val(um_T um, int rA, uint32_t val);
static bool um_is_valid_inst(um_T um);

/***********************************um_new*************************************
 *
 * Creates a new um_T structure
 * Inputs:
 *         uint32_t size: The number of 32-bit words in segment zero of the um
 * Return: A new um_T with its segment zero having the provided size
 * Expects:
 *         none
 * Notes:
 *         none
 *         Allocated memory is supposed to be deallocated using um_free
 *****************************************************************************/
static um_T um_new(uint32_t size)
{
        /* Allocating memory for the um_T variable */
        um_T um;
        NEW(um);

        /* Initialize Segment_T variable */
        Segment_T segments = Segment_new(size);

        /* Assigning values to the um_T variable */
        um->segments = segments;
        um->program_count = 0;

        /* Giving registers default values */
        for (int i = 0; i < 8; i ++) {
                um->registers[i] = 0;
        }
        
        return um;
}

/********************************um_free***************************************
 *
 * Free the memory associated with a um_T
 * Inputs:
 *         um_T *um: The um_T that will be freed
 * Return: none
 * Expects:
 *         um and *um to be non-null
 * Notes:
 *         CRE if um or *um is null
 *         Frees memory allocated in um_new
 *****************************************************************************/
static void um_free(um_T *um)
{
        /* Ensuring non-null value was passed in */
        assert(um && *um);

        /* Freeing segments if not freed already */
        if ((*um)->segments != NULL) {
                Segment_free(&(*um)->segments);
        }

        FREE(*um);
}

/********************************um_conditional********************************
 *
 * Executes conditional operation of the um
 * Inputs:
 *         um_T um:  The um that is in use
 *         int rA:   The number of register A, which is the potential 
                     destination
 *         int rB:   The number of register B, which contains the value to 
                     potentially be copied
 *         int rC:   The number of register C, which is the conditional
 * Return: none
 * Expects:
 *         um to be non-null
 *         rA, rB, and rC to be in range [0, 7]
 * Notes:
 *         CRE if um is null
 *         CRE if rA, rB, or rC is not in range [0, 7]
 *****************************************************************************/
static void um_conditional(um_T um, int rA, int rB, int rC)
{
        /* Ensuring non-null value was passed in */
        assert(um && rA >= 0 && rA < 8 && 
               rB >= 0 && rB < 8 && rC >= 0 && rC < 8);
        
        if (um->registers[rC] != 0) {
                um->registers[rA] = um->registers[rB];
        }
}

/********************************um_seg_load***********************************
 *
 * Executes segmented load operation of the um
 * Inputs:
 *         um_T um:  The um that is in use
 *         int rA:   The number of register A, where the value will be stored
 *         int rB:   The number of register B, the id of the segment to access
 *         int rC:   The number of register C, the offset of the word to load
 * Return: none
 * Expects:
 *         um to be non-null
 *         rA, rB, and rC to be in range [0, 7]
 * Notes:
 *         CRE if um is null
 *         CRE if rA, rB, or rC is not in range [0, 7]
 *****************************************************************************/
static void um_seg_load(um_T um, int rA, int rB, int rC)
{
        /* Ensuring non-null value was passed in */
        assert(um && rA >= 0 && rA < 8 && 
               rB >= 0 && rB < 8 && rC >= 0 && rC < 8);

        um->registers[rA] = Segment_word_at(um->segments, um->registers[rB], 
                                            um->registers[rC]);
}

/********************************um_seg_store**********************************
 *
 * Executes segmented store operation of the um
 * Inputs:
 *         um_T um:  The um that is in use
 *         int rA:   The number of register A, containing the id of the segment
 *                   to store in
 *         int rB:   The number of register B, which contains the offset to  
 *                   store the value at
 *         int rC:   The number of register C, containing the value to store
 * Return: none
 * Expects:
 *         um to be non-null
 *         rA, rB, and rC to be in range [0, 7]
 * Notes:
 *         CRE if um is null
 *         CRE if rA, rB, or rC is not in range [0, 7]
 *****************************************************************************/
static void um_seg_store(um_T um, int rA, int rB, int rC)
{
        /* Ensuring non-null value was passed in */
        assert(um && rA >= 0 && rA < 8 && 
               rB >= 0 && rB < 8 && rC >= 0 && rC < 8);

        Segment_load_word(um->segments, um->registers[rA], um->registers[rB], 
                          um->registers[rC]);
}

/********************************um_addition***********************************
 *
 * Executes addition operation of the um
 * Inputs:
 *         um_T um:  The um that is in use
 *         int rA:   The number of register A, the destination of the result
 *         int rB:   The number of register B, containing a value to add
 *         int rC:   The number of register C, containing a value to add
 * Return: none
 * Expects:
 *         um to be non-null
 *         rA, rB, and rC to be in range [0, 7]
 * Notes:
 *         CRE if um is null
 *         CRE if rA, rB, or rC is not in range [0, 7]
 *****************************************************************************/
static void um_addition(um_T um, int rA, int rB, int rC)
{
        /* Ensuring non-null value was passed in */
        assert(um && rA >= 0 && rA < 8 && 
               rB >= 0 && rB < 8 && rC >= 0 && rC < 8);

        um->registers[rA] = um->registers[rB] + um->registers[rC];
}

/********************************um_multi**************************************
 *
 * Executes multiplication operation of the um
 * Inputs:
 *         um_T um:  The um that is in use
 *         int rA:   The number of register A, the destination of the result
 *         int rB:   The number of register B, containing a value to multiply
 *         int rC:   The number of register C, containing a value to multiply
 * Return: none
 * Expects:
 *         um to be non-null
 *         rA, rB, and rC to be in range [0, 7]
 * Notes:
 *         CRE if um is null
 *         CRE if rA, rB, or rC is not in range [0, 7]
 *****************************************************************************/
static void um_multi(um_T um, int rA, int rB, int rC)
{
        /* Ensuring non-null value was passed in */
        assert(um && rA >= 0 && rA < 8 && 
               rB >= 0 && rB < 8 && rC >= 0 && rC < 8);
        
        um->registers[rA] = um->registers[rB] * um->registers[rC];
}

/********************************um_division***********************************
 *
 * Executes division operation of the um
 * Inputs:
 *         um_T um:  The um that is in use
 *         int rA:   The number of register A, the destination of the result
 *         int rB:   The number of register B, containing a value to divide
 *         int rC:   The number of register C, containing a value to divide
 * Return: none
 * Expects:
 *         um to be non-null
 *         rA, rB, and rC to be in range [0, 7]
 *         The value in registers[rC] to be non-zero
 * Notes:
 *         CRE if um is null
 *         CCRE if rA, rB, or rC is not in range [0, 7]
 *         CRE if value in registers[rC] is zero
 *****************************************************************************/
static void um_division(um_T um, int rA, int rB, int rC)
{
        /* Ensuring non-null value was passed in */
        assert(um && rA >= 0 && rA < 8 && 
               rB >= 0 && rB < 8 && rC >= 0 && rC < 8 &&
               um->registers[rC] != 0);

        um->registers[rA] = um->registers[rB] / um->registers[rC];
}

/********************************um_nand***************************************
 *
 * Executes NAND operation of the um
 * Inputs:
 *         um_T um:  The um that is in use
 *         int rA:   The number of register A, the destination of the result
 *         int rB:   The number of register B, containing a value to NAND
 *         int rC:   The number of register C, containing a value to NAND
 * Return: none
 * Expects:
 *         um to be non-null
 *         rA, rB, and rC to be in range [0, 7]
 * Notes:
 *         CRE if um is null
 *         CRE if rA, rB, or rC is not in range [0, 7]
 *****************************************************************************/
static void um_nand(um_T um, int rA, int rB, int rC)
{
        /* Ensuring non-null value was passed in */
        assert(um && rA >= 0 && rA < 8 && 
               rB >= 0 && rB < 8 && rC >= 0 && rC < 8);

        um->registers[rA] = ~(um->registers[rB] & um->registers[rC]);
}

/********************************um_halt***************************************
 *
 * Executes halt operation of the um that stops the um from running
 * Inputs:
 *         um_T um: The um that is in use
 * Return: none
 * Expects:
 *         um to be non-null
 * Notes:
 *         CRE if um is null
 *****************************************************************************/
static void um_halt(um_T um)
{
        /* Ensuring non-null value was passed in */
        assert(um);

        Segment_free(&um->segments);
        um->segments = NULL;
}

/********************************um_map_seg************************************
 *
 * Executes map segment operation of the um that allows the user to allocate
 * memory for a segment
 * Inputs:
 *         um_T um:  The um that is in use
 *         int rB:   The number of register B, where the id of the mapped 
 *                   segment will be stored
 *         int rC:   The number of register C, which contains the length of the
 *                   new segment
 * Return: none
 * Expects:
 *         um to be non-null
 *         rB and rC to be in range [0, 7]
 * Notes:
 *         CRE if um is null
 *         CRE if rB or rC is not in range [0, 7]
 *****************************************************************************/
static void um_map_seg(um_T um, int rB, int rC)
{
        /* Ensuring non-null value was passed in */
        assert(um && rB >= 0 && rB < 8 && rC >= 0 && rC < 8);

        um->registers[rB] = Segment_map(um->segments, um->registers[rC]);
}

/********************************um_unmap_seg**********************************
 *
 * Executes unmap segment operation of the um that allows the user to
 * deallocate memory associated with a segment
 * Inputs:
 *         um_T um:  The um that is in use
 *         int rC:   The number of register C, which contatins the id of the 
 *                   segment to unmap
 * Return: none
 * Expects:
 *         um to be non-null
 *         rC to be in range [0, 7]
 *         registers[rC] to be non-zero
 * Notes:
 *         CRE if um is null
 *         CRE if rC is not in range [0, 7]
 *         CRE if registers[rC] is zero
 *****************************************************************************/
static void um_unmap_seg(um_T um, int rC)
{
        /* Ensuring non-null value was passed in */
        assert(um && rC >= 0 && rC < 8 && um->registers[rC] != 0);

        Segment_unmap(um->segments,  um->registers[rC]);
}

/********************************um_output*************************************
 *
 * Executes output operation of the um that allows the user to print out a
 * value from a register
 * Inputs:
 *         um_T um:  The um that is in use
 *         int rC:   The number of register C which contains the value that
 *                   will be printed out
 * Return: none
 * Expects:
 *         um to be non-null
 *         rC to be in range [0, 7]
 * Notes:
 *         CRE if um is null
 *         CRE if rC is not in range [0, 7]
 *****************************************************************************/
static void um_output(um_T um, int rC)
{
        /* Ensuring non-null value was passed in */
        assert(um && rC >= 0 && rC < 8);
        assert(um->registers[rC] < 256);

        putchar(um->registers[rC]);
}

/********************************um_input**************************************
 *
 * Executes input operation of the um that allows the user to provide input
 * that gets stored in a register
 * Inputs:
 *         um_T um:  The um that is in use
 *         int rC:   The number of register C which will store the value that
 *                   the user provides
 * Return: none
 * Expects:
 *         um to be non-null
 *         rC to be in range [0, 7]
 * Notes:
 *         CRE if um is null
 *         CRE if rC is not in range [0, 7]
 *****************************************************************************/
static void um_input(um_T um, int rC)
{
        int c = getchar();

        if (c == EOF) {
                um->registers[rC] = ~(uint32_t)0;
        }
        else {
                assert(c < 256);
                um->registers[rC] = (uint32_t) c;
        }
}

/********************************um_load_prog**********************************
 *
 * Executes load program operation of the um that duplicates a segment and
 * replaces it with the current segment at segment zero
 * Inputs:
 *         um_T um:  The um that is in use
 *         int rB:   The number of register B which contains the id of the 
 *                   segment that will be duplicated
 *         int rC:   The number of register C which contains the value that
 *                   program counter will be set to
 * Return: none
 * Expects:
 *         um to be non-null
 *         rB and rC to be in range [0, 7]
 *         registers[rC] to be less than the length of the segment whose id is
 *         in register[rB]
 *         Program counter to point to a valid instruction in segment zero
 * Notes:
 *         CRE if um is null
 *         CRE if rC is not in range [0, 7]
 *         CRE if registers[rC] is greater than or equal to the length of the
 *         segment whose id is in register[rB]
 *         CRE if program counter points to an invalid instruction in segment
 *         zero
 *****************************************************************************/
static void um_load_prog(um_T um, int rB, int rC)
{
        /* Ensuring non-null value was passed in */
        assert(um && rB >= 0 && rB < 8 && rC >= 0 && rC < 8);

        /* Loading segment zero to segment zero */
        if (um->registers[rB] != 0) {
                Segment_load_program(um->segments, um->registers[rB]);
        }

        /* Assert program counters new placement is in bounds of seg 0 */
        assert((int) um->registers[rC] < Segment_line_length(um->segments, 0));

        /* Minus one because we increment program counter every time in run */
        um->program_count = um->registers[rC];
        assert(um_is_valid_inst(um));
}

/********************************um_load_val***********************************
 *
 * Executes load value operation of the um that loads a value into a register
 * Inputs:
 *         um_T um:      The um that is in use
 *         int rA:       The number of register A which will store the value
 *                       provided
 *         uint32_t val: The value that will be stored in the register
 * Return: none
 * Expects:
 *         um to be non-null
 *         rA to be in range [0, 7]
 * Notes:
 *         CRE if um is null
 *         CRE if rA is not in range [0, 7]
 *****************************************************************************/
static void um_load_val(um_T um, int rA, uint32_t val)
{
        /* Ensuring non-null value was passed in */
        assert(um && rA >= 0 && rA < 8);

        um->registers[rA] = val;
}

/***************************um_is_valid_inst***********************************
 *
 * Determines whether the word at program counter contains a valid instruction
 * Inputs:
 *         um_T um: The um that is in use
 * Return: Boolean indicating whether the word at program counter contains a 
 *         valid instruction
 * Expects:
 *         um to be non-null
 * Notes:
 *         CRE if um is null
 *****************************************************************************/
static bool um_is_valid_inst(um_T um)
{
        /* Ensuring non-null value was passed in */
        assert(um);

        /* Access the instruction from segment zero */
        uint32_t word = Segment_word_at(um->segments, 0, um->program_count);
        uint32_t op_code = Bitpack_getu(word, 4, 28);

        return op_code <= 13;
}

#endif