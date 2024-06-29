
/******************************************************************************
 *
 *                               ppmtrans.c
 *
 *     Assignment: locality
 *     Authors:    Marten Tropp and Matt Carey
 *     Date:       10/10/2023
 *      
 *     The purpose of this file is to act as a driver program for the
 *     ppmtrans program. It takes in a raw or plain ppm file (P3 or P6),
 *     and performs the given operation on it, either rotating, flipping, or
 *     transposing. It then prints out a raw transformed image to standard 
 *     output. 
 *
 *    
 *
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "mem.h"
#include "cputiming.h"

Pnm_ppm Pnm_ppm_new(unsigned width, unsigned height, 
                    unsigned denominator, A2Methods_T methods);
FILE *openForReading(char filename[]);
Pnm_ppm rotate(Pnm_ppm image, A2Methods_T methods, 
               char time_file[], int rotation);
Pnm_ppm flip(Pnm_ppm image, A2Methods_T methods, 
             char time_file[], char *flip_type);
Pnm_ppm transpose(Pnm_ppm image, A2Methods_T methods, char time_file[]);
void helper90(int col, int row, A2Methods_UArray2 arr, void *elem, void *cl);
void helper180(int col, int row, A2Methods_UArray2 arr, void *elem, void *cl);
void helper270(int col, int row, A2Methods_UArray2 arr, void *elem, void *cl);
void helperHori(int col, int row, A2Methods_UArray2 arr, void *elem, void *cl);
void helperVert(int col, int row, A2Methods_UArray2 arr, void *elem, void *cl);
void helperTran(int col, int row, A2Methods_UArray2 arr, void *elem, void *cl);
CPUTime_T startTimer(char *time_file);
void stopTimer(char *time_file, CPUTime_T timer);

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (false)

static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s "
                "[{-rotate <angle>,-flip <{vertical,horizontal}>,-transpose}]"
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}

int main(int argc, char *argv[]) 
{
        char *time_file_name = NULL;
        bool commandGiven = false;
        (void) time_file_name;
        char *flip_type = NULL;
        (void) flip_type;
        bool transp = false;
        (void) transp;
        int rotation = 0;
        int i;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default;
        assert(map);

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (commandGiven) {
                                usage(argv[0]);
                        }
                        commandGiven = true;
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-transpose") == 0) {
                        if (commandGiven) {
                                usage(argv[0]);
                        }
                        commandGiven = true;
                        transp = true;
                } else if (strcmp(argv[i], "-flip") == 0) {
                        if (commandGiven) {
                                usage(argv[0]);
                        }
                        commandGiven = true;
                        if (!(i + 1 < argc)) {      /* no flip value */
                                usage(argv[0]);
                        }
                        flip_type = argv[++i];
                        if (strcmp(flip_type, "horizontal") != 0 &&
                            strcmp(flip_type, "vertical") != 0) {
                                fprintf(stderr, 
                                        "Flip must be horizontal or "
                                         "vertical\n");
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        usage(argv[0]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        break;
                }
        }

        /* Opening image file that will be read */
        FILE *fp = openForReading(argv[i]);
        /* Reading contents from the image file */
        Pnm_ppm image = Pnm_ppmread(fp, methods);
        Pnm_ppm result = NULL;
        if (flip_type != NULL) {
                result = flip(image, methods, time_file_name, flip_type);
        }
        else if (transp) {
                result = transpose(image, methods, time_file_name);
        }
        else if (rotation == 0) {
                CPUTime_T timer = startTimer(time_file_name);
                stopTimer(time_file_name, timer);
                Pnm_ppmwrite(stdout, image);

        }
        else if (rotation == 90 || rotation == 180 || rotation == 270) {
                result = rotate(image, methods, time_file_name, rotation);
        }

        /* Freeing memory and closing file */
        if (result != NULL) {
                Pnm_ppmwrite(stdout, result);
                Pnm_ppmfree(&result);
        }
        fclose(fp);
        Pnm_ppmfree(&image);
        
        exit(0);
}

