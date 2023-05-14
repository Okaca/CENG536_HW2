//
// Created by onur on 14.05.2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// global variables
int **matrix1, **matrix2, **outputMatrix; //mxn matrix
int row,col,d;

void readInputFile();
void sumRows(int start_row, int end_row);
void *sumMatrix(void *arg);

int main(){
    clock_t start, end;
    double cpu_time_used;

    readInputFile();

    for ( int epoch = 1; epoch <= row; epoch += row-1){
        // start time
        start = clock();
        d = epoch;
        pthread_t threads[epoch];
        int thread_id[epoch];

        for (int i = 0; i < epoch; i++) {
            thread_id[i] = i;
            pthread_create(&threads[i], NULL, sumMatrix, &thread_id[i]);
        }

        for (int i = 0; i < epoch; i++) {
            pthread_join(threads[i], NULL);
        }

        printf("Output Matrix with %d thread:\n", epoch);
        for(int i=0; i<row; i++) {
            for(int j=0; j<col; j++) {
                printf("%d ", outputMatrix[i][j]);
            }
            printf("\n");
        }

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
    if((file = fopen("input2.txt","r"))==NULL){
        printf("You do NOT have an INPUT file name is input2.txt\n");
        printf("Please create 1 input file with the format:\t input2.txt");
        exit(1);
    }
    int m, n;
    fscanf(file, "%d %d", &m, &n); // get first integer input

    if(matrix1 == NULL)
        matrix1 = (int **)malloc(m * sizeof(int *)); // allocate mxm matrix1

    row = m;
    col = n;

    for(int i=0; i<m; i++) {
        if(matrix1[i] == NULL)
            matrix1[i] = (int *)malloc(n * sizeof(int));
        for(int j=0; j<n; j++) {
            fscanf(file, "%d", &matrix1[i][j]); // fill in the matrix1
        }
    }

    fscanf(file, "%d %d", &m, &n); // get first integer input
    if(matrix2 == NULL)
        matrix2 = (int **)malloc(m * sizeof(int *)); // allocate mxm matrix2

    for(int i=0; i<m; i++) {
        if(matrix2[i] == NULL)
            matrix2[i] = (int *)malloc(n * sizeof(int));
        for(int j=0; j<n; j++) {
            fscanf(file, "%d", &matrix2[i][j]); // fill in the matrix1
        }
    }
    if(outputMatrix == NULL)
        outputMatrix = (int **)malloc(m * sizeof(int *)); // allocate mxm outputMatrix

    for(int i=0; i<m; i++) {
        if(outputMatrix[i] == NULL)
            outputMatrix[i] = (int *)malloc(n * sizeof(int));
    }
    fclose(file);
}

void sumRows(int start_row, int end_row){

    // sum rows
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < col; j++) {
            outputMatrix[i][j] = matrix1[i][j] + matrix2[i][j];
        }
    }

}

void *sumMatrix(void *arg){
    int tid = *(int *)arg;
    int start_row = tid * (row/d);
    int end_row = (tid + 1) * (row/d);
    if (tid == d-1) end_row = row;

    sumRows(start_row, end_row);
}