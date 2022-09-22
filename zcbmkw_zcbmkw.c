#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "input_error.h"

#define GROUP_SIZE 5

void insertionSort(int *A, int l, int h){
    // assume that l < h, l > 0, h < A.len
    int i, j, key;
    for(i = l+1; i < h; i++){
        key = A[i];
        j = i - 1;
        while(j >= l && A[j] > key){
            A[j+1] = A[j];
            j--;
        }
        A[j+1] = key;
    }
}

void merge(int *A, int l, int m, int r){
    int i, j, k;
    int nl = m - l + 1;
    int nr = r - m;

    int L[nl], R[nr];

    for(i = 0; i < nl; i++){
        L[i] = A[l+i];
    }
    for(j = 0; j < nr; j++){
        R[j] = A[m + 1 + j];
    }

    i = 0, j = 0, k = l;

    while(i < nl && j < nr){
        if(L[i] <= R[j]){
            A[k] = L[i];
            i++;
        } else {
            A[k] = R[j];
            j++;
        }
        k++;
    }

    while(i < nl){
        A[k] = L[i];
        i++;
        k++;
    }

    while(j < nr){
        A[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(int *A, int l, int r){
    if(l < r) {
        int m = l + (r-l) / 2;
        mergeSort(A, l, m);
        mergeSort(A, m+1, r);

        merge(A, l, m, r);
    }
}

void partitionThreeWays(const int* A, int n, int* L, int *l, int* M, int *m, int* R, int *r, int p){
    int i;
    for(i=0; i<n; i++){
        if(A[i] < p)
            (*l)++;
        if(A[i] > p)
            (*r)++;
        if(A[i] == p)
            (*m)++;
    }


    L = malloc((*l) * sizeof(int));
    M = malloc((*m) * sizeof(int));
    R = malloc((*r) * sizeof(int));

    (*l) = 0, (*m) = 0, (*r) = 0;

    for(i=0; i<n; i++){
        if(A[i] < p){
            L[(*l)] = A[i];
            (*l)++;
        }
        if(A[i] > p){
            M[(*m)] = A[i];
            (*m)++;
        }
        if(A[i] == p) {
            R[(*r)] = A[i];
            (*r)++;
        }
    }
}

int medianOfMedianSelect(int *A, int n, int k){
    int i, n_groups;

    n_groups = floor((double)n / GROUP_SIZE);

    if(n < 50){
        mergeSort(A, 0, n-1);
        return A[k];
    }

    int *B = malloc(n_groups * sizeof(int));

    for(i = 0; i < n_groups; i++){
        insertionSort(A, i*GROUP_SIZE, (i+1) * GROUP_SIZE);
        B[i] = A[i * GROUP_SIZE + 2];
    }

    int p = medianOfMedianSelect(B, n_groups, floor((double)n / 10));

    free(B);

    int *L, *M, *R, l, m, r;

    partitionThreeWays(A, n, L, &l, M, &m, R, &r, p);

    if( k <= l){
        return medianOfMedianSelect(L, l, k);
    } else {
        if(k > l + m){
            return medianOfMedianSelect(R, r, k - l - m);
        } else {
            return p;
        }
    }
}

// read a file to an array
int readFile(char *filename, int **A) {

    // declare variables
    int i, length, current_position, n;
    int c;
    char *line = NULL;
    size_t len;
    ssize_t read;
    FILE *fp;

    // initialize variables
    current_position = 0;
    line = NULL;
    len = 0;

    // open the file
    fp = fopen(filename, "r");

    // check that the file was successfully opened, if not free 'A' and exit
    if (fp == NULL){
        exit(INPUT_FILE_FAILED_TO_OPEN);
    }

    // initialize length of 'A' to 0
    n = 0;

    // check that file is not empty by checking the first character, if so free 'A' and exit
    c = fgetc(fp);
    if(c == EOF){
        exit(PARSING_ERROR_EMPTY_FILE);
    } else {
        ungetc(c, fp);
    }

    // iterate line-by-line through the file:
    //  increment the length of 'A' so long as the line isn't blank
    //  check that no line contains an invalid character
    while ((read = getline(&line, &len, fp)) != -1){
        // We need to differentiate between the final line and all other lines
        //  Why? Because the final line is 'x' instead of 'x\n'
        //  the final line will have a length equal to 'read'
        //  all other lines have a length of ('read' - 1)
        length = (feof(fp) ? read : read - 1);

        // check that each character in 'line' is a digit, if not free 'A' and exit
        for(i = 0; i < length; i++){
            if(!isdigit(line[i])){
                exit(PARSING_ERROR_INVALID_CHARACTER_ENCOUNTERED);
            }
        }

        // increase the size of the array if 'line' != "\n"
        if(length != 0){
            n++;
        }
    }

    //TODO: prolly should remove this
    // Check that the array isn't empty. If so, the file was empty, free 'A' and exit.
    if(n == 0){
        exit(PARSING_ERROR_EMPTY_FILE);
    }

    // allocate memory for the data of
    (*A) = malloc(n * sizeof(int));

    // rewind the file pointer to the beginning of the file
    rewind(fp);

    //  read through the file again, this time we will:
    //    initialize the length of the line as we did previously
    //    check that the line isn't blank
    //      if it is, do nothing
    //      otherwise, log the integer into A->data[current_position] and increment current_position by 1
    while ((read = getline(&line, &len, fp)) != -1) {
        length = (feof(fp) ? read : read - 1);
        if(length != 0){
            (*A)[current_position] = strtol(line, NULL, 10);
            current_position++;
        }
    }

    free(line);

    //  close the file and check that the file was closed properly
    if(fclose(fp) != 0){
        free((*A));
        exit(INPUT_FILE_FAILED_TO_CLOSE);
    }

    return n;
}

int main(int argc, char **argv) {
//    testMergeSort();
    char *filename;
    int *A, n, x;

    x = 6;

    if(argc != 2) {
        exit(INCORRECT_NUMBER_OF_COMMAND_LINE_ARGUMENTS);
    }

    filename = argv[1];

    n = readFile(filename, &A);

    printf("%d", medianOfMedianSelect(A, n, x));

    free(A);

    return 0;
}
