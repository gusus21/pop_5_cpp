#include <iostream>
#include <random>
#include <vector>
#include "omp.h"

using namespace std;

struct sum_idx
{
    int index;
    long long sum;
};


const int arrLength = 10000;
const int matrixLength = 10000;

vector<vector<int>> matrix(matrixLength);

void matrix_init();
long long part_sum(int, int, int);
sum_idx part_min(int, int, int);


int main(){
    matrix_init();
    int threads_amount = 1;
    double t1 = omp_get_wtime();
    cout << "Threads: " << threads_amount << endl;
    omp_set_nested(1);
#pragma omp parallel sections
    {
#pragma omp section
        {
            long long min = INT64_MAX;
            int idx = -1;
            for(int i = 0; i < threads_amount; i++){
                sum_idx part = part_min(i*arrLength/threads_amount+1, (i+1)*arrLength/threads_amount, threads_amount);
                
                if(min > part.sum){
                    min = part.sum;
                    idx = part.index;
                }
            }
            printf("minimal sum: %lld in index: %d\n", min, idx);
        }
#pragma omp section
        {

            long long sum = 0;
            for(int i = 0; i < threads_amount; i++){
            sum += part_sum(i*arrLength/threads_amount+1, (i+1)*arrLength/threads_amount, threads_amount);
            }
            printf("sum: %lld\n", sum);
        }
    }

    double t2 = omp_get_wtime();

    printf("time: %f\n", t2-t1);
    return 0;
}

void  matrix_init(){
    for(int i = 0; i < matrixLength; i++){
        for(int j = 0; j < arrLength; j++)
            matrix[i].push_back(rand()%10);
    }
}

long long part_sum(int start_idx, int end_idx, int thread_amount){
    long long sum = 0;
    double t1 = omp_get_wtime();
#pragma omp parallel for reduction(+:sum) num_threads(thread_amount)
    for(int i = start_idx; i < end_idx; i++){
        for(int j = 0; j < arrLength; j++)
            sum += matrix[i][j];
    }

    double t2 = omp_get_wtime();
    //printf("sum: %lld, time: %f\n", sum, t2-t1);
    return sum;
}

sum_idx part_min(int start_idx, int end_idx, int thread_amount){
    sum_idx result;
    long long min_sum = INT64_MAX;
    int row_min_sum = -1;
    double t1 = omp_get_wtime();
#pragma omp parallel for num_threads(thread_amount)
    for(int i = start_idx; i < end_idx; i++){
        long long sum = 0;

        for(int j = 0; j < arrLength; j++)
            sum+=matrix[i][j];
        
        if(min_sum > sum)
#pragma omp critical
            if(min_sum > sum){
                min_sum = sum;
                row_min_sum = i;
            }
    }
    double t2 = omp_get_wtime();
    //printf("min row: %d, time: %f\n", row_min_sum, t2-t1);
    result.sum = min_sum;
    result.index = row_min_sum;
    return result;
}