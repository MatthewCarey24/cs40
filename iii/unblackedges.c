/*unblackedges.c
* written by Matthew Carey and Kaelen Song
* HW02: iii
*
* Summary: Implements methods for a program unblack edges which removes 
*          any black edges from a pbm file
*/

#include "unblackedges.h"
#include "bit2.h"
#include "stack.h"
#include <assert.h>

int main(int argc, char *argv[])
{
    assert(argc < 3);

    if (argc == 2) {
        FILE *file_p = open_or_fail(argv[1], "r");
        unblack_edges(file_p);
    }
    else {
        unblack_edges(stdin);
    }
}
/********** unblack_edges ********
 *
 * calls all the appropriate functions to remove black edges from pgm file
 * outputs the final result
 * Parameters:
 *       FILE *file_p: a pointer to the pbm file with black edges
 * Return: 
 *      none
 * Expects
 *      none
 ************************/
void unblack_edges(FILE *file_p)
{
    int WIDTH, HEIGHT = 0;
    read_file_header(file_p, &WIDTH, &HEIGHT);

    Bit2_T array = Bit2_new(WIDTH, HEIGHT);
    pbmread(file_p, array, WIDTH, HEIGHT);

    remove_black_edges(array);

/*printing the output*/
    fprintf(stdout, "P1\n%d %d\n", array->width, array->height);

    for (int row = 0; row < array->height; row ++) {
        for (int col = 0; col < array->width; col ++) {
            fprintf(stdout, "%d", Bit2_get(array, col, row));
        }
        fprintf(stdout, "\n");
    }
    fclose(file_p);
    Bit2_free(&array);
}

/********** read_file_header ********
 *
 * reads the file header of the sudoku board, storing its width and height
 * 
 * Parameters:
 *       FILE *file_p: a pointer to the file with the sudoku solution
 *       int *width: a pointer to an integer that will store the width of 
 *       the sudoku board
 *       int *height: a pointer to an integer that will store the height of 
 *       the sudoku board
 * Return: 
 *      nothing directly, indirectly returns the width and height of the board
 * Expects
 *      none
 ************************/
void read_file_header(FILE *file_p, int *width, int *height)
{
    char magic[3];
    fscanf(file_p, "%2s %d %d", magic, width, height);
}

/********** pbmread ********
 *
 * reads the pbm file, putting the sudoku board into the array
 * 
 * Parameters:
 *       FILE *file_p: a pointer to the file with sudoku board
 *       Bit2_T array: a pointer to the UArray
 *       int WIDTH: the width of the board
 *       int HEIGHT: the height of the board
 * Return: 
 *      none
 * Expects
 *      none
 ************************/
void pbmread(FILE *file_p, Bit2_T array, int WIDTH, int HEIGHT)
{
    char tmp = 0;
    int col = 0;
    int row = 0;
    /*interate through file row major*/
    while (row < HEIGHT && !feof(file_p)) {
        col = 0;
        while (col < WIDTH) {
            tmp = fgetc(file_p);
            /*add the numeric value of 1s or 0s to array*/
            if (tmp == 48 || tmp == 49) {
                Bit2_put(array, col, row, tmp - 48);
                col ++;
            }
        }
        row ++;
    }
}

/********** remove_black_edges ********
 *
 * re
 * 
 * Parameters:
 *       Bit2_T array: a pointer to the UArray
 * Return: 
 *      none
 * Expects
 *      none
 ************************/
void remove_black_edges(Bit2_T array)
{
    // calls depth first removal on all edges on top and bottom
    for (int col = 0; col < array->width; col ++) {
        for (int row = 0; row < array->height; row += array->height - 1) {
            if (Bit2_get(array, col, row) == 1) {
                depth_first_removal(array, col, row);
            }   
        }
    }
   // calls depth first removal on all edges on left and right
    for (int row = 0; row < array->height; row ++) {
        for (int col = 0; col < array->width; col += array->width - 1) {
           if (Bit2_get(array, col, row) == 1) {
                depth_first_removal(array, col, row);
            }
        }
    }
}

/********** depth_first_removal ********
 * 
 * removes all black pixels attached to a given starting pixel 
 * 
 * 
 * Inputs: 
 *         Bit2_T array: the 2D array of bits
 *         int Startcol: the column of the first pixel
 *         int Startrow: the row of the first pixel 
 *		
 * Return: none
 ************************/
void depth_first_removal(Bit2_T array, int Startcol, int Startrow)
{
    Stack_T stack = Stack_new();
    
    /*store the 1D row major index of black pixels on the stack*/
    int *index = malloc(sizeof(int));
    assert(index != NULL);
    *index = row_major_index(array, Startcol, Startrow);
    Stack_push(stack, index);

    while (!Stack_empty(stack)) {
        /*retrive the 2D index from the top of the stack and change it to 0*/
        int *top = Stack_pop(stack);
        int col = col_from_row_major(array, *top);
        int row = row_from_row_major(array, *top);
        free(top);
        Bit2_put(array, col, row, 0);
        
        /*check the pixel to the left of current*/
        if (col - 1 > 0 && Bit2_get(array, col - 1, row) == 1) {
            int *pushVal1 = malloc(sizeof(int));
            assert(pushVal1 != NULL);
            *pushVal1 = row_major_index(array, col - 1, row);
            Stack_push(stack, pushVal1);
        }
        /*check pixel to the right of current*/
        if (col + 1 < array->width && Bit2_get(array, col + 1, row) == 1) {
            int *pushVal2 = malloc(sizeof(int));
            assert(pushVal2 != NULL);
            *pushVal2 = row_major_index(array, col + 1, row);
            Stack_push(stack, pushVal2);
        }
        /*check pixel below the current*/
        if (row - 1 > 0 && Bit2_get(array, col, row - 1) == 1) {
            int *pushVal3 = malloc(sizeof(int));
            assert(pushVal3 != NULL);
            *pushVal3 = row_major_index(array, col, row - 1);
            Stack_push(stack, pushVal3);
        }
        /*check the pixel above the current*/
        if (row + 1 < array->height && Bit2_get(array, col, row + 1) == 1) {
            int *pushVal4 = malloc(sizeof(int));
            assert(pushVal4 != NULL);
            *pushVal4 = row_major_index(array, col, row + 1);
            Stack_push(stack, pushVal4);
        }
        
    } 
    Stack_free(&stack);
}

/********** open_or_fail ********
 * 
 * tests that the given file can be opened, 
 * then opens the file if possible
 * 
 * Inputs: 
 *         char *filename: the filename
 *         char *mode: reading
 *		
 * Return: the pointer to the opened file
 * 
 * Notes:
 *    Will CRE file cannot be opened
 ************************/
static FILE *open_or_fail(char *filename, char *mode)
{
        FILE *fp = fopen(filename, mode);
        assert(fp != NULL);
        return fp;
}