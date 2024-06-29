/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        /* Initializing empty 32-bit instruction */
        Um_instruction instruction = 0;

        /* Packing values into the instruction */
        instruction = Bitpack_newu(instruction, 4, 28, op);
        instruction = Bitpack_newu(instruction, 3, 6, ra);
        instruction = Bitpack_newu(instruction, 3, 3, rb);
        instruction = Bitpack_newu(instruction, 3, 0, rc);

        return instruction;
}

Um_instruction loadval(unsigned ra, unsigned val)
{
        /* Initializing empty 32-bit instruction */
        Um_instruction instruction = 0;

        /* Packing values into the instruction */
        instruction = Bitpack_newu(instruction, 4, 28, 13);
        instruction = Bitpack_newu(instruction, 3, 25, ra);
        instruction = Bitpack_newu(instruction, 25, 0, val);

        return instruction;
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

/* Wrapper functions for each of the instructions */
static inline Um_instruction conditional_move(Um_register a, Um_register b, Um_register c)
{
        return three_register(CMOV, a, b, c);
}

static inline Um_instruction segment_load(Um_register a, Um_register b, Um_register c)
{
        return three_register(SLOAD, a, b, c);
}

static inline Um_instruction segment_store(Um_register a, Um_register b, Um_register c)
{
        return three_register(SSTORE, a, b, c);
}

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction multiply(Um_register a, Um_register b, Um_register c) 
{
        return three_register(MUL, a, b, c);
}

static inline Um_instruction divide(Um_register a, Um_register b, Um_register c) 
{
        return three_register(DIV, a, b, c);
}

static inline Um_instruction nand(Um_register a, Um_register b, Um_register c) 
{
        return three_register(NAND, a, b, c);
}

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

static inline Um_instruction map(Um_register b, Um_register c)
{
        return three_register(ACTIVATE, 0, b, c);
}

static inline Um_instruction unmap(Um_register c)
{
        return three_register(INACTIVATE, 0, 0, c);
}

static inline Um_instruction output(Um_register c)
{
        return three_register(OUT, 0, 0, c);
}

static inline Um_instruction input(Um_register c)
{
        return three_register(IN, 0, 0, c);
}

static inline Um_instruction loadp(Um_register b, Um_register c)
{
        return three_register(LOADP, 0, b, c);
}


/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}


/* Unit tests for the UM */

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_halt_twice_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, halt());
}

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '!'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
}

void build_add_output_test(Seq_T stream)
{
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 6));
        append(stream, add(r3, r1, r2));
        append(stream, output(r3)); // expect 6
        append(stream, halt());
}

void build_load_value_test(Seq_T stream)
{
        append(stream, loadval(r3, 54));
        append(stream, output(r3)); // expect 6
        append(stream, halt());
}

void build_input_test(Seq_T stream)
{
        append(stream, input(r3));
        append(stream, output(r3)); // expect same as input
        append(stream, halt());
}

void build_add_test(Seq_T stream)
{
        append(stream, loadval(r1, 55));
        append(stream, loadval(r2, 45));
        append(stream, add(r3, r1, r2));
        append(stream, output(r3)); // expect d
        append(stream, halt());
}

void build_multiplication_test(Seq_T stream)
{
        append(stream, loadval(r1, 3));
        append(stream, loadval(r2, 17));
        append(stream, multiply(r3, r1, r2));
        append(stream, output(r3)); // expect 3
        append(stream, halt());
}

void build_division_test(Seq_T stream)
{
        append(stream, loadval(r1, 150));
        append(stream, loadval(r2, 3));
        append(stream, divide(r3, r1, r2));
        append(stream, output(r3)); // expect 2
        append(stream, halt());
}

void build_nand_test(Seq_T stream)
{
        append(stream, input(r3));
        append(stream, nand(r3, r3, r3));
        append(stream, nand(r3, r3, r3));
        append(stream, loadval(r1, 48));
        append(stream, add(r3, r1, r3));
        append(stream, output(r3)); // expect input value
        append(stream, halt());
}

void build_true_conditional_move_test(Seq_T stream)
{
        append(stream, loadval(r1, 55));
        append(stream, loadval(r2, 53));
        append(stream, loadval(r3, 1));
        append(stream, conditional_move(r1, r2, r3));
        append(stream, output(r1)); // expect 5
        append(stream, halt());
}

void build_false_conditional_move_test(Seq_T stream)
{
        append(stream, loadval(r1, 55));
        append(stream, loadval(r2, 54));
        append(stream, loadval(r3, 0));
        append(stream, conditional_move(r1, r2, r3));
        append(stream, output(r1)); // expect 7
        append(stream, halt());
}

void build_segment_load_test(Seq_T stream)
{
        append(stream, loadval(r1, 3));
        append(stream, loadval(r2, 0));
        append(stream, loadval(r3, 1));
        append(stream, segment_load(r1, r2, r3));
        append(stream, halt());
}