/**********************************openForReading******************************
 *
 * Open a file for reading or if filename is not provided read input from
 * standard input.
 * Inputs:
 *         char filename[]: A char array representing the name of a file
 * Return: A file pointer to the opened file passed to the command line,
 *         assuming it was opened with no problems
 * Expects:
 *         char filename[] to contain the name of a openable file
 * Notes:
 *         CRE if there is an error opening the file
 *                      
 *****************************************************************************/
FILE *openForReading(char filename[]) 
{
        /* Opening file */
        FILE *fp = NULL;
        if (filename != NULL) {
                fp = fopen(filename, "rb");

                /* File couldn't be opened */
                if (fp == NULL) {
                        fprintf(stderr, "Could not open provided file.\n");
                        exit(1);
                }
        }

        /* Reading input from standard input */
        else {
                fp = stdin;
        }

        return fp;
}

/**********************************rotate**************************************
 *
 * Rotate a provided image by a specific degree that is also given.
 * Inputs:
 *         Pnm_ppm image: A pointer to a Pnm_ppm struct that contains the data
 *         on the original image that will be manipulated
 *         A2Methods_T methods: A pointer to a A2Methods_T struct that contains
 *         the methods used for storing and handling data
 *         char time_file[]: A char array representing the name of a file that
 *         the timing information should be stored in
 *         int rotation: The angle of the rotation that was requested
 * Return: A pointer to a Pnm_ppm struct that contains the result of the 
 *         rotation
 * Expects:
 *         int rotation to be either 90, 180, or 270
 * Notes:
 *         The value of rotation is already checked in the for loop in main
 *                      
 *****************************************************************************/
Pnm_ppm rotate(Pnm_ppm image, A2Methods_T methods, char time_file[],
               int rotation)
{
        /* Allocating memory for result image based on rotation value */
        Pnm_ppm result;
        if (rotation == 90 || rotation == 270) {
                result = Pnm_ppm_new(image->height, image->width, 
                                     image->denominator, methods);
        }
        else {
                result = Pnm_ppm_new(image->width, image->height, 
                                     image->denominator, methods);
        }
        
        /* Start the timer */
        CPUTime_T timer = startTimer(time_file);

        /* Rotate based on the rotation provided */
        if (rotation == 90) {
                image->methods->map_default(image->pixels, helper90, result);
        }
        else if (rotation == 180) {
                image->methods->map_default(image->pixels, helper180, result);
        }
        else if (rotation == 270) {
                image->methods->map_default(image->pixels, helper270, result);
        }

        /* Stop timer and write to file if timing was requested */
        stopTimer(time_file, timer);

        return result;
}

/**********************************flip****************************************
 *
 * Flip a provided image horizontally or vertically.
 * Inputs:
 *         Pnm_ppm image: A pointer to a Pnm_ppm struct that contains the data
 *         on the original image that will be manipulated
 *         A2Methods_T methods: A pointer to a A2Methods_T struct that contains
 *         the methods used for storing and handling data
 *         char time_file[]: A char array representing the name of a file that
 *         the timing information should be stored in
 *         char *flip_type: A char array containing the type of flip that was
 *         requested
 * Return: A pointer to a Pnm_ppm struct that contains the result of the flip
 * Expects:
 *         char *flip_type to be either "horizontal" or "vertical"
 * Notes:
 *         char *flip_type is already checked in the for loop in main
 *                      
 *****************************************************************************/
Pnm_ppm flip(Pnm_ppm image, A2Methods_T methods, char time_file[], 
             char *flip_type)
{
        /* Allocating memory for result image based on rotation value */
        Pnm_ppm result;
        result = Pnm_ppm_new(image->width, image->height, 
                             image->denominator, methods);

        /* Start the timer */
        CPUTime_T timer = startTimer(time_file);

        /* Rotate based on the rotation provided */
        if (strcmp(flip_type, "horizontal") == 0) {
                image->methods->map_default(image->pixels, helperHori, result);
        }
        else if (strcmp(flip_type, "vertical") == 0) {
                image->methods->map_default(image->pixels, helperVert, result);
        }

        /* Stop timer and write to file if timing was requested */
        stopTimer(time_file, timer);

        return result;
} 

