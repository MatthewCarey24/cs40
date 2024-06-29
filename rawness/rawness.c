#include <stdio.h>

int main(int argc, char *argv[]){
    printf("Hello world \n");

    (void) argc;
    
    FILE *file;
    file = fopen(argv[1], "r");

    if (file == NULL){
        printf("unable to open file\n");
        return 1;
    }
    else {
        printf("file open\n");
        char magic[3];
        int width, height, maxVal, numPix;
        fscanf(file, "%2s %d %d %d", magic, &width, &height, &maxVal);
        printf("Magic number: %2s\n", magic);
        printf("Width: %d\n", width);
        printf("Height: %d\n", height);
        printf("Max Value: %d\n", maxVal);
        numPix = width * height;
        printf("number of pixels: %d\n", numPix);
    }

    fclose(file);
    return 0;
}