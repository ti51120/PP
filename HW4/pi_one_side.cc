#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

using ll = long long int;

ll cal_pi(ll iters, int world_rank){

    unsigned seed = (int)time(NULL) * (world_rank + 1); 
    ll local_nums_in_circle = 0;
    for(ll it = 0; it < iters; ++it){
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;
        if(x * x + y * y <= 1.0)
            local_nums_in_circle++;
    }
    return local_nums_in_circle;
}

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---


    // TODO: MPI init
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Win window;
    
    ll iters = tosses / world_size;
    ll local_cnts, global_cnts;
    
    
    if(world_rank == 0){
        // Master
        MPI_Win_create(&global_cnts, sizeof(ll), sizeof(ll), MPI_INFO_NULL, MPI_COMM_WORLD, &window);
        ll iters = tosses - (world_size - 1) * (tosses / world_size);
        local_cnts = cal_pi(iters, world_rank);
    }
    else{
        // Workers
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &window);
        local_cnts = cal_pi(iters, world_rank);
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, window);
        MPI_Accumulate(&local_cnts, 1, MPI_LONG_LONG, 0, 0, 1, MPI_LONG_LONG, MPI_SUM, window);
        MPI_Win_unlock(0, window);
    }
    MPI_Win_fence(0, window);
    MPI_Win_free(&window);

    if (world_rank == 0)
    {
        // TODO: handle PI result
        pi_result = 4 * (global_cnts + local_cnts) / (double)tosses;
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }
    
    MPI_Finalize();
    return 0;
}