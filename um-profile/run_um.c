/******************************************************************************
 *
 *                                  run_um.c
 *
 *     Assignment: um
 *     Authors:    Marten Tropp and Matthew Carey
 *     Date:       11/20/2023
 *      
 *     The purpose of this file is to handle the command line and open any 
 *     files when the UM is run. It takes a .um file from the command line and
 *     emulates the behavior of it being run on a UM.  
 *    
 *
 *****************************************************************************/

#include <stdlib.h>
#include "assert.h"
#include <stdint.h>
#include "mem.h"
#include <stdio.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>
#include "seq.h"

struct line_T {
        uint32_t *words;
        unsigned length;
};

struct Segment_T {
        struct line_T *segments;
        int numSegs;
        int capacity;

        uint32_t *IDs;
        int64_t rightMost;
        unsigned size;
};

struct um_T {
        uint32_t registers[8];
        struct Segment_T segments;
        int program_count;
        bool halt;
};

static inline FILE *open_file(int argc, char *argv[]);
static inline void run_um(struct um_T *um, FILE *fp, int length);
static inline void initialize_seg_zero(struct um_T *um, FILE *fp, int length);
static inline void handle_instruction(struct um_T *um, uint32_t instruction);
static inline struct um_T um_new(uint32_t size);
static inline struct Segment_T Segment_new(uint32_t size);
static inline uint32_t Segment_map(struct Segment_T *seg, uint32_t size);
static inline void Segment_unmap(struct Segment_T *seg, uint32_t id);
static inline uint32_t Segment_word_at(struct Segment_T *seg, uint32_t id, uint32_t offset);
static inline void Segment_load_program(struct Segment_T *seg, uint32_t id);
static inline void Segment_load_word(struct Segment_T *seg, uint32_t id, uint32_t offset, uint32_t word);
static inline uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb);

int main(int argc, char *argv[]) 
{
        /* Trying to open a file */
        FILE *fp = open_file(argc, argv);

        /* Getting size of the file */
        fseek(fp, 0L, SEEK_END); 
        float length = ftell(fp) / 4; 
        fseek(fp, 0, SEEK_SET);

        /* Running the um */
        struct um_T universal_machine = um_new(length);
        run_um(&universal_machine, fp, length);
        
        /* Closing input file*/
        fclose(fp);
        
        return EXIT_SUCCESS;
}

static inline FILE *open_file(int argc, char *argv[]) 
{
        /* Incorrect comman line check */
        if (argc != 2) {
                fprintf(stderr, "Usage: ./um [file].um\n");
                exit(1);
        }
        
        /* Opening file */
        FILE *fp = NULL;
        fp = fopen(argv[1], "r");

        /* File couldn't be opened */
        if (fp == NULL) {
                fprintf(stderr, "Error opening file.\n");
                exit(1);
        }

        return fp;
}

static inline void initialize_seg_zero(struct um_T *um, FILE *fp, int length)
{
        /* Get first charater from file */
        int word1 = fgetc(fp);
        int word2 = fgetc(fp);
        int word3 = fgetc(fp);
        int word4 = fgetc(fp);

        /* Store contents of file in segment zero */
        for (int offset = 0; offset < length; offset++) {
                uint32_t word = 0;
                word = ((word << (8)) >> (8)) | (word1 << 24);
                word = ((word >> 24) << 24) | ((word << (16)) >> (16)) | (word2 << 16);
                word = ((word >> 16) << 16) | ((word << (24)) >> (24)) | (word3 << 8);
                word = ((word >> 8) << 8) | (word4);
                um->segments.segments[0].words[offset] = word;

                /* Get bit values from next chars for next instruction */
                word1 = fgetc(fp);
                word2 = fgetc(fp);
                word3 = fgetc(fp);
                word4 = fgetc(fp);
        }
}

static inline void run_um(struct um_T *um, FILE *fp, int length)
{
        /* Reading in values from files to segment zero */
        initialize_seg_zero(um, fp, length);

        /* Running program until end of segment zero */
        while (!(um->halt)) {
                uint32_t instruction = Segment_word_at(&(um->segments), 0, 
                                   um->program_count);
                handle_instruction(um, instruction);
                um->program_count++;
        }
}

static inline void handle_instruction(struct um_T *um, uint32_t instruction)
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
                assert(rA < 8 && rB < 8 && rC < 8);
                if (um->registers[rC] != 0) {
                        um->registers[rA] = um->registers[rB];
                }
                break;
        case 1:
                assert(rA < 8 && rB < 8 && rC < 8);
                um->registers[rA] = Segment_word_at(&(um->segments), um->registers[rB], um->registers[rC]);
                break;
        case 2:
                assert(rA < 8 && rB < 8 && rC < 8);
                Segment_load_word(&(um->segments), um->registers[rA], um->registers[rB], um->registers[rC]);                
                break;
        case 3:
                assert(rA < 8 && rB < 8 && rC < 8);
                um->registers[rA] = um->registers[rB] + um->registers[rC];
                break;
        case 4:
                assert(rA < 8 && rB < 8 && rC < 8);
                um->registers[rA] = um->registers[rB] * um->registers[rC];
                break;
        case 5:
                assert(rA < 8 && rB < 8 && rC < 8);
                um->registers[rA] = um->registers[rB] / um->registers[rC];
                break;
        case 6:
                assert(rA < 8 && rB < 8 && rC < 8);
                um->registers[rA] = ~(um->registers[rB] & um->registers[rC]);
                break;
        case 7: ;
                struct Segment_T seg = um->segments;
                int numItems = seg.numSegs - 1;
                while (numItems >= 0) {
                        struct line_T line = seg.segments[numItems];
                        if (line.words != NULL) {
                                free(line.words);
                        }
                        
                        numItems--;
                }

                free(seg.segments);
                free(seg.IDs);
                um->halt = true;
                break;
        case 8:
                assert(rB < 8 && rC < 8);
                um->registers[rB] = Segment_map(&(um->segments), um->registers[rC]);
                break;
        case 9:
                assert(rC < 8);
                Segment_unmap(&(um->segments),  um->registers[rC]);
                break;
        case 10:
                assert(rC < 8);
                putchar(um->registers[rC]);
                break;
        case 11:
                assert(rC < 8);
                int c = getchar();
                if (c == EOF)
                        um->registers[rC] = ~(uint32_t)0;
                else
                        um->registers[rC] = (uint32_t) c;
                break;
        case 12:
                assert(rB < 8 && rC < 8);
                if (um->registers[rB] != 0)
                        Segment_load_program(&(um->segments), um->registers[rB]);
                um->program_count = um->registers[rC] -1;
                break;
        case 13:
                assert(rA < 8);
                um->registers[rA] = value;
                break;
        }
}

