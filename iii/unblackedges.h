/*unblackedges.h
* written by Matthew Carey and Kaelen Song
* HW02: iii
*
* Summary: defines the methods for unblackedges program, which will be 
*     implmented in unblack.c. This program removes any black edges from
*     a pbm file
*/

#ifndef UNBLACKEDGES_INCLUDED
#define UNBLACKEDGES_INCLUDED

#include "bit2.h"
#include <stdlib.h> 
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

void read_file_header(FILE *file_p, int *width, int *height);
static FILE *open_or_fail(char *filename, char *mode);
void unblack_edges(FILE *file_p);
void pbmread(FILE *file_p, Bit2_T array, int WIDTH, int HEIGHT);
void remove_black_edges(Bit2_T array);
void depth_first_removal(Bit2_T array, int col, int row);

#endif