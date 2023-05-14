//
// Created by onur on 14.05.2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// global variables
int **matrix1, **matrix2, **outputMatrix; //mxn & nxk matrices
int m, n, k, d;
int elementSum;
pthread_mutex_t mutex;

void readInputFile();
void multMatrixByRow(int start_row, int end_row);
void *multMatrix(void *arg);

int main(){
    clock_t start, end;
    double cpu_time_used;

    readInputFile();

    for ( int epoch = 1; epoch <= m; epoch += m-1){
        // start time
        start = clock();
        d = epoch;
        pthread_t threads[epoch];
        int thread_id[epoch];

        for (int i = 0; i < epoch; i++) {
            thread_id[i] = i;
            pthread_create(&threads[i], NULL, multMatrix, &thread_id[i]);
        }

        for (int i = 0; i < epoch; i++) {
            pthread_join(threads[i], NULL);
        }

        printf("Output Matrix with %d thread:\n", epoch);
        for(int i=0; i<m; i++) {
            for(int j=0; j<k; j++) {
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
    if((file = fopen("input3.txt","r"))==NULL){
        printf("You do NOT have an INPUT file name is input3.txt\n");
        printf("Please create 1 input file with the format:\t input3.txt");
        printf("The file begins with m n and then a mxn matrix, and n k and then a nxk matrix.\n");
        exit(1);
    }

    fscanf(file, "%d %d", &m, &n); // get first integer input

    if(matrix1 == NULL)
        matrix1 = (int **)malloc(m * sizeof(int *)); // allocate mxm matrix1

    for(int i=0; i<m; i++) {
        if(matrix1[i] == NULL)
            matrix1[i] = (int *)malloc(n * sizeof(int));
        for(int j=0; j<n; j++) {
            fscanf(file, "%d", &matrix1[i][j]); // fill in the matrix1
        }
    }

    fscanf(file, "%d %d", &n, &k); // get first integer input
    if(matrix2 == NULL)
        matrix2 = (int **)malloc(n * sizeof(int *)); // allocate mxm matrix2

    for(int i=0; i<n; i++) {
        if(matrix2[i] == NULL)
            matrix2[i] = (int *)malloc(k * sizeof(int));
        for(int j=0; j<k; j++) {
            fscanf(file, "%d", &matrix2[i][j]); // fill in the matrix2
        }
    }

    if(outputMatrix == NULL)
        outputMatrix = (int **)malloc(m * sizeof(int *)); // allocate mxm outputMatrix
    for(int i=0; i<m; i++) {
        if(outputMatrix[i] == NULL)
            outputMatrix[i] = (int *)malloc(k * sizeof(int));
        for(int j = 0; j < k; j++){
            outputMatrix[i][j] = 0;
        }
    }
    fclose(file);
}

void multMatrixByRow(int start_row, int end_row){

    // matrix multiplication
    for (int i = start_row; i < end_row; i++) {
        for (int l = 0; l < k; l++){
            // lock thread until row multiplication is done
            pthread_mutex_lock(&mutex);
            elementSum = 0;
            for (int j = 0; j < n; j++) {
                elementSum += matrix1[i][j] * matrix2[j][l];
            }
            outputMatrix[i][l] = elementSum;
            pthread_mutex_unlock(&mutex);
        }
    }

}

void *multMatrix(void *arg){
    int tid = *(int *)arg;
    int start_row = tid * (m/d);
    int end_row = (tid + 1) * (m/d);
    if (tid == d-1) end_row = m;

    multMatrixByRow(start_row, end_row);
}