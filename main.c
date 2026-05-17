#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "frontend.h"

int main(int argc, char *argv[]) {
    if (argc != 3 && strcmp(argv[1],"ETI-660") != 0 && strcmp(argv[1],"NORMAL") != 0) {
        printf("Usage: ./C8EM mode romfile\n");
        printf("\t-mode: ETI-660 or NORMAL.\n");
        printf("\tromfile: the file path for the program to run.\n");
        exit(WRONG_NUMBER_OF_ARGUMENTS);
    }
    printf("+File name to be loaded: %s\n", argv[2]);
    load_rom(argv[2]);

}