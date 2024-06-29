/*restoration.c
* written by Matthew Carey and Isaac Lopez
* HW01: filesofpix
*
* Summary: given a corruputed pgm file, restoration will decorrupt the image and
* output the data to standard output in a P5 raw format
*/



#include <stdio.h>
#include <readaline.h>
#include <stdlib.h>
#include "table.h"
#include "list.h"  
#include "atom.h"
#include "string.h"
#include "seq.h"
#include "assert.h"
#include "pnmrdr.h"
#include <except.h>

// stores a char pointer (string) and its size
struct Line {
    char *line;
    size_t size;
};

void decorrupt(FILE *file);
struct Line *getSeq(char *line, size_t size);
char *getCorrectSeq(FILE *file, Seq_T lines, size_t *length);
void freeTableElements(Table_T table);
void applyFree(const void *key, void **value, void *cl);
void printOutput(Seq_T lines);
char *removeChars(char *line, size_t size);
char *getascii(char *p2lines, int size, int *w);

/*
 * name:      main 
 * purpose:   open a pnm file and decorrupt the image
 * arguments: command line arguements for file
 * returns:   int success
 */
int main(int argc, char *argv[]){
    FILE *file = NULL;
    
    assert(argc < 3);
    if(argc == 1){
        decorrupt(stdin);
    }
    else if(argc == 2){
        file = fopen(argv[1], "r");
        // check for read error
        assert(file != NULL);
        decorrupt(file);
    }
    exit(EXIT_SUCCESS);
}

/*
 * name:      decorrupt 
 * purpose:   decorrupt a given image pnm file
 * arguments: pnm file to decorrupt
 * returns:   void
 */
// builds a Seq of the correct lines and builds the raw pnm
void decorrupt(FILE *file){
    // create a sequence that will store line structs of original raster lines
    Seq_T lines = Seq_new(1);
    size_t length = 0;
    // get the correct sequence using getCorrectSeq
    char *correctSeq = getCorrectSeq(file, lines, &length);

    struct Line *currLine;

    // after getCorrectSeq has found first 2 lines, continue iterating through 
    // file comparing for lines with correct sequence
    while(!feof(file)){
        currLine = malloc(sizeof(struct Line));
        assert(currLine != NULL);

        currLine->size = readaline(file, &currLine->line);
        
        struct Line *currSeq = getSeq(currLine->line, currLine->size);
        
        if(Atom_new(currSeq->line, currSeq->size) == 
        Atom_new(correctSeq, length)){
            Seq_addhi(lines, currLine);
        }
    }   
    // use complete line sequence to print the output
    printOutput(lines);
    int length2 = Seq_length(lines);
    for(int i = 0; i < length2; i ++){
        struct Line *tmp = (struct Line *) Seq_remhi(lines);
        free(tmp->line);
        free(tmp);
    }

    Seq_free(&lines);
}

/*
 * name:      printOutput 
 * purpose:   prints the decorrupted file to output in P5
 * arguments: sequence of correct lines 
 * returns:   void
 * effects: prints to output
 */
void printOutput(Seq_T lines){
    // create iterator struct
    struct Line *currStruct = malloc(sizeof(struct Line));
    assert(currStruct != NULL);
    // determine the size of total lines in p2 data
    int p2size = 0;
    for(int x = 0; x < Seq_length(lines); x ++){
        currStruct = (struct Line *) Seq_get(lines, x);
        p2size += currStruct->size;
    }
    // create array of p2 data
    char *p2lines = malloc(p2size);
    int counter = 0;

    // iterate through each sequence
    for(int i = 0; i < Seq_length(lines); i ++){
        currStruct = (struct Line *) Seq_get(lines, i);
        // iterate through each struct line and add each char to p2 array
        for(size_t j = 0; j < currStruct->size; j ++){
            p2lines[counter] = 
            removeChars(currStruct->line, currStruct->size)[j];
            counter ++;
        }
    }
    int width = 0;
    int height = 0;
    // call get ascii to create a array of the p5 data from p2 data
    char *p5lines = getascii(p2lines, p2size, &width);
    height = Seq_length(lines);
    // width is currently the total number of elements in image, divided by 
    // height gives width
    width = width / height;

    // check for invalid image file
    assert(height >= 2 && width >= 2);

    // print the header
    printf("P5\n%i %i\n255\n", width, height);

    // iterate through p5 data and print to output char by char
    for(int q = 0; q < width * height; q ++){
        char c = p5lines[q];
        printf("%c", c);
    }
    free(p5lines);
}

/*
 * name:      getascii 
 * purpose:   read in p2 data and convert it to the cooresponding ascii value
 * arguments: char array of p2 data, size of the data, width passed by reference
 * returns:   char * to p5 data
 * effects:   builds new array, changes width by reference 
 */
