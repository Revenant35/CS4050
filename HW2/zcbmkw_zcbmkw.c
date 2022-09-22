#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include "input_error.h"

#define GROUP_SIZE 5

// simple insertion sort algorithm
//  assume that (l < h), (l > 0), (h < A.len)
void insertionSort(unsigned int **A, unsigned int l, unsigned int h){
    unsigned int i, j, key;
    for(i = l+1; i < h; i++){
        key = (*A)[i];
        j = i - 1;
        while(j >= l && (*A)[j] > key || (j-1) != UINT_MAX){
            (*A)[j+1] = (*A)[j];
            if(j == 0)
                break;
            else
                j--;
        }
        (*A)[j+1] = key;
    }
}

// merge and sort two sub-arrays from mergeSort
void merge(unsigned int **A, unsigned int l, unsigned int m, unsigned int r){
    unsigned int i, j, k;
    unsigned int nl = m - l + 1;
    unsigned int nr = r - m;

    unsigned int L[nl], R[nr];

    for(i = 0; i < nl; i++){
        L[i] = (*A)[l+i];
    }
    for(j = 0; j < nr; j++){
        R[j] = (*A)[m + 1 + j];
    }

    i = 0, j = 0, k = l;

    while(i < nl && j < nr){
        if(L[i] <= R[j]){
            (*A)[k] = L[i];
            i++;
        } else {
            (*A)[k] = R[j];
            j++;
        }
        k++;
    }

    while(i < nl){
        (*A)[k] = L[i];
        i++;
        k++;
    }

    while(j < nr){
        (*A)[k] = R[j];
        j++;
        k++;
    }
}

// a simple merge sort algorithm
//  assume that (l < h), (l > 0), (h < A.len)
void mergeSort(unsigned int **A, unsigned int l, unsigned int r){
    if(l < r) {
        unsigned int m = l + (r-l) / 2;
        mergeSort(A, l, m);
        mergeSort(A, m+1, r);

        merge(A, l, m, r);
    }
}

// Partitions an Array into 3 separate arrays
// NOTE: The arrays L, M, R must be passed into the function though they should not have memory allocated yet
// NOTE: l, m, r should be passed into this array as &l, &m, and &r
void partitionThreeWays(const unsigned int* A, unsigned int n, unsigned int** L, unsigned int *l, unsigned int** M, unsigned int *m, unsigned int** R, unsigned int *r, unsigned int p){
    unsigned int i;

    (*l) = 0, (*m) = 0, (*r) = 0;

    for(i=0; i<n; i++){
        if(A[i] < p)
            (*l)++;
        if(A[i] > p)
            (*r)++;
        if(A[i] == p)
            (*m)++;
    }


    (*L) = malloc((*l) * sizeof(unsigned int));
    (*M) = malloc((*m) * sizeof(unsigned int));
    (*R) = malloc((*r) * sizeof(unsigned int));

    (*l) = 0, (*m) = 0, (*r) = 0;

    for(i=0; i<n; i++){
        if(A[i] < p){
            (*L)[(*l)] = A[i];
            (*l)++;
        }
        if(A[i] == p){
            (*M)[(*m)] = A[i];
            (*m)++;
        }
        if(A[i] > p) {
            (*R)[(*r)] = A[i];
            (*r)++;
        }
    }
}

// selects the k-th smallest element of A using the Median of Medians algorithm
unsigned int medianOfMedianSelect(unsigned int **A, unsigned int n, unsigned int k){
    unsigned int i, n_groups;

    n_groups = n / GROUP_SIZE;

    if(n < 50){
        mergeSort(A, 0, n-1);
        if((*A) == NULL || (k-1) >= n || (k-1) < 0){
            printf("ERROR\n");
        }
        return (*A)[k-1];
    }

    unsigned int *B = malloc(n_groups * sizeof(unsigned int));

    for(i = 0; i < n_groups; i++){
        insertionSort(A, i*GROUP_SIZE, (i+1) * GROUP_SIZE);
        B[i] = (*A)[i * GROUP_SIZE + 2];
    }

    unsigned int p = medianOfMedianSelect(&B, n_groups, n / 10);

    free(B);

    unsigned int *L, *M, *R, l, m, r;

    partitionThreeWays(*A, n, &L, &l, &M, &m, &R, &r, p);

    if ( k <= l ) {
        p = medianOfMedianSelect(&L, l, k);
    } else {
        if ( k > l + m ) {
            p = medianOfMedianSelect(&R, r, k - l - m);
        }
    }

    free(L);
    free(M);
    free(R);
    return p;
}

