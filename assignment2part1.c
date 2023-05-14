//
// Created by onur on 13.05.2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// global variables
int **matrix1; //mxm matrix1
int row,col,s,d;
pthread_barrier_t barrier;

void readInputFile();
void shiftRows(int start_row, int end_row);
void shiftCols(int start_col, int end_col);
void *shiftMatrix(void *arg);

int main(){
    clock_t start, end;
    double cpu_time_used;

    readInputFile();
    printf("\nOriginal Matrix\n");
    for(int i=0; i<row; i++) {
        for(int j=0; j<col; j++) {
            printf("%d ", matrix1[i][j]);
        }
        printf("\n");
    }

    printf("Enter d and s: ");
    scanf("%d %d", &d, &s);
    for ( int epoch = 1; epoch <= d; epoch++){
        // start time
        start = clock();

        pthread_t threads[epoch];
        int thread_id[epoch];

        pthread_barrier_init(&barrier, NULL, epoch);

        for (int i = 0; i < epoch; i++) {
            thread_id[i] = i;
            pthread_create(&threads[i], NULL, shiftMatrix, &thread_id[i]);
        }

        for (int i = 0; i < epoch; i++) {
            pthread_join(threads[i], NULL);
        }

        printf("Output Matrix with %d threads and %d many times:\n", epoch, s);
        for(int i=0; i<row; i++) {
            for(int j=0; j<col; j++) {
                printf("%d ", matrix1[i][j]);
            }
            printf("\n");
        }
        pthread_barrier_destroy(&barrier);

        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("Execution time: %f ms\n", cpu_time_used*1000);

        // reset matrix1
        readInputFile();
    }
    return 0;
}

void readInputFile(){
    FILE *file;
    if((file = fopen("input1.txt","r"))==NULL){
        printf("You do NOT have an INPUT file name is input1.txt\n");
        printf("Please create 1 input file with the format:\t input1.txt");
        printf("The file begins with m and then a mxm matrix1.\n");
        exit(1);
    }
    int m;
    fscanf(file, "%d", &m); // get first integer input
    if(matrix1 == NULL)
        matrix1 = (int **)malloc(m * sizeof(int *)); // allocate mxm matrix1

    row = col = m;

    for(int i=0; i<m; i++) {
        if(matrix1[i] == NULL)
            matrix1[i] = (int *)malloc(m * sizeof(int));
        for(int j=0; j<m; j++) {
            fscanf(file, "%d", &matrix1[i][j]); // fill in the matrix1
        }
    }
    fclose(file);
}

void shiftRows(int start_row, int end_row){
    // shift rows from left to right
    for (int i = start_row; i < end_row; i++) {
        int temp = matrix1[i][col - 1];
        for (int j = col-1; j > 0; j--) {
            matrix1[i][j] = matrix1[i][j - 1];
        }
        matrix1[i][0] = temp;
    }
}

void shiftCols(int start_col, int end_col){
    // shift rows from bottom to top
    for (int i = start_col; i < end_col; i++) {
        int temp = matrix1[0][i];
        for (int j = 0; j < row-1; j++) {
            matrix1[j][i] = matrix1[j + 1][i];
        }
        matrix1[col - 1][i] = temp;
    }
}

void *shiftMatrix(void *arg){
    int tid = *(int *)arg;
    int start_row = tid * (row/d);
    int end_row = (tid + 1) * (row/d);
    if (tid == d-1) end_row = row;
    int start_col = tid * (col/d);
    int end_col = (tid + 1) * (col/d);
    if (tid == d-1) end_col = col;

    for (int i = 0; i < s; i++) {
        shiftRows(start_row, end_row);
        // wait for thread to shift rows
        pthread_barrier_wait(&barrier);
        shiftCols(start_col, end_col);
        // wait for thread to shift columns
        pthread_barrier_wait(&barrier);
    }
}