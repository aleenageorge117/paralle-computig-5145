#include <mpi.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <tuple>

#ifdef __cplusplus
extern "C"
{
#endif

	float f1(float x, int intensity);
	float f2(float x, int intensity);
	float f3(float x, int intensity);
	float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

#define INITIAL_WORK_REQ 1
#define QUIT 1

float getSum(int fId, float x, int intensity)
{

	switch (fId)
	{
	case 1:
		return f1(x, intensity);
	case 2:
		return f2(x, intensity);
	case 3:
		return f3(x, intensity);
	case 4:
		return f4(x, intensity);
	default:
		return -1;
	}
}

float getNumInt(int start, int end, int functionId, int intensity, float a, float b, long n)
{
	float result = 0.0;
	float width = (b - a) / float(n);
	for (int i = start; i < end; i++)
	{
		float x = (a + (i + 0.5) * width);
		float func = getSum(functionId, x, intensity);
		result = result + (width * func);
	}
	return result;
}

std::tuple<int, int> getData(int Id, long n, int size)
{
	size = size - 1;
	int granularity = n / (size);
	int strtPtr = Id * granularity;
	int endPtr = strtPtr + granularity;

	if ((n % size != 0) && (endPtr > n))
	{
		endPtr = n;
	}

	return std::make_tuple(strtPtr, endPtr);
}

float masterFunct(long n, int size)
{
	int is_inital_req = 0;
	MPI_Status status;
	float finalRes = 0.0;
	int redId = -1;
	int workSent = 0;
	int start, end=0;
	float result=0.0;
	
	for (int i=1; i<size; i++) {
		if (end < n) {
			redId++;
			workSent++;
			std::tie(start, end) = getData(redId, n, size);
			int work[2]={0};
			work[0] = start;
			work[1] = end;
			MPI_Send(work, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
			}
			
		else {
			MPI_Send(0, 0, MPI_INT, i, QUIT, MPI_COMM_WORLD); }

		}
		
	while(workSent != 0) {
		MPI_Status status;
		MPI_Recv(&result, 1, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		int id = status.MPI_SOURCE;
		finalRes += result;
		workSent--;
		
		if (end < n) {
			redId++;
			workSent++;
			std::tie(start, end) = getData(redId, n, size);
			int work[2] = {0};
			work[0] = start;
			work[1] = end;
			MPI_Send(work, 2, MPI_INT, id, 0, MPI_COMM_WORLD); }
		
		else {
			MPI_Send(0, 0, MPI_INT, id, QUIT, MPI_COMM_WORLD); }
	
		}
			
	
	return finalRes;
}

void workerFunct(int functionId, int intensity, float a, float b, long n)
{
	float result = 0.0;
	int work[2] = {0};
	MPI_Status status;
	while (1)
	{
		MPI_Recv(work, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		int tag = status.MPI_TAG;
		if (tag != QUIT)
		{
			int start = work[0];
			int end = work[1];
			result = getNumInt(start, end, functionId, intensity, a, b, n);

			MPI_Send(&result, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
		}
		else
		{
			return;
		}
	}
}

int main(int argc, char *argv[])
{

	if (argc < 6)
	{
		std::cerr << "usage: mpirun " << argv[0] << " <functionid> <a> <b> <n> <intensity>" << std::endl;
		return -1;
	}

	int functionId, intensity;
	long int n;
	float a, b;
	float result = 0.0;
	MPI_Init(NULL, NULL);

	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	sscanf(argv[1], "%i", &functionId);
	sscanf(argv[2], "%f", &a);
	sscanf(argv[3], "%f", &b);
	sscanf(argv[4], "%ld", &n);
	sscanf(argv[5], "%i", &intensity);


	using namespace std::chrono;
	std::chrono::time_point<std::chrono::system_clock> start;

	if (rank == 0)
	{
		start = std::chrono::system_clock::now();
		result = masterFunct(n, size);
	}
	else
	{
		workerFunct(functionId, intensity, a, b, n);
	}

	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;

	if (rank == 0)
	{
		std::cout << result << std::endl;
		std::cerr << elapsed_seconds.count() << std::endl;
	}

	MPI_Finalize();
	return 0;
}