void build_segment_store_test(Seq_T stream)
{
        append(stream, loadval(r1, 0));
        append(stream, loadval(r2, 0));
        append(stream, loadval(r3, 50));
        append(stream, segment_store(r1, r2, r3));
        append(stream, loadval(r2, 0));
        append(stream, loadval(r3, 0));
        append(stream, segment_load(r1, r2, r3));
        append(stream, output(r1)); // expect 2
        append(stream, halt());
}

void build_map_segment_test(Seq_T stream)
{
        append(stream, map(r2, 7));
        append(stream, map(r2, 7));
        append(stream, map(r2, 7));
        append(stream, halt());
}

void build_unmap_segment_test(Seq_T stream)
{

        append(stream, map(r2, 7));
        append(stream, map(r2, 7));
        append(stream, map(r2, 7));
        append(stream, unmap(r2));
        append(stream, loadval(r3, 50));
        append(stream, map(r2, r3));
        append(stream, map(r2, r3));
        append(stream, loadval(r1, 5));
        append(stream, loadval(r3, 50));
        append(stream, segment_store(r2, r1, r3));
        append(stream, segment_load(r1, r2, r1));
        append(stream, output(r1)); // expect 2
        append(stream, halt());
}

void build_load_program_test(Seq_T stream)
{
        append(stream, loadval(r1, 50));
        append(stream, map(r2, r1));
        append(stream, loadval(r3, 49));
        append(stream, loadp(r2, r3));
        append(stream, halt());
}

void build_division_by_zero_test(Seq_T stream) // expect error
{
        append(stream, loadval(r1, 3));
        append(stream, loadval(r2, 0));
        append(stream, divide(r3, r1, r2));
        append(stream, output(r3));
        append(stream, halt());
}

void build_output_fail_test(Seq_T stream)
{
        append(stream, loadval(r1, 1));
        append(stream, loadval(r2, 4));
        append(stream, nand(r3, r1, r2));
        append(stream, output(r3)); // expect error
        append(stream, halt());
}

void build_exec_500k(Seq_T stream)
{
        for (size_t i = 0; i < 125000; i++)
        {
                append(stream, loadval(r1, i));
                append(stream, loadval(r3, i + 1));
                append(stream, map(r1, r3));
                append(stream, unmap(r1));
        }       
}

void build_out_of_bounds_prog_count_test(Seq_T stream)
{
        append(stream, loadval(r1, 0));
        append(stream, loadval(r2, 999999));
        append(stream, loadp(r1, r2));
        append(stream, halt());
}

void build_invalid_instr_at_prog_count_test(Seq_T stream)
{
        append(stream, loadval(r1, 0));
        append(stream, loadval(r2, 0));
        append(stream, loadval(r3, 444444));
        append(stream, nand(r3, r3, r3));
        append(stream, segment_store(r1, r2, r3));
        append(stream, loadval(r1, 0));
        append(stream, loadp(r1, r1));
        append(stream, halt());
}

void build_seg_load_unmapped(Seq_T stream)
{
        append(stream, loadval(r1, 1));
        append(stream, loadval(r3, 3));
        append(stream, map(r2, r3));
        append(stream, unmap(r2));
        append(stream, segment_load(r1, r2, r3));
        append(stream, halt());
}

void build_seg_store_unmapped(Seq_T stream)
{
        append(stream, loadval(r1, 1));
        append(stream, loadval(r3, 3));
        append(stream, map(r2, r3));
        append(stream, unmap(r2));
        append(stream, segment_store(r2, r1, r3));
        append(stream, halt());
}

void build_seg_load_out_of_bounds(Seq_T stream)
{
        append(stream, loadval(r1, 1));
        append(stream, loadval(r3, 3));
        append(stream, map(r2, r3));
        append(stream, segment_load(r1, r2, r3));
}

void build_seg_store_out_of_bounds(Seq_T stream)
{
        append(stream, loadval(r1, 1));
        append(stream, loadval(r3, 3));
        append(stream, map(r2, r3));
        append(stream, segment_store(r2, r3, r1));
        append(stream, halt());
}

void build_unmap_seg_zero(Seq_T stream)
{
        append(stream, loadval(r1, 0));
        append(stream, unmap(r1));
        append(stream, halt());
}

void build_unmap_unmapped_seg(Seq_T stream)
{
        append(stream, loadval(r1, 1));
        append(stream, loadval(r3, 3));
        append(stream, map(r2, r3));
        append(stream, unmap(r2));
        append(stream, unmap(r2));
        append(stream, halt());
}

void build_load_prog_from_not_mapped(Seq_T stream)
{
        append(stream, loadval(r1, 5));
        append(stream, loadp(r1, r1));
        append(stream, halt());
}

void build_load_prog_from_unmapped(Seq_T stream)
{
        append(stream, loadval(r1, 1));
        append(stream, loadval(r3, 3));
        append(stream, map(r2, r3));
        append(stream, unmap(r2));
        append(stream, loadp(r1, r2));
        append(stream, halt());
}