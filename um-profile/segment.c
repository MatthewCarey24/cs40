/******************************************************************************
 *
 *                                  segment.c
 *
 *     Assignment: um
 *     Authors:    Marten Tropp and Matthew Carey
 *     Date:       11/20/2023
 *      
 *     The purpose of this file is to implement the Segment_T data structure
 *     by defining the functions associated with it. The data structure is
 *     used in our implementation of an universal machine.
 *
 *    
 *
 *****************************************************************************/

#include "segment.h"
#include "seq.h"
#include "mem.h"
#include <stdlib.h>
#include <assert.h>

/******************************Segment_T***************************************
 *
 * Structure that contains two sequences to keep track of mapped segments and
 * identifiers that were in use at some point, but have been unmapped.
 * Stores:
 *         Seq_T segments:    Sequence that stores the mapped segments
 *         Seq_T unmappedIDs: Sequence that stores unsigned 32-bit values that
 *                            were used as identifiers
 *                      
 *****************************************************************************/
struct Segment_T {
        Seq_T segments;
        Seq_T unmappedIDs;
};

/******************************line_T******************************************
 *
 * Structure that stores information related to a segment or line.
 * Stores:
 *         uint32_t *words: A pointer to an array of 32-bit unsigned values
 *                          that store the contents of a line
 *         unsigned length: Unsigned value that contains the length of the 
 *                          array containing words
 *                      
 *****************************************************************************/
typedef struct line_T {
        uint32_t *words;
        unsigned length;
} *line_T;

/********************************line_new**************************************
 *
 * Creates a new line_T structure with the given size
 * Inputs:
 *         uint32_t size: The number of 32-bit words in the line
 * Return: A new line_T with the given length
 * Expects:
 *         none
 * Notes:
 *         CRE if unable to allocate memory of provided size
 *         Allocated memory is supposed to be deallocated using line_free
 *****************************************************************************/
line_T line_new(uint32_t size)
{
        /* Allocating memory for the line_T variable */
        line_T line;
        NEW(line);

        /* Assigning variables right values */
        line->length = size;
        line->words = calloc(size, sizeof(uint32_t));

        /* Ensuring calloc worked */
        assert(line->words);

        return line;
}

/********************************line_free*************************************
 *
 * Free the memory associated with a line_T
 * Inputs:
 *         line_T *line: The line_T that will be freed
 * Return: none
 * Expects:
 *         line and *line to be non-null
 * Notes:
 *         CRE if line or *line is null
 *         Frees memory allocated in line_new
 *****************************************************************************/
void line_free(line_T *line)
{
        /* Ensuring non-null value was passed in */
        assert(line && *line);

        /* Freeing memory associated with the segments */
        free((*line)->words);
        FREE(*line);
}

/********************************Segment_new***********************************
 *
 * Creates a new Segment_T structure with the given size being the size of 
 * segment zero
 * Inputs:
 *         uint32_t size: The number of 32-bit words in segment zero
 * Return: A new Segment_T with segment zero having the provided size
 * Expects:
 *         none
 * Notes:
 *         none
 *         Allocated memory is supposed to be deallocated using Segment_free
 *****************************************************************************/
Segment_T Segment_new(uint32_t size)
{
        /* Allocating memory for the Segment_T variable */
        Segment_T seg;
        NEW(seg);

        /* Storing the zero segment in the segments sequence */
        Seq_T segments = Seq_new(0);

        /* Making sequence for unmapped IDs */
        Seq_T unmapped = Seq_new(0);

        /* Assigning values to the Segment_T variable */
        seg->segments = segments;
        seg->unmappedIDs = unmapped;

        /* Adding segment zero to the segments */
        Segment_map(seg, size);

        return seg;
}

/********************************Segment_map***********************************
 *
 * Map a segment in a provided Segment_T structure of size that is also
 * provided
 * Inputs:
 *         Segment_T seg: The structure containing segments that will store the
 *                        new one that gets mapped
 *         uint32_t size: The number of 32-bit words in the new segment
 * Return: Unique 32-bit identifier of the segment that was mapped
 * Expects:
 *         Segment_T seg to be non-null
 * Notes:
 *         CRE if Segment_T seg is null
 *         Allocated memory is supposed to be deallocated using Segment_unmap
 *****************************************************************************/