char *getascii(char *p2lines, int size, int *w){
    int width = 0;
    char *p5lines = malloc(size);
    int tmpInt = 0;
    char tmpChar = 0;

    // iterate through every char in the p2 data
    for(int i = 0; i < size; i ++){
        // reset place holders to 0;
        int hundredsPlace = 0;
        int tensPlace = 0;
        int onesPlace = 0;
        // if an int is found, check the next 2 ints and adjust place values 
        // accordingly
        if(p2lines[i] > 47 && p2lines[i] < 58){
            if((i+1) < size && p2lines[i+1] > 47 && p2lines[i+1] < 58){
                if((i+2) < size && p2lines[i+2] > 47 && p2lines[i+2] < 58){
                    hundredsPlace = p2lines[i] - 48;
                    tensPlace = p2lines[i+1] - 48;
                    onesPlace = p2lines[i+2] - 48;
                    i += 2;
                }
                else{
                    tensPlace = p2lines[i] - 48;
                    onesPlace = p2lines[i+1] - 48;
                    i ++;
                }
            }
            else{
                onesPlace = p2lines[i] - 48;
            }
            // for each int build the int from place holders, set the char to 
            // that ascii value, add that char to the output and increment width
            tmpInt = 100*hundredsPlace + 10*tensPlace + onesPlace;
            tmpChar = tmpInt;
            p5lines[width] = tmpChar;
            width ++;
        }
    }
    // set width reference variable to what was built then return p5 data
    *w = width;
    return p5lines;
}

/*
 * name:      getCorrectSeq 
 * purpose:   finds the repeated injected sequence in a file
 * arguments: file to check, sequence to add correct lines to
 * returns:   char pointer to the correct injected sequence
 * effects:   adds to sequence, creates and frees a table, looks through file
 */
char *getCorrectSeq(FILE *file, Seq_T lines, size_t *length){
    // build a table that has injected sequences as keys and full lines as 
    // values
    Table_T table = Table_new(0, NULL, NULL);
    struct Line *currLine;
    struct Line *currSeq;
    char *correctSeq;

    // read through file building structs from data in file
    while(!feof(file)){
        currLine = malloc(sizeof(struct Line));
        assert(currLine != 0);

        currLine->size = readaline(file, &currLine->line);
        currSeq = getSeq(currLine->line, currLine->size);
        // if a value at the current key already exists, that is the repeated 
        // sequence
        if(Table_get(table, Atom_new(currSeq->line, currSeq->size)) != NULL){
            correctSeq = currSeq->line;
            // add the line that is already in the table at the correct sequence
            Seq_addhi(lines, (struct  Line *) 
            Table_get(table, Atom_new(correctSeq, currSeq->size)));
            // add the current correct line to the Seq
            Seq_addhi(lines, currLine);
            *length = currSeq->size;
            return correctSeq;
        }
        else {
            // else add the key value pair to the table
            Table_put(table, Atom_new(currSeq->line, currSeq->size), currLine);
        }
    }

    Table_map(table, applyFree, NULL);
    Table_free(&table);
    return "\n";
}

void applyFree(const void *key, void **value, void *cl){
    (void) key;
    (void) cl;
    free(((struct Line *) *value)->line);
    free((struct Line *) *value);
}


//takes a corrupted line and returns the non numeric values

/*
 * name:      getSeq 
 * purpose:   Given a line of values, gives the non numeric values
 * arguments: a line to read through
 * returns:   char pointer to the injected sequence 
 * effects:   none
 */
struct Line *getSeq(char *line, size_t size){
    // return line;
    int charCount = 0;
    size_t j = 0;
    for(size_t i = 0; i < size; i ++) {
        if(line[i] < 48 || line[i] > 57){
            charCount ++;
        }
    }
    char *seqChars = malloc(charCount * sizeof(char));
    assert(seqChars != NULL);

    for(size_t i = 0; i < size; i ++) {
        if(line[i] < 48 || line[i] > 57){
            seqChars[j] = line[i];
            j ++;
        }
    }
    struct Line *seq = malloc(sizeof(struct Line));
    assert(seq != NULL);

    seq->line = seqChars;
    seq->size = j;
    return seq;
}

/*
 * name:      removeChars 
 * purpose:   Given a line, turns every non digit to whitespace
 * arguments: a string of the line and the size of the line
 * returns:   string of the altered line
 * effects:   creates a new string on the heap
 */
char *removeChars(char *line, size_t size){
    char *output = malloc(size);
    assert(output != NULL);

    for(size_t i = 0; i < size; i ++){
        if(line[i] > 47 && line[i] < 58){
            output[i] = line[i];
        }
        else {
            output[i] = 32;
        }
    }
    return output;
}