/**********************************transpose***********************************
 *
 * Transpose a provided image.
 * Inputs:
 *         Pnm_ppm image: A pointer to a Pnm_ppm struct that contains the data
 *         on the original image that will be manipulated
 *         A2Methods_T methods: A pointer to a A2Methods_T struct that contains
 *         the methods used for storing and handling data
 *         char time_file[]: A char array representing the name of a file that
 *         the timing information should be stored in
 * Return: A pointer to a Pnm_ppm struct that contains the result of the flip
 * Expects:
 *         none
 * Notes:
 *         none
 *                      
 *****************************************************************************/
Pnm_ppm transpose(Pnm_ppm image, A2Methods_T methods, char time_file[])
{
        /* Allocating memory for result image based on rotation value */
        Pnm_ppm result = Pnm_ppm_new(image->height, image->width, 
                                     image->denominator, methods);
        
        /* Start the timer */
        CPUTime_T timer = startTimer(time_file);

        /* Rotate based on the rotation provided */
        image->methods->map_default(image->pixels, helperTran, result);

        /* Stop timer and write to file if timing was requested */
        stopTimer(time_file, timer);

        return result;
}

/**********************************helperHori**********************************
 *
 * Put an element from the original image to a position in the result image
 * Inputs:
 *         int col: The column position of elem in A2Methods_UArray2 arr
 *         int row: The row position of elem in A2Methods_UArray2 arr
 *         A2Methods_UArray2 arr: A 2D array that contains elem at position
 *         (col, row)
 *         void *elem: The element at location (col, row) in 
 *         A2Methods_UArray2 arr that is put into the result Pnm_ppm struct
 *         passed in through void *cl
 *         void *cl: Closure pointer that in this case contains a pointer to a
 *         Pnm_ppm struct that will be used to store the result of the flip
 * Return: none
 * Expects:
 *         none
 * Notes:
 *         helperHori is a helper function that is called in flip while
 *         mapping through the data in Pnm_ppm image while rotating 
 *         horizontally
 *                      
 *****************************************************************************/
void helperHori(int col, int row, A2Methods_UArray2 arr, void *elem, void *cl)
{
        /* Accessing location where elem needs to be put in result */
        Pnm_ppm result = cl;
        struct Pnm_rgb *c = result->methods->at(result->pixels, 
                                         result->width - col - 1, row);
        
        /* Placing element in correct location */
        *c = *(struct Pnm_rgb *)elem;
        (void) arr;
}

/**********************************helperVert**********************************
 *
 * Put an element from the original image to a position in the result image
 * Inputs:
 *         int col: The column position of elem in A2Methods_UArray2 arr
 *         int row: The row position of elem in A2Methods_UArray2 arr
 *         A2Methods_UArray2 arr: A 2D array that contains elem at position
 *         (col, row)
 *         void *elem: The element at location (col, row) in 
 *         A2Methods_UArray2 arr that is put into the result Pnm_ppm struct
 *         passed in through void *cl
 *         void *cl: Closure pointer that in this case contains a pointer to a
 *         Pnm_ppm struct that will be used to store the result of the flip
 * Return: none
 * Expects:
 *         none
 * Notes:
 *         helperVert is a helper function that is called in flip while
 *         mapping through the data in Pnm_ppm image while rotating vertically
 *                      
 *****************************************************************************/
void helperVert(int col, int row, A2Methods_UArray2 arr, void *elem, void *cl)
{
        /* Accessing location where elem needs to be put in result */
        Pnm_ppm result = cl;
        struct Pnm_rgb *c = result->methods->at(result->pixels, 
                                         col, result->height - row - 1);

        /* Placing element in correct location */
        *c = *(struct Pnm_rgb *)elem;
        (void) arr;
}