uint32_t Segment_map(Segment_T seg, uint32_t size)
{
        /* Ensuring non-null value was passed in */
        assert(seg);

        /* Allocating memory for a segment of provided length */
        line_T new_seg = line_new(size);

        if (Seq_length(seg->unmappedIDs) != 0) {
                /* Freeing memory associated with the line at id */
                uint32_t id = (uint32_t) (uintptr_t) 
                              Seq_remhi(seg->unmappedIDs);        

                /* Storging the new segment */
                Seq_put(seg->segments, id, (void *) new_seg);
                return id;
        }
        
        /* Storing the new segment */
        Seq_addhi(seg->segments, (void *) new_seg);
        return (uint32_t) Seq_length(seg->segments) - 1;
}

/********************************Segment_unmap*********************************
 *
 * Unmap a segment in a provided Segment_T structure that can be accessed
 * using the provided identifier
 * Inputs:
 *         Segment_T seg: The structure containing the segment that will be 
 *                        unmapped
 *         uint32_t id:  The identifier associated with the segment that will 
 *                       be unmapped
 * Return: none
 * Expects:
 *         Segment_T seg to be non-null
 *         uint32_t id to have a value greater than 0
 *         A mapped segment to exist associated with the identifier
 * Notes:
 *         CRE if Segment_T seg is null
 *         CRE if uint32_t is 0
 *         CRE if there does not exis a segment associated with the id
 *         Memory that is deallocated was allocated using Segment_map
 *****************************************************************************/
void Segment_unmap(Segment_T seg, uint32_t id)
{
        /* Ensuring non-null value was passed in and segment is mapped */
        assert(seg && Segment_is_mapped(seg, id) && id > 0);

        /* Access the segment */
        line_T line = Seq_get(seg->segments, id);
        line_free(&line);
        Seq_put(seg->segments, id, NULL);

        /* Adding id to unmapped IDs sequence */
        Seq_addhi(seg->unmappedIDs, (void *) (uintptr_t) id);
}

/********************************Segment_free**********************************
 *
 * Free the memory associated with a Segment_T
 * Inputs:
 *         Segment_T *seg: The Segment_T that will be freed
 * Return: none
 * Expects:
 *         seg and *seg to be non-null
 * Notes:
 *         CRE if seg or *seg is null
 *         Frees memory allocated in Segment_new
 *****************************************************************************/
void Segment_free(Segment_T *seg)
{
        /* Ensuring non-null value was passed in */
        assert(seg && *seg);

        /* Freeing memory associated with the segments */
        int numItems = Seq_length((*seg)->segments);
        while (numItems > 0) {
                line_T line = Seq_remhi((*seg)->segments);
                if (line != NULL) {
                        line_free(&line);
                }
                
                numItems--;
        }

        /* Freeing memory associated with the sequences */
        Seq_free(&(*seg)->segments);
        Seq_free(&(*seg)->unmappedIDs);
        FREE(*seg);
}

/********************************Segment_at************************************
 *
 * Access a segment associated with the provided identifier from a Segment_T
 * Inputs:
 *         Segment_T seg: The Segment_T that contains segments
 *         uint32_t id:   The identifier associated with the segment will be
 *                        accessed
 * Return: Pointer to a line_T structure that is casted to a void pointer
 * Expects:
 *         Segment_T seg to be non-null
 *         A mapped segment to exist associated with the identifier
 * Notes:
 *         CRE if Segment_T seg is null
 *         CRE if there does not exis a segment associated with the id
 *****************************************************************************/
void *Segment_at(Segment_T seg, uint32_t id)
{
        /* Ensuring non-null value was passed in and segment is mapped */
        assert(seg && Segment_is_mapped(seg, id));
        
        /* Access value at id */
        void *line = Seq_get(seg->segments, id);
        
        return line;
}

/*****************************Segment_word_at**********************************
 *
 * Access a word at offset in a segment associated with the provided identifier 
 * from a Segment_T
 * Inputs:
 *         Segment_T seg:   The Segment_T that contains segments
 *         uint32_t id:     The identifier associated with the segment will be
 *                          accessed
 *         uint32_t offset: The offset of the word in the segment
 * Return: A 32-bit word
 * Expects:
 *         Segment_T seg to be non-null
 *         A mapped segment to exist associated with the identifier
 *         Offset to be in range of the segment
 * Notes:
 *         CRE if Segment_T seg is null
 *         CRE if there does not exis a segment associated with the id
 *         CRE if the offset is out of range
 *****************************************************************************/
