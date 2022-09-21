#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include "input_error.h"

#define GROUP_SIZE 5
#define max(x,y) (((x) >= (y)) ? (x) : (y))
#define min(x,y) (((x) <= (y)) ? (x) : (y))

typedef struct smart_array_struct{
    int *data;
    int len;
} *SmartArray;

void smartInsertionSort(SmartArray A, int l, int h){
    if(h > A->len || l >= h || l < 0){
        fprintf(stderr, "ERROR: INVALID ENTRIES TO FUNCTION smartInsertionSort");
        return;
    }

    int i, j;
    int key;

    for(i = l+1; i < h; i++){
        key = A->data[i];
        j = i - 1;
        while(j >= l && A->data[j] > key){
            A->data[j+1] = A->data[j];
            j = j - 1;
        }
        A->data[j+1] = key;
    }
}

SmartArray createSmartArray(){
    SmartArray newArray = malloc(sizeof(struct smart_array_struct));
    if(newArray){
        newArray->len = 0;
        newArray->data = NULL;
    }
    return newArray;
}

void printSmartArray(SmartArray A, char* arr_name){
    printf("Smart Array %s: [ ", arr_name);
    for(int i = 0; i < A->len - 1; i++){
        printf("%d, ", A->data[i]);
    }
    printf("%d]\n", A->data[A->len-1]);
}

bool mallocSmartArrayData(SmartArray A){
    A->data = malloc(A->len * sizeof(int));
    return A->data != NULL;
}

void freeSmartArray(SmartArray A){
    if(A->data == NULL){
        free(A->data);
        A->data = NULL;
    }
    free(A);
    A = NULL;
}

void smartMerge(SmartArray A, int l, int m, int r){
    int i, j, k;

    SmartArray L = createSmartArray();
    SmartArray R = createSmartArray();

    L->len = m - l + 1;
    R->len = r - m;

    mallocSmartArrayData(L);
    mallocSmartArrayData(R);


    for(i = 0; i < L->len; i++)
        L->data[i] = A->data[l + i];
    for(i = 0; i < R->len; i++)
        R->data[i] = A->data[m + 1 + i];

    i = 0;
    j = 0;
    k = l;

    while(i < L->len && j < R->len){
        A->data[k] = min(L->data[i], R->data[j]);
        if(L->data[i] <= R->data[j]){
            i++;
        } else {
            j++;
        }
        k++;
    }

    while(i < L->len){
        A->data[k] = L->data[i];
        i++;
        k++;
    }

    while(j < R->len){
        A->data[k] = R->data[j];
        j++;
        k++;
    }
}

void smartMergeSort(SmartArray A, int l, int r){
    if(l < r) {
        int m = l + (r-l) / 2;
        smartMergeSort(A, l, m);
        smartMergeSort(A, m+1, r);

        smartMerge(A, l, m, r);
    }
}

void partitionThreeWays(SmartArray A, SmartArray L, SmartArray M, SmartArray R, int p){
    int l, m, r;

    for(int i = 0; i < A->len; i++){
        if(A->data[i] < p){
            L->len++;
        }
        if(A->data[i] > p){
            R->len++;
        }
        if(A->data[i] == p){
            M->len++;
        }
    }

    mallocSmartArrayData(L);
    mallocSmartArrayData(M);
    mallocSmartArrayData(R);

    l = m = r = 0;

    for(int i = 0; i < A->len; i++){
        if(A->data[i] < p){
            L->data[l] = A->data[i];
            l++;
        }
        if(A->data[i] > p){
            R->data[r] = A->data[i];
            r++;
        }
        if(A->data[i] == p){
            M->data[m] = A->data[i];
            m++;
        }
    }
}

int MoM_Select(SmartArray A, int k){
    int i, n_groups = floor(A->len / GROUP_SIZE);

    if(A->len < 50){
        smartMergeSort(A, 0, A->len);
        return A->data[k];
    }

    SmartArray B = createSmartArray();
    B->len = n_groups;
    mallocSmartArrayData(B);

    for(i = 0; i < n_groups; i++) {
        smartInsertionSort(A, i * GROUP_SIZE, (i + 1) * GROUP_SIZE);
        B->data[i] = A->data[i * GROUP_SIZE + 2];
    }

    int p = MoM_Select(B, floor(A->len / 10));

    freeSmartArray(B);

    SmartArray L, M, R;
    L = createSmartArray();
    M = createSmartArray();
    R = createSmartArray();
    partitionThreeWays(A, L, M, R, p);

    if( k <= L->len){
        return MoM_Select(L, k);
    } else {
        if(k > L->len + M->len){
            return MoM_Select(R, k - L->len - M->len);
        } else {
            return p;
        }
    }
}


// read a file to an array
SmartArray readFile(char *filename) {

    // declare variables
    int i, length, current_position;
    int c;
    char *line;
    size_t len;
    ssize_t read;
    FILE *fp;
    SmartArray A;

    // initialize variables
    current_position = 0;
    line = NULL;
    len = 0;
    A = createSmartArray();

    // open the file
    fp = fopen(filename, "r");

    // check that the file was successfully opened, if not free 'A' and exit
    if (fp == NULL){
        freeSmartArray(A);
        exit(INPUT_FILE_FAILED_TO_OPEN);
    }

    // initialize length of 'A' to 0
    A->len = 0;

    // check that file is not empty by checking the first character, if so free 'A' and exit
    c = fgetc(fp);
    if(c == EOF){
        freeSmartArray(A);
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
                freeSmartArray(A);
                printf("invalid character encountered\n");
                exit(PARSING_ERROR_INVALID_CHARACTER_ENCOUNTERED);
            }
        }

        // increase the size of the array if 'line' != "\n"
        if(length != 0){
            A->len++;
        }
    }

    //TODO: prolly should remove this
    // Check that the array isn't empty. If so, the file was empty, free 'A' and exit.
    if(A->len == 0){
        freeSmartArray(A);
        exit(PARSING_ERROR_EMPTY_FILE);
    }

    // allocate memory for the data of
    mallocSmartArrayData(A);

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
            A->data[current_position] = strtol(line, NULL, 10);
            current_position++;
        }
    }

    //  close the file and check that the file was closed properly
    if(fclose(fp) != 0){
        freeSmartArray(A);
        exit(INPUT_FILE_FAILED_TO_CLOSE);
    }

    return A;
}

int main(int argc, char **argv) {
    char *filename;
    SmartArray A;
    int x = 10;

    if(argc != 2) {
        fprintf(stderr, "ERROR: INCORRECT NUMBER OF COMMAND LINE ARGUMENTS\n");
        exit(INCORRECT_NUMBER_OF_COMMAND_LINE_ARGUMENTS);
    }

    filename = argv[1];

    A = readFile(filename);

    printf("%d", MoM_Select(A, x));

    return 0;
}
