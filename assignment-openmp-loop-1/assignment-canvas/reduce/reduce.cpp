#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <chrono>
#include <unistd.h>

#ifdef __cplusplus
extern "C"
{
#endif
    void generateReduceData(int *arr, size_t n);
#ifdef __cplusplus
}
#endif

int main(int argc, char *argv[])
{
    #pragma omp parallel
    {
        int fd = open(argv[0], O_RDONLY);
        if (fd != -1)
        {
            close(fd);
        }
        else
        {
            std::cerr << "something is amiss" << std::endl;
        }
    }

    if (argc < 5)
    {
        std::cerr << "Usage: " << argv[0] << " <n> <nbthreads> <scheduling> <granularity>" << std::endl;
        return -1;
    }

    int n = atoi(argv[1]);
    int *arr = new int[n];
    int num_threads = atoi(argv[2]);
    char *schedulingType = argv[3];
    int gran = atoi(argv[4]);
    generateReduceData(arr, atoi(argv[1]));

    int res = 0;

    omp_set_num_threads(num_threads);

    if (schedulingType == "static")
    {
        omp_set_schedule(omp_sched_static, gran);
    }
    else if (schedulingType == "dynamic")
    {
        omp_set_schedule(omp_sched_dynamic, gran);
    }
    else
    {
        omp_set_schedule(omp_sched_guided, gran);
    }

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    // insert reduction code here

    #pragma omp parallel for schedule(runtime) reduction(+:res)
    for (int i = 0; i < n; ++i)
    {
        res = res + arr[i];
    }
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapased_seconds = end - start;

    std::cout << res << std::endl;
    std::cerr << elapased_seconds.count() << std::endl;

    delete[] arr;

    return 0;
}