static inline struct Segment_T Segment_new(uint32_t size)
{
        /* Allocating memory for the Segment_T variable */
        struct Segment_T seg;

        /* Making sequence for unmapped IDs */
        seg.IDs = calloc(1000, sizeof(uint32_t));
        assert(seg.IDs);
        seg.size = 1000;
        seg.rightMost = -1;

        /* Assigning values to the Segment_T variable */
        seg.segments = (struct line_T *)malloc(1000 * sizeof(struct line_T));
        assert(seg.segments);
        
        struct line_T bot = {NULL, 0};
        for (size_t i = 0; i < 1000; i++) {
                seg.segments[i] = bot;
        }
        
        seg.numSegs = 0;
        seg.capacity = 1000;
        /* Adding segment zero to the segments */
        Segment_map(&seg, size);

        return seg;
}

static inline uint32_t Segment_map(struct Segment_T *seg, uint32_t size)
{
        /* Allocating memory for a segment of provided length */
        struct line_T new_seg = {NULL, size};
        new_seg.words = calloc(size, sizeof(uint32_t));

        if (seg->rightMost >= 0) {
                /* Freeing memory associated with the line at id */
                uint32_t id = seg->IDs[seg->rightMost];
                seg->rightMost--;

                /* Storging the new segment */
                seg->segments[id] = new_seg;

                return id;
        }
        
        if (seg->numSegs >= seg->capacity) {
                seg->capacity = seg->capacity * 2;
                struct line_T *temp = (struct line_T *)realloc(seg->segments, seg->capacity * sizeof(struct line_T));
                assert(temp);

                struct line_T bot = {NULL, 0};
                for (int i = seg->numSegs; i < seg->capacity; i++) {
                        temp[i] = bot;
                }

                seg->segments = temp;
        }

        /* Storing the new segment */
        seg->segments[seg->numSegs] = new_seg;
        seg->numSegs++;
        return (uint32_t) seg->numSegs - 1;
}

static inline void Segment_unmap(struct Segment_T *seg, uint32_t id)
{
        /* Access the segment */
        struct line_T line = seg->segments[id];
        free(line.words);

        struct line_T bot = {NULL, 0};
        seg->segments[id] = bot;

        /* Adding id to unmapped IDs sequence */
        if (seg->size - 1 <= seg->rightMost) {
                seg->size = seg->size * 2;
                uint32_t *temp = realloc(seg->IDs, seg->size * sizeof(uint32_t));
                assert(temp);

                unsigned cap = seg->size;
                for (unsigned i = seg->rightMost + 1; i < cap; i++) {
                        temp[i] = 0;
                }

                seg->IDs = temp;
        }

        seg->rightMost++;
        seg->IDs[seg->rightMost] = id;
}

static inline uint32_t Segment_word_at(struct Segment_T *seg, uint32_t id, uint32_t offset)
{
        /* Accessing desired segment */
        struct line_T line = seg->segments[id];

        return line.words[offset];
}

static inline void Segment_load_program(struct Segment_T *seg, uint32_t id)
{
        /* Freeing segment currently at zero */
        struct line_T zero = seg->segments[0];
        free(zero.words);

        /* Getting length of line that is duplicated */
        struct line_T segment_zero = seg->segments[id];
        int length = segment_zero.length;

        /* Making copy of the segment */
        struct line_T copy_zero = {NULL, length};
        copy_zero.words = calloc(length, sizeof(uint32_t));
        assert(copy_zero.words);

        for (int i = 0; i < length; i++) {
                copy_zero.words[i] = segment_zero.words[i];
        }

        /* Overwriting segment zero */
        seg->segments[0] = copy_zero;
}

static inline void Segment_load_word(struct Segment_T *seg, uint32_t id, 
                       uint32_t offset, uint32_t word)
{
        seg->segments[id].words[offset] = word;
}

static inline struct um_T um_new(uint32_t size)
{
        /* Allocating memory for the um_T variable */
        struct um_T um;

        /* Initialize Segment_T variable */
        struct Segment_T segments = Segment_new(size);

        /* Assigning values to the um_T variable */
        um.segments = segments;
        um.program_count = 0;
        um.halt = false;

        /* Giving registers default values */
        for (int i = 0; i < 8; i ++) {
                um.registers[i] = 0;
        }
        
        return um;
}

static inline uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        unsigned hi = lsb + width;
        return (word << (64 - hi)) >> (64 - width);
}