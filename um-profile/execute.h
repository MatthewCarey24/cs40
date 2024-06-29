/******************************************************************************
 *
 *                                  exectue.h
 *
 *     Assignment: um
 *     Authors:    Marten Tropp and Matthew Carey
 *     Date:       11/20/2023
 *      
 *     The purpose of this file is to define the functions of the execute 
 *     module. These are the functions used by the main UM program to emulate a
 *     UM. 
 *    
 *
 *****************************************************************************/

#include "um.h"
#include <stdio.h>
#include "segment.h"
#include <inttypes.h>
#include "bitpack.h"

#ifndef EXECUTE_INCLUDED
#define EXECUTE_INCLUDED

static void run_um(um_T um, FILE *fp, int length);
static void initialize_seg_zero(um_T um, FILE *fp, int length);
static void handle_instruction(um_T um, uint32_t instruction);

/***************************initialize_seg_zero********************************
 *
 * Initializes every word value in the 0th segment
 * Inputs:
 *         um_T um:    A struct representing a UM.
 *         FILE *fp:   A pointer to the .um file containing the program 
 *                     instructions.
 *         int length: An integer detailing the length of the file in bytes.
 * Return: none
 * Expects:
 *         File to not be NULL, length to be the correct length of fp
 * Notes:
 *         none
 *****************************************************************************/
static void initialize_seg_zero(um_T um, FILE *fp, int length)
{
        /* Get first charater from file */
        int word1 = fgetc(fp);
        int word2 = fgetc(fp);
        int word3 = fgetc(fp);
        int word4 = fgetc(fp);

        /* Store contents of file in segment zero */
        for (int offset = 0; offset < length; offset++) {
                uint32_t word = 0;
                word = Bitpack_newu(word, 8, 24, word1);
                word = Bitpack_newu(word, 8, 16, word2);
                word = Bitpack_newu(word, 8, 8, word3);
                word = Bitpack_newu(word, 8, 0, word4);
                Segment_load_word(um->segments, 0, offset, word);

                /* Get bit values from next chars for next instruction */
                word1 = fgetc(fp);
                word2 = fgetc(fp);
                word3 = fgetc(fp);
                word4 = fgetc(fp);
        }
}


/**********************************run_um**************************************
 *
 * Executes the program provided to the um
 * Inputs:
 *         um_T um:    A struct representing a UM.
 *         FILE *fp:   A pointer to the .um file containing the program 
 *                     instructions.
 *         int length: An integer detailing the length of the file in bytes.
 * Return: void
 * Expects:
 *         File to not be NULL, length to be the correct length of fp
 * Notes:
 *         none
 *****************************************************************************/
static void run_um(um_T um, FILE *fp, int length)
{
        /* Reading in values from files to segment zero */
        initialize_seg_zero(um, fp, length);

        /* Running program until end of segment zero */
        while (um->program_count < Segment_line_length(um->segments, 0)) {
                uint32_t instruction = Segment_word_at(um->segments, 0, 
                                   um->program_count);
                handle_instruction(um, instruction);
                um->program_count++;
        }
}

/***************************handle_instruction*********************************
 *
 * Perform the correct um operation based on the given instruction
 * Inputs:
 *         um_T um:              A struct representing a UM.
 *         uint32_t instruction: 32-bit word representing the bitpacked um 
                                 instruction
 * Return: void
 * Expects:
 *         none
 * Notes:
 *         none
 *****************************************************************************/
static void handle_instruction(um_T um, uint32_t instruction)
{
        uint32_t op_code = Bitpack_getu(instruction, 4, 28);
        uint32_t rA = 0;
        uint32_t rB = 0;
        uint32_t rC = 0;
        uint32_t value = 0;

        /* Unpacking values from instruction */
        if (op_code == 13) {
                rA = Bitpack_getu(instruction, 3, 25);
                value = Bitpack_getu(instruction, 25, 0);
        }
        else {
                rA = Bitpack_getu(instruction, 3, 6);
                rB = Bitpack_getu(instruction, 3, 3);
                rC = Bitpack_getu(instruction, 3, 0);
        }

        /* Handling command */
        switch (op_code) {
        case 0:
                um_conditional(um, rA, rB, rC);
                break;
        case 1:
                um_seg_load(um, rA, rB, rC);
                break;
        case 2:
                um_seg_store(um, rA, rB, rC);
                break;
        case 3:
                um_addition(um, rA, rB, rC);
                break;
        case 4:
                um_multi(um, rA, rB, rC);
                break;
        case 5:
                um_division(um, rA, rB, rC);
                break;
        case 6:
                um_nand(um, rA, rB, rC);
                break;
        case 7:
                um_halt(um);
                break;
        case 8:
                um_map_seg(um, rB, rC);
                break;
        case 9:
                um_unmap_seg(um, rC);
                break;
        case 10:
                um_output(um, rC);
                break;
        case 11:
                um_input(um, rC);
                break;
        case 12:
                um_load_prog(um, rB, rC);
                um->program_count--;
                break;
        case 13:
                um_load_val(um, rA, value);
                break;
        }
}

#endif