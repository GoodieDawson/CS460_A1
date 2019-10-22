#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 

#include <string.h>

#include "data_types.h"
#include "storage_engine.h"

#include "btree.h"
#include <time.h>

#define CLOCKS_PER_SEC ((clock_t)(1000))

int main(){

    clock_t start, end;
    double cpu_time_used;

    bpTree root = bptCreate();

    for (size_t i = 0; i < 7000000; i++)
    {
         bptInsert(root, i);
    }

    start = clock();
    for (size_t i = 0; i < 7000000; i++)
    {
        int result = bptSearch(root, i);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time Elapsed is %.10f", cpu_time_used);

    return 0;
}