/**********************************helperTran**********************************
 *
 * Put an element from the original image to a position in the result image
 * Inputs:
 *         int col: The column position of elem in A2Methods_UArray2 arr
 *         int row: The row position of elem in A2Methods_UArray2 arr
 *         A2Methods_UArray2 arr: A 2D array that contains elem at position
 *         (col, row)
 *         void *elem: The element at location (col, row) in 
 *         A2Methods_UArray2 arr that is put into the result Pnm_ppm struct
 *         passed in through void *cl
 *         void *cl: Closure pointer that in this case contains a pointer to a
 *         Pnm_ppm struct that will be used to store the result of the flip
 * Return: none
 * Expects:
 *         none
 * Notes:
 *         helperTran is a helper function that is called in transpose while
 *         mapping through the data in Pnm_ppm image while transposing
 *                      
 *****************************************************************************/
void helperTran(int col, int row, A2Methods_UArray2 arr, void *elem, void *cl)
{
        /* Accessing location where elem needs to be put in result */
        Pnm_ppm result = cl;
        struct Pnm_rgb *c = result->methods->at(result->pixels, row, col);
        
        /* Placing element in correct location */
        *c = *(struct Pnm_rgb *)elem;
        (void) arr;   
}

/**********************************helper90************************************
 *
 * Put an element from the original image to a position in the result image
 * Inputs:
 *         int col: The column position of elem in A2Methods_UArray2 arr
 *         int row: The row position of elem in A2Methods_UArray2 arr
 *         A2Methods_UArray2 arr: A 2D array that contains elem at position
 *         (col, row)
 *         void *elem: The element at location (col, row) in 
 *         A2Methods_UArray2 arr that is put into the result Pnm_ppm struct
 *         passed in through void *cl
 *         void *cl: Closure pointer that in this case contains a pointer to a
 *         Pnm_ppm struct that will be used to store the result of the flip
 * Return: none
 * Expects:
 *         none
 * Notes:
 *         helper90 is a helper function that is called in rotate while
 *         mapping through the data in Pnm_ppm image while rotating
 *                      
 *****************************************************************************/
void helper90(int col, int row, A2Methods_UArray2 arr, void *elem, void *cl)
{
        /* Accessing location where elem needs to be put in result */
        Pnm_ppm result = cl;
        struct Pnm_rgb *c = result->methods->at(result->pixels, 
                                         result->width - row - 1, col);
        
        /* Placing element in correct location */
        *c = *(struct Pnm_rgb *)elem;
        (void) arr;
}

/**********************************helper180***********************************
 *
 * Put an element from the original image to a position in the result image
 * Inputs:
 *         int col: The column position of elem in A2Methods_UArray2 arr
 *         int row: The row position of elem in A2Methods_UArray2 arr
 *         A2Methods_UArray2 arr: A 2D array that contains elem at position
 *         (col, row)
 *         void *elem: The element at location (col, row) in 
 *         A2Methods_UArray2 arr that is put into the result Pnm_ppm struct
 *         passed in through void *cl
 *         void *cl: Closure pointer that in this case contains a pointer to a
 *         Pnm_ppm struct that will be used to store the result of the flip
 * Return: none
 * Expects:
 *         none
 * Notes:
 *         helper180 is a helper function that is called in rotate while
 *         mapping through the data in Pnm_ppm image while rotating
 *                      
 *****************************************************************************/
void helper180(int col, int row, A2Methods_UArray2 arr, void *elem, void *cl)
{
        /* Accessing location where elem needs to be put in result */
        Pnm_ppm result = cl;
        struct Pnm_rgb *c = result->methods->at(result->pixels, 
                                         result->width - col - 1, 
                                         result->height - row - 1);
        
        /* Placing element in correct location */
        *c = *(struct Pnm_rgb *)elem;
        (void) arr;
}

/**********************************helper270***********************************
 *
 * Put an element from the original image to a position in the result image
 * Inputs:
 *         int col: The column position of elem in A2Methods_UArray2 arr
 *         int row: The row position of elem in A2Methods_UArray2 arr
 *         A2Methods_UArray2 arr: A 2D array that contains elem at position
 *         (col, row)
 *         void *elem: The element at location (col, row) in 
 *         A2Methods_UArray2 arr that is put into the result Pnm_ppm struct
 *         passed in through void *cl
 *         void *cl: Closure pointer that in this case contains a pointer to a
 *         Pnm_ppm struct that will be used to store the result of the flip
 * Return: none
 * Expects:
 *         none
 * Notes:
 *         helper270 is a helper function that is called in rotate while
 *         mapping through the data in Pnm_ppm image while rotating
 *                      
 *****************************************************************************/
