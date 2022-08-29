#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include <omp.h>
#include <math.h>

#ifdef __cplusplus
extern "C"
{
#endif
  void generatePrefixSumData(int *sortArr, size_t n);
  void checkPrefixSumResult(int *sortArr, size_t n);
#ifdef __cplusplus
}
#endif

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0] << " <n> <>" << std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int nbthread = atoi(argv[2]);
  omp_set_num_threads(nbthread);

  int *sortArr = new int[n];
  generatePrefixSumData(sortArr, n);

  int *prefix = new int[n + 1];
  int *temp = new int[nbthread];
  prefix[0] = 0;

  //insert prefix sum code here
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    #pragma omp parallel
  {
    int index = omp_get_thread_num();
    int sum = 0;

    #pragma omp for schedule(static)
    for (int i = 0; i < n; i++)
    {
      sum += sortArr[i];
      prefix[i + 1] = sum;
    }

    #pragma omp critical
    temp[index] = sum;

    #pragma omp barrier
    int x = 0;
    for (int j = 0; j < index; j++)
    {
      x += temp[j];
    }

    #pragma omp for schedule(static)
    for (int k = 0; k < n; k++)
    {
      prefix[k + 1] += x;
    }
  }

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end - start;

  checkPrefixSumResult(prefix, n);
  std::cerr << elapsed_seconds.count() << std::endl;

  delete[] sortArr;

  return 0;
}