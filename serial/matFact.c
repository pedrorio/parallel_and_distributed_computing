#include <stdlib.h>
#include <stdio.h>

//#define RAND01 ((double)random() / (double)RAND_MAX)
//void random_fill_LR(int nU, int nI, int nF) {
//    srandom(0);
//    for(int i = 0; i < nU; i++) for(int j = 0; j < nF; j++)
//            L[i][j] = RAND01 / (double) nF; for(int i = 0; i < nF; i++)
//        for(int j = 0; j < nI; j++)
//            R[i][j] = RAND01 / (double) nF;
//}

// A = i Items, u Users, Items and Users
// L = u Users, f Features, Users and Features
// R = f Features, i Items, Features and Items
// B = i Items, u Users, Items and Users
// B = L * R

// Initialize L and R with random_fill_LR
// Make a copy of L and R

int main(int argc, char *argv[]) {

    printf("Program name is: %s\n", argv[0]);

    if (argc < 2) {
        printf("No inFile was provided through command line.\n");
        return 1;
    }
    printf("inFile: %s\n", argv[1]);

    FILE *inFile;
    inFile = fopen(argv[1], "r");

    if (inFile == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    printf("The contents of %s file are:\n", inFile);

    char ch;
    while((ch = fgetc(inFile)) != EOF)
        printf("%c", ch);

    fclose(inFile);
    return 0;
}