// read a file into an array and return the query value
unsigned int readFiletoArray(char *filename, unsigned int **A, unsigned int *n) {

    // declare variables
    unsigned int i, length, current_position, x;
    int c;
    char *line = NULL;
    size_t len;
    ssize_t read;
    FILE *fp;

    // initialize variables
    current_position = 0;
    line = NULL;
    len = 0;
    (*n) = 0;

    // open the file
    fp = fopen(filename, "r");

    // check that the file was successfully opened, if not, exit
    if (fp == NULL){
        exit(INPUT_FILE_FAILED_TO_OPEN);
    }

    // check that file is not empty by checking the first character, if so, exit
    c = fgetc(fp);
    if(c == EOF){
        exit(PARSING_ERROR_EMPTY_FILE);
    } else {
        ungetc(c, fp);
    }

    read = getline(&line, &len, fp);

    length = (feof(fp) ? read : read - 1);

    // check that each character (aside from the final '\n') in 'line' is a digit, if not free 'A' and exit
    for(i = 0; i < length; i++){
        if(!isdigit(line[i])){
            exit(PARSING_ERROR_INVALID_CHARACTER_ENCOUNTERED);
        }
    }

    x = strtol(line, NULL, 10);


    // iterate line-by-line through the file:
    //  increment the length of 'A' so long as the line isn't blank
    //  check that no line contains an invalid character
    while ((read = getline(&line, &len, fp)) != -1){
        // We need to differentiate between the final line and all other lines
        //  Why? Because the final line is 'x' instead of 'x\n'
        length = (feof(fp) ? read : read - 1);

        // check that each character (aside from the final '\n') in 'line' is a digit, if not free 'A' and exit
        for(i = 0; i < length; i++){
            if(!isdigit(line[i])){
                exit(PARSING_ERROR_INVALID_CHARACTER_ENCOUNTERED);
            }
        }

        // increase the size of the array if 'line' != "\n"
        if(length != 0){
            (*n)++;
        }
    }

    // Check that the array isn't empty. If so, the file was only `\n`s, exit.
    if((*n) == 0){
        exit(PARSING_ERROR_EMPTY_FILE);
    }

    // allocate memory for A
    (*A) = malloc((*n) * sizeof(int));

    // rewind the file pointer to the beginning of the file
    rewind(fp);

    read = getline(&line, &len, fp);

    //  read through the file again, this time we will:
    //    initialize the length of the line as we did previously
    //    check that the line isn't blank
    //      if it is, do nothing
    //      otherwise, log the integer into A[current_position] and increment current_position by 1
    while ((read = getline(&line, &len, fp)) != -1) {
        length = (feof(fp) ? read : read - 1);
        if(length != 0){
            (*A)[current_position] = strtol(line, NULL, 10);
            current_position++;
        }
    }

    // free memory allocated by getline()
    free(line);

    // close the file and check that the file was closed properly
    if(fclose(fp) != 0){
        free((*A));
        exit(INPUT_FILE_FAILED_TO_CLOSE);
    }

    // return the size of A
    return x;
}

int main(int argc, char **argv) {
    char *filename;
    unsigned int *A, n, x;

    if(argc != 2) {
        exit(INCORRECT_NUMBER_OF_COMMAND_LINE_ARGUMENTS);
    }

    filename = argv[1];

    x = readFiletoArray(filename, &A, &n);

    printf("%d\n", ( n < x ) ? -1 : medianOfMedianSelect(&A, n, x));

    mergeSort(&A, 0, n-1);

    free(A);

    return 0;
}
