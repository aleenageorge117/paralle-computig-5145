#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <mpi.h>

using namespace std;
#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

float getSum(int fId, float x, int intensity) {
  switch(fId){
    case 1:
    {
      return f1(x, intensity);
    }
    case 2:
    {
      return f2(x, intensity);
    }
    case 3:
    {
      return f3(x, intensity);
    }
    case 4:
    {
      return f4(x, intensity);
    }
  }
  return 0; 
}

void getNumInt(int functionid, float a, float b, int intensity, int n, int rankVal, int size) {
  
  float commonFact = (b-a)/n;
  float summation = 0;
  float sum;
  float x;

  for (int i=(rankVal)*n/size; i<(rankVal+1)*n/size; i++) {
    x = a + (i+0.5)*commonFact;
    summation += getSum(functionid, x, intensity);
  }
      
  summation = commonFact*summation;

  MPI_Reduce(&summation, &sum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
  
  if(rankVal == 0)
    cout<<sum;
}
  
int main (int argc, char* argv[]) {
  
  if (argc < 6) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return 0;
  }

  int fId = atoi(argv[1]);
  float a = atof(argv[2]);
  float b = atof(argv[3]);
  int n = atoi(argv[4]);
  int intensity = atoi(argv[5]);

  int rankVal, size;

  MPI_Init(NULL, NULL);

  MPI_Comm_rank(MPI_COMM_WORLD, &rankVal);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  std::chrono::time_point<std::chrono::system_clock> start; 
  
  if(rankVal ==0)
    start = std::chrono::system_clock::now();

  getNumInt(fId, a, b, intensity, n, rankVal, size);


  MPI_Finalize();

  if(rankVal==0){
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr<<elapsed_seconds.count()<<std::endl;
  }

  return 0;
}