#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 

#include <string.h>

#include "data_types.h"
#include "storage_engine.h"

#include "btree.h"
#include <time.h>

#define CLOCKS_PER_SEC ((clock_t)(1000))

//main function
int main(){

    //Declare variables used to measure execution time
    clock_t start, end;
    double cpu_time_used;

    //Create tree
    bpTree root = bptCreate();

    //Test insert function
    for (size_t i = 0; i < 7000000; i++)
    {
         bptInsert(root, i);
    }

    start = clock();
    //Test find function
    for (size_t i = 0; i < 7000000; i++)
    {
        int result = bptSearch(root, i);
    }
    end = clock();

    //Calculate execution time
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    //Print result
    printf("Time Elapsed is %.10f", cpu_time_used);

    return 0;
}