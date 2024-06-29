#include <stdio.h>
#include <string.h>
#include <stdlib.h>

size_t readaline(FILE *inputfd, char **datapp){
    char *line = malloc(1000);

    if(inputfd == NULL || datapp == NULL){
        fprintf(stderr, "arguments can not be null\n");
        exit(4);
    }

    char c = 32;
    size_t i = 0;
    while(c != '\n' && !feof(inputfd)){
        c = fgetc(inputfd);
        if(c != '\n'){
            line[i] = c;
            i ++;
            if(i == 1000){
                fprintf(stderr, "readaline: input too long\n");
                exit(4);
            }
        }
    }
    *datapp = line;
    return i;
}

