/*sudoku.h
* written by Matthew Carey and Kaelen Song
* HW02: iii
*
* Summary: defines the methods for sudoku program, which will be implmented
*     in sudoku.c. This program determines whether or not a file representing 
*          a sudoku board is valid solution or not
*/

#ifndef SUDOKU_INCLUDED
#define SUDOKU_INCLUDED

#include <stdlib.h> 
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include "uarray2.h"

static FILE *open_or_fail(char *filename, char *mode);
int verify_sudoku_solution(FILE *file_p);
void read_sol_into_table(FILE *file_p, UArray2_T array);
void read_file_header(FILE *file_p);
bool no_col_dups(UArray2_T array);
bool no_row_dups(UArray2_T array);
bool no_submap_dups(UArray2_T array);
bool no_submap_dups_helper(UArray2_T array, int col, int row);
void checkRowDuplicates(int col, int row, UArray2_T array, void *currValue, 
                        void *cl);

#endif