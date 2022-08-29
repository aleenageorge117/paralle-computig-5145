#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include<chrono>
#include <omp.h>

#ifdef __cplusplus
extern "C" {
#endif
  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif


int min(int x, int y) {
    return (x < y) ? x : y;
}


void mergeArr(int arr[], int tmp[], int startPosition, int middle, int endPosition, int n){

 int k = startPosition, i = startPosition, j =  middle + 1;
 
  
    while (i <=  middle && j <= endPosition)
    {
        if (arr[i] < arr[j]) {
            tmp[k++] = arr[i++];
        }
        else {
            tmp[k++] = arr[j++];
        }
    }
 
   
    while (i < n && i <= middle) {
        tmp[k++] = arr[i++];
    }
 
   
    for (int i = startPosition; i <= endPosition; i++) {
        arr[i] = tmp[i];
    }
   
    
} 

void sortMerge(int arr[], int tmp[], int start, int end,int n, int nbthreads)

{
   omp_set_num_threads(nbthreads);
   omp_set_schedule(omp_sched_static,1);
   
   for(int blockSize = 1; blockSize < end-start; blockSize = 2 * blockSize){
     #pragma omp parallel for schedule(runtime)
     for (int i = start; i <= end; i = i + 2 * blockSize){
     
            int  startPosition = i;
            int middle = i + blockSize - 1;
            int endPosition = min(i + 2*blockSize - 1, end);
 
            mergeArr(arr, tmp, startPosition, middle, endPosition,n);
     }
   }
}



int main (int argc, char* argv[]) {

  //forces openmp to create the threads beforehand
#pragma omp parallel
  {
    int fd = open (argv[0], O_RDONLY);
    if (fd != -1) {
      close (fd);
    }
    else {
      std::cerr<<"something is amiss"<<std::endl;
    }
  }
  
  if (argc < 3) { std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);
  int start = 0;
  int end = n-1;
  
  // get arr data
  int * arr = new int [n];
  generateMergeSortData (arr, n);
  
  int tmp[n];
  
   for (int i = 0; i < n; i++) {
        tmp[i] = arr[i];
    }

  //insert sorting code here.
  auto start_time = std::chrono::system_clock::now();
  sortMerge(arr, tmp, start, end, n, nbthreads);

  
  checkMergeSortResult (arr, n);
  
 std::chrono::time_point<std::chrono::system_clock> end_time = std::chrono::system_clock::now();
 std::chrono::duration<double> elapsed_seconds = end_time-start_time;
 std::cerr<<elapsed_seconds.count()<<std::endl;
  
  delete[] arr;

  return 0;
}
