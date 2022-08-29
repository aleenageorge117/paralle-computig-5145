#include <mpi.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {

    int rankVal, size, len;
    char arr[100];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rankVal);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Get_processor_name(arr, &len);
    cout << "I am process " << rankVal << " out of " << size << ". I am running on" << arr << endl;
    MPI_Finalize();

    return 0;

}





















