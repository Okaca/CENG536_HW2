//
// Created by onur on 13.05.2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// global variables
int **matrix1, **matrix2, **outputMatrix; //global matrices
int **constMatrix1, ***constMatrix2, **constOutMatrix;
int row,col,s,d;
pthread_barrier_t barrier;
int m, n, k;
int elementSum;
pthread_mutex_t mutex;

int **readInputFileP1(FILE *file);
void shiftRows(int start_row, int end_row);
void shiftCols(int start_col, int end_col);
void *shiftMatrix(void *arg);

int ***readInputFileP2(FILE *file);
void sumRows(int start_row, int end_row);
void *sumMatrix(void *arg);

int ***readInputFileP3(FILE *file);
void multMatrixByRow(int start_row, int end_row);
void *multMatrix(void *arg);

int main(){
    clock_t start, end;
    double cpu_time_used;

    FILE *file;
    if((file = fopen("input.txt","r"))==NULL){
        printf("You do NOT have an INPUT file name is input.txt\n");
        printf("Please create 1 input file with the format:\t input.txt");
        exit(1);
    }

    // ================================== PART 1 ===========================//
    constMatrix1 = readInputFileP1(file);
    printf("\n====== PART 1 ======== \n");
    printf("\nOriginal Matrix \n");
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
        matrix1 = constMatrix1;
    }

    //=================================== PART 2 =================================//
    printf("\n====== PART 2 ======== \n");
    readInputFileP2(file);

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
    }

    //=================================== PART 3 =================================//
    constMatrix2 = readInputFileP3(file);
    printf("\n====== PART 3 ======== \n");
    printf("m = %d, n = %d \n",m, n);
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

        // reset matrix1 & matrix2
        matrix1 = constMatrix2[0];
        matrix2 = constMatrix2[1];
    }

    free(matrix1);
    free(matrix2);
    free(outputMatrix);
    return 0;
}

int **readInputFileP1(FILE *file){

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
    return matrix1;
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

int ***readInputFileP2(FILE *file){

    fscanf(file, "%d %d", &m, &n); // get first integer input

    if(matrix1 == NULL)
        matrix1 = (int **)malloc(m * sizeof(int *)); // allocate mxn matrix1
    else
        matrix1 = (int **)realloc(matrix1,m * sizeof(int *)); // allocate mxn matrix1

    row = m;
    col = n;

    for(int i=0; i<m; i++) {
        if(matrix1[i] == NULL)
            matrix1[i] = (int *)malloc(n * sizeof(int));
        else
            matrix1[i] = (int *)realloc(matrix1[i] ,n * sizeof(int));
        for(int j=0; j<n; j++) {
            fscanf(file, "%d", &matrix1[i][j]); // fill in the matrix1
        }
    }


    fscanf(file, "%d %d", &m, &n); // get first integer input
    if(matrix2 == NULL)
        matrix2 = (int **)malloc(m * sizeof(int *)); // allocate mxm matrix2
    else
        matrix2 = (int **)realloc(matrix2, m * sizeof(int *)); // reallocate mxm matrix2

    for(int i=0; i<m; i++) {
        if(matrix2[i] == NULL)
            matrix2[i] = (int *)malloc(n * sizeof(int));
        else
            matrix2[i] = (int *)realloc(matrix2[i], n * sizeof(int));
        for(int j=0; j<n; j++) {
            fscanf(file, "%d", &matrix2[i][j]); // fill in the matrix1
        }
    }

    if(outputMatrix == NULL)
        outputMatrix = (int **)malloc(m * sizeof(int *)); // allocate mxm outputMatrix
    else
        outputMatrix = (int **)realloc(outputMatrix, m * sizeof(int *));

    for(int i=0; i<m; i++) {
        if(outputMatrix[i] == NULL)
            outputMatrix[i] = (int *)malloc(n * sizeof(int));
        else
            outputMatrix[i] = (int *)realloc(outputMatrix[i],n * sizeof(int));
    }
    fclose(file);
    int ***mats = malloc(sizeof(int**) * 2);
    mats[0] = matrix1;
    mats[1] = matrix2;
    return mats;
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

int ***readInputFileP3(FILE *file){

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
        matrix2 = (int **)realloc(matrix2,n * sizeof(int *)); // allocate mxm matrix2

    for(int i=0; i<n; i++) {
        if(matrix2[i] == NULL)
            matrix2[i] = (int *)realloc(matrix2,k * sizeof(int));
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
    int ***mats = malloc(sizeof(int**) * 2);
    mats[0] = matrix1;
    mats[1] = matrix2;
    return mats;
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