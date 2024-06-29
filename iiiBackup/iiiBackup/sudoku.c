/*sudoku.c
* written by Matthew Carey and Kaelen Song
* HW02: iii
*
* Summary: creates an executable program ./sudoku which acts as a 
           predicate to determine whether or not a file representing 
*          a sudoku board is valid solution or not. 
*          Implements methods defined in sudoku.h
*/

#include "sudoku.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include "uarray2.h"
#include <stdbool.h> 

#define WIDTH 9
#define HEIGHT 9
 
int main(int argc, char *argv[]) 
{
    assert(argc < 3);

    if (argc == 2) {
        FILE *file_p = open_or_fail(argv[1], "r");
        return verify_sudoku_solution(file_p);
    }
    else {
        return verify_sudoku_solution(stdin);
    }

}

/********** int verify_sudoku_solution ********
 *
 * determines the validity of a given sudoku board
 *
 * Parameters: 
 *      FILE *file_p: file containing the data of the sudoku board  
 * Return: 
 *      An int as a predicate of the validity of the board               
 * Expects
 *      a completed sudoku board
 * Notes:
 *      none
 *      
 *      
 ************************/
int verify_sudoku_solution(FILE *file_p) 
{
    UArray2_T array = UArray2_new(WIDTH, HEIGHT, sizeof(char));
    
    read_file_header(file_p);
    read_sol_into_table(file_p, array);
    fclose(file_p);
    if (no_col_dups(array) && no_row_dups(array) && no_submap_dups(array)) {
        UArray2_free(&array);
        return 0;
    }
    else{
        UArray2_free(&array);
        return 1;
    } 

    
}

/********** no_col_dups ********
 *
 * determine whether or not there are duplicates in any columns of the board
 *
 * Parameters: 
 *      UArray2_T array: UArray2 instance containing the values of a board  
 * Return: 
 *      A boolean indicating if there are no duplicates in the columns               
 * Expects
 *      a completed sudoku board, width & height = 9
 * Notes:
 *      despite triple nested for loops, is constant time
 *      
 *      
 ************************/
bool no_col_dups(UArray2_T array) 
{
    for (int col = 0; col < WIDTH; col ++) {
        for (int row = 0; row < HEIGHT - 1; row ++) {
            for (int i = row + 1; i < HEIGHT; i ++) {
                if (*(char *)UArray2_at(array, col, row) == 
                    *(char*)UArray2_at(array, col, i)) {
                    return false;
                }
            }
        }
    }
    return true;
}

/********** no_row_dups ********
 *
 * determine whether or not there are duplicates in any rows of the board
 *
 * Parameters: 
 *      UArray2_T array: UArray2 instance containing the values of a board  
 * Return: 
 *      A boolean indicating if there are no duplicates in the rows               
 * Expects
 *      a completed sudoku board, width & height = 9
 * Notes:
 *      despite triple nested for loops, is constant time
 *      
 *      
 ************************/
bool no_row_dups(UArray2_T array) 
{
    for (int row = 0; row < HEIGHT; row ++) {
        for (int col = 0; col < WIDTH - 1; col ++) {
            for (int i = col + 1; i < WIDTH; i ++) {
                if (*(char *)UArray2_at(array, col, row) == *(char*)UArray2_at(array, i, row)) {
                    return false;
                }
            }
        }
    }
    return true;
}

/********** no_submap_dups ********
 *
 * determine whether or not there are duplicates in any submaps of the board
 *
 * Parameters: 
 *      UArray2_T array: UArray2 instance containing the values of a board  
 * Return: 
 *      A boolean indicating if there are no duplicates in the submaps               
 * Expects
 *      a completed sudoku board, width & height = 9
 * Notes:
 *      calls no_submap_dups_helper as a helper function
 *      
 *      
 ************************/
bool no_submap_dups(UArray2_T array) 
{
    bool no_dups = true;
    for (int startCol = 0; startCol <= 6; startCol += 3) {
        for (int startRow = 0; startRow <= 6; startRow += 3) {
            no_dups = no_submap_dups_helper(array, startCol, startRow);
            if (!no_dups) {
                return false;
            }
        }
    }
    return true;
}

/********** no_submap_dups_helper ********
 *
 * determine whether or not there are duplicates in any submaps of the board
 *
 * Parameters: 
 *      UArray2_T array: UArray2 instance containing the values of a board  
 * Return: 
 *      A boolean indicating if there are no duplicates in the submaps               
 * Expects
 *      a completed sudoku board, width & height = 9
 * Notes:
 *      is the helper function of no_submap_dups
 *      
 *      
 ************************/
bool no_submap_dups_helper(UArray2_T array, int startCol, int startRow) 
{
    bool seen[10] = {false};
    for (int col = startCol; col < startCol + 3; col ++) {
        for (int row = startRow; row < startRow + 3; row ++) {
            if (seen[(*(char *)UArray2_at(array, col, row)) - 48]) {
                return false;
            }
            seen[*(char *)UArray2_at(array, col, row) - 48] = true;
        }
    }
    return true;
}

/********** read_file_header ********
 *
 * read in the header of the pnm file and store the data 
 *
 * Parameters: 
 *      FILE *file_p: file containing data of sudoku board  
 * Return: 
 *      void               
 * Expects
 *      a valid pnm file object with header
 * Notes:
 *      makes assertions about height and width read in
 *      
 *      
 ************************/
void read_file_header(FILE *file_p) 
{
    char magic[3];
    int width, height, maxVal;
    fscanf(file_p, "%2s %d %d %d", magic, &width, &height, &maxVal);

    assert(width == WIDTH);
    assert(height == HEIGHT);
    
}

/********** read_sol_into_table ********
 *
 * read in the sudoku table from the pnm file and store the data 
 *
 * Parameters: 
 *      FILE *file_p: file containing data of sudoku board  
 * Return: 
 *      void               
 * Expects
 *      a valid pnm file object containing a complete sudoku board
 * Notes:
 *      none
 *      
 *      
 ************************/
void read_sol_into_table(FILE *file_p, UArray2_T array) 
{
    char tmp = 0;
    int col = 0;
    int row = 0;
    while (row < HEIGHT && !feof(file_p)) {
        col = 0;
        while (col < WIDTH) {
            tmp = fgetc(file_p);
            assert(tmp != 48 && !(tmp > 57)); //checks max val is 9 and no 0s
            if (tmp > 48 && tmp < 58) {
                *(char *)UArray2_at(array, col, row) = tmp;
                col ++;
            }
        }
        row ++;
    }
}

/********** open_or_fail ********
 * 
 * tests that the given file can be opened, 
 * then opens the file if possible
 * 
 * Inputs: 
 *         char *filename: the filename
 *         char *mode: 
 *		
 * Return: the pointer to the opened file
 * 
 * Notes:
 *    Will CRE file cannot be opened
 ************************/
static FILE *open_or_fail(char *filename, char *mode) 
{
        FILE *fp = fopen(filename, mode);
        assert(fp);
        return fp;
}