void helper270(int col, int row, A2Methods_UArray2 arr, void *elem, void *cl) 
{
        /* Accessing location where elem needs to be put in result */
        Pnm_ppm result = cl;
        struct Pnm_rgb *c = result->methods->at(result->pixels, 
                                         row, result->height - col - 1);
        
        /* Placing element in correct location */
        *c = *(struct Pnm_rgb *)elem;
        (void) arr;
}

/**********************************Pnm_ppm_new*********************************
 *
 * Create a new Pnm_ppm struct with the given dimensions, denominator, and
 * methods
 * Inputs:
 *         unsigned width: The number of columns in the A2Methods_UArray2 
 *         pixels 2D array
 *         unsigned height: The number of rows in the A2Methods_UArray2 pixels
 *         2D array
 *         unsigned denominator: The maximum color value in the
 *         A2Methods_UArray2 pixels 2D array
 *         A2Methods_T methods: A pointer to a A2Methods_T struct that contains
 *         the methods used for storing and handling data
 * Return: A pointer to a Pnm_ppm struct with the provided dimensions,
 *         denominator, and methods
 * Expects:
 *         A2Methods_T methods to be non-null
 * Notes:
 *         CRE if A2Methods_T methods is NULL
 *                      
 *****************************************************************************/
Pnm_ppm Pnm_ppm_new(unsigned width, unsigned height, unsigned denominator,      
                    A2Methods_T methods)
{
        assert(methods != NULL);

        /* Allocating memory */
        Pnm_ppm newResultA2;
        NEW(newResultA2);

        /* Assigning initial values */
        newResultA2->pixels = methods->new(width, height, 
                                           sizeof(struct Pnm_rgb));
        newResultA2->methods = methods;
        newResultA2->height = height;
        newResultA2->width = width;
        newResultA2->denominator = denominator;
        
        return newResultA2;
}

/**********************************startTimer**********************************
 *
 * Initiate a new CPUTime_T and start a timer that will measure the time it
 * takes to transform an image
 * Inputs:
 *         char *time_file: A char array representing the name of a file that
 *         should store the information about timing, but, in thi case, is used
 *         to check whether timing was requested
 * Return: A pointer to a CPUTime_T struct that has a running timer
 * Expects:
 *         none
 * Notes:
 *         none
 *                      
 *****************************************************************************/
CPUTime_T startTimer(char *time_file)
{
        CPUTime_T timer = NULL;

        /* Start a timer if a time_file was given */
        if (time_file != NULL) {
                /* Allocate memory for a CPUTime_T */
                timer = CPUTime_New();

                /* Start the timer */
                CPUTime_Start(timer);
        }

        return timer;
}

/**********************************stopTimer***********************************
 *
 * Stop a timer, write the timing information into a file whose name was
 * provided, and free memory associated with the CPUTime_T that was provided
 * Inputs:
 *         char *time_file: A char array representing the name of a file that
 *         should store the information about timing, but, in thi case, is used
 *         to check whether timing was requested
 *         CPUTime_T timer: A pointer to a CPUTime_T struct that has a started
 *         timer or NULL if timing was not requested
 * Return: none
 * Expects:
 *         char *time_file to either contain a NULL or the name of a openable
 *         file
 * Notes:
 *         CRE if file could not be opened but name was provided
 *                      
 *****************************************************************************/
void stopTimer(char *time_file, CPUTime_T timer)
{
        /* Check if timing was requested */
        if (time_file != NULL) {
                /* Stop timer */
                double time_used = CPUTime_Stop(timer);

                /* Open file to store time_used */
                FILE *tfp = fopen(time_file, "w");
                assert(tfp != NULL);

                /* Write to file and close it */
                fprintf(tfp, "%f", time_used);
                fclose(tfp);

                /* Free memory associated with the timer */
                CPUTime_Free(&timer);
        }
}