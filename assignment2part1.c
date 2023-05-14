//
// Created by onur on 13.05.2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// global variables
int **matrix; //mxm matrix
int row,col,s,d;
pthread_mutex_t mutex;
pthread_barrier_t barrier;

void readInputFile();
void shiftRows(int start_row, int end_row);
void shiftCols(int start_col, int end_col);
void *shiftMatrix(void *arg);

int main(){
    readInputFile();

    printf("\nOriginal Matrix\n");
    for(int i=0; i<row; i++) {
        for(int j=0; j<col; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    printf("Enter d and s: ");
    scanf("%d %d", &d, &s);

    pthread_t threads[d];
    int thread_id[d];

    pthread_mutex_init(&mutex, NULL);
    pthread_barrier_init(&barrier, NULL, d);

    for (int i = 0; i < d; i++) {
        thread_id[i] = i;
        pthread_create(&threads[i], NULL, shiftMatrix, &thread_id[i]);
    }

    for (int i = 0; i < d; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier);

    return 0;

    shiftRows(0,row);
    shiftCols(0,col);
    return  0;
}

void readInputFile(){
    FILE *file;
    if((file = fopen("input1.txt","r"))==NULL){
        printf("You do NOT have an INPUT file name is input1.txt\n");
        printf("Please create 1 input file with the format:\t input1.txt");
        printf("The file begins with m and then a mxm matrix.\n");
        exit(1);
    }
    int m;
    fscanf(file, "%d", &m); // get first integer input
    matrix = (int **)malloc(m * sizeof(int *)); // allocate mxm matrix

    row = col = m;

    for(int i=0; i<m; i++) {
        matrix[i] = (int *)malloc(m * sizeof(int));
        for(int j=0; j<m; j++) {
            fscanf(file, "%d", &matrix[i][j]); // fill in the matrix
        }
    }
    fclose(file);
}

void shiftRows(int start_row, int end_row){
    // wait for thread to shift columns
    pthread_mutex_lock(&mutex);
    // shift rows from left to right
    for (int i = start_row; i < end_row; i++) {
        int temp = matrix[i][col-1];
        for (int j = col-1; j > 0; j--) {
            matrix[i][j] = matrix[i][j - 1];
        }
        matrix[i][0] = temp;
    }

    printf("Shift Rows:\n");
    for(int i=0; i<row; i++) {
        for(int j=0; j<col; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    pthread_mutex_unlock(&mutex);
    pthread_barrier_wait(&barrier);
}

void shiftCols(int start_col, int end_col){
    // wait for thread to shift rows
    pthread_mutex_lock(&mutex);
    // shift rows from bottom to top
    for (int i = start_col; i < end_col; i++) {
        int temp = matrix[0][i];
        for (int j = 0; j < row-1; j++) {
            matrix[j][i] = matrix[j + 1][i];
        }
        matrix[col - 1][i] = temp;
    }

    printf("Shift Columns:\n");
    for(int i=0; i<row; i++) {
        for(int j=0; j<col; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    pthread_mutex_unlock(&mutex);
    pthread_barrier_wait(&barrier);
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
        pthread_barrier_wait(&barrier);
        shiftCols(start_col, end_col);
        pthread_barrier_wait(&barrier);
    }
}