#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
using namespace std;
int sum;
#ifdef __cplusplus
extern "C"
{
#endif

  void generateReduceData(int *arr, size_t n);

#ifdef __cplusplus
}
#endif

int getPartSum(int start, int end, int * arr) {

  int sum = 0;
  for(int j = start;j <= end;j++)
  {
    sum += arr[j];
  }
  return sum;
}

int main(int argc, char *argv[])
{
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
  
  if (argc < 3) {
    std::cerr<<"usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int * arr = new int [n];
  int sum;
  int stepCount = n/atoi(argv[2]);
  
  generateReduceData(arr, atoi(argv[1]));
  
  omp_set_num_threads(atoi(argv[2]));

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  #pragma omp parallel 
  {
    #pragma omp single
    for(int i = 0; i < n; i += stepCount) {

      int j, loopStrt = i, loopEnd = i+stepCount-1;
      
      if(loopEnd > n){
        loopEnd = n-1;
      }

      #pragma omp task untied 
      { 
        
        int partSum = 0;
        partSum = getPartSum(loopStrt, loopEnd, arr);
        
        #pragma omp taskwait
        
        #pragma omp critical
        sum += partSum;
      }
    }
  }
  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapased_seconds = end-start;
 
  std::cout << sum << std::endl;
  std::cerr<<elapased_seconds.count()<<std::endl;
  //delete[] arr;

  return 0;
}