uint32_t Segment_word_at(Segment_T seg, uint32_t id, uint32_t offset)
{
        /* Ensuring non-null value was passed in and segment is mapped */
        assert(seg && Segment_is_mapped(seg, id));

        /* Accessing desired segment */
        line_T line = Segment_at(seg, id);

        /* Ensure offset is within boundaries */
        assert(offset < line->length);

        return line->words[offset];
}

/*************************Segment_load_program*********************************
 *
 * Duplicate a segment associated with the provided identifier from a Segment_T
 * and put it into segment zero
 * Inputs:
 *         Segment_T seg: The Segment_T that contains segments
 *         uint32_t id:   The identifier associated with the segment will be
 *                        duplicated
 * Return: none
 * Expects:
 *         Segment_T seg to be non-null
 *         A mapped segment to exist associated with the identifier
 * Notes:
 *         CRE if Segment_T seg is null
 *         CRE if there does not exis a segment associated with the id
 *****************************************************************************/
void Segment_load_program(Segment_T seg, uint32_t id)
{
        /* Ensuring non-null value was passed in and segment is mapped */
        assert(seg && Segment_is_mapped(seg, id));

        /* Freeing segment currently at zero */
        line_T zero = Seq_get(seg->segments, 0);
        line_free(&zero);

        /* Getting length of line that is duplicated */
        line_T segment_zero = Seq_get(seg->segments, id);
        int length = segment_zero->length;

        /* Making copy of the segment */
        line_T copy_zero = line_new(length);
        for (int i = 0; i < length; i++) {
                copy_zero->words[i] = segment_zero->words[i];
        }

        /* Overwriting segment zero */
        Seq_put(seg->segments, 0, (void *) copy_zero);
}

/***************************Segment_load_word**********************************
 *
 * Put a word at offset in a segment associated with the provided identifier 
 * from a Segment_T
 * Inputs:
 *         Segment_T seg:   The Segment_T that contains segments
 *         uint32_t id:     The identifier associated with the segment will be
 *                          accessed
 *         uint32_t offset: The offset of the location in the segment
 *         uint32_t word:   The word that will be stored
 * Return: none
 * Expects:
 *         Segment_T seg to be non-null
 *         A mapped segment to exist associated with the identifier
 *         Offset to be in range of the segment
 * Notes:
 *         CRE if Segment_T seg is null
 *         CRE if there does not exis a segment associated with the id
 *         CRE if the offset is out of range
 *****************************************************************************/
void Segment_load_word(Segment_T seg, uint32_t id, 
                       uint32_t offset, uint32_t word)
{
        /* Ensuring non-null value was passed in and segment is mapped */
        assert(seg && Segment_is_mapped(seg, id));
        /* Accessing the location */
        line_T line = Seq_get(seg->segments, id);
        assert(offset < line->length);
        /* Storing value */
        line->words[offset] = word;
}

/*************************Segment_line_length**********************************
 *
 * Get the length of the segment associated with the provided identifier from 
 * a Segment_T
 * Inputs:
 *         Segment_T seg: The Segment_T that contains segments
 *         uint32_t id:   The identifier associated with the segment
 * Return: Length of the segment associated with the provided identifier 
 * Expects:
 *         none
 * Notes:
 *         none
 *****************************************************************************/
int Segment_line_length(Segment_T seg, uint32_t id)
{
        /* Ensuring non-null value was passed in and segment is mapped */
        if (seg == NULL) {
                return 0;
        }

        line_T line = Segment_at(seg, id);
        return line->length;
}

/*************************Segment_is_mapped************************************
 *
 * Check if there exists a mapped segment at provided identifier
 * Inputs:
 *         Segment_T seg: The Segment_T that contains segments
 *         uint32_t id:   The identifier associated with the segment
 * Return: Boolean indicating whether a mapped segment at provided identifier
 *         exists
 * Expects:
 *         Segment_T seg to be non-null
 *         uint32_t id to be less than the length of the sequence of segments
 * Notes:
 *         CRE if Segment_T seg is null
 *         CRE if uint32_t id is greater than the length of the sequence of 
 *         segments
 *****************************************************************************/
bool Segment_is_mapped(Segment_T seg, uint32_t id)
{
        /* Ensuring non-null value was passed in and that id is in range */
        assert(seg);
        
        unsigned length = Seq_length(seg->segments);
        assert(seg && id < length);

        /* Check if line is mapped */
        line_T line = Seq_get(seg->segments, id);
        if (line == NULL) {
                return false;
        }

        return true;
}