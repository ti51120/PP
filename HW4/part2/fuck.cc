/**
 * @author RookieHPC
 * @brief Original source code at https://www.rookiehpc.com/mpi/docs/mpi_scatterv.php
 **/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
 
/**
 * @brief Illustrates how to use the variable version of a scatter.
 * @details A process is designed as root and begins with a buffer containig all
 * values, and prints them. It then dispatches these values to all the processes
 * in the same communicator. Other process just receive the dispatched value(s)
 * meant for them. Finally, everybody prints the value received. This
 * application is designed to cover all cases:
 * - Different send counts
 * - Different displacements
 * This application is meant to be run with 3 processes.
 *
 *       +-----------------------------------------+
 *       |                Process 0                |
 *       +-----+-----+-----+-----+-----+-----+-----+
 *       | 100 |  0  | 101 | 102 |  0  |  0  | 103 |
 *       +-----+-----+-----+-----+-----+-----+-----+
 *         |            |     |                |
 *         |            |     |                |
 *         |            |     |                |
 *         |            |     |                |
 *         |            |     |                |
 *         |            |     |                |
 * +-----------+ +-------------------+ +-----------+
 * | Process 0 | |    Process 1      | | Process 2 |
 * +-+-------+-+ +-+-------+-------+-+ +-+-------+-+
 *   | Value |     | Value | Value |     | Value |
 *   |  100  |     |  101  |  102  |     |  103  |
 *   +-------+     +-------+-------+     +-------+ 
 *                
 **/
int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
 
    // Get number of processes and check that 3 processes are used
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // if(size != 3)
    // {
    //     printf("This application is meant to be run with 3 processes.\n");
    //     MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    // }
 
    // Determine root's rank
    int root_rank = 0;
 
    // Get my rank
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    int n = 12;
    int m = 15;
    int block = n / size;
    int offset = n % size;

    int *my_values = (int*)malloc(sizeof(int) * (block + offset) * m);
    switch(my_rank)
    {
        case 0:
        {
            // Define my value
            int *counts = (int*)malloc(sizeof(int) * size);
            int *displacements = (int*)malloc(sizeof(int) * size);
            
            int *buffer = (int*)malloc(sizeof(int) * n * m);
            for(int i = 0; i < n * m; ++i){
                buffer[i] = i;
            }
            
            
            for(int p = 0; p < size; ++p){
                counts[p] = (!offset) ? block * m : ((!p) ? (block + offset) * m : block * m);
                displacements[p] = (!offset || !p) ? block * p * m : ((p == 1) ? (block + offset) * m : (block * p + offset) * m);
            }

            int recv_count = (offset == 0) ? block * m : (block + offset) * m;
            MPI_Scatterv(buffer, counts, displacements, MPI_INT, my_values, recv_count, MPI_INT, root_rank, MPI_COMM_WORLD);
           
           
            printf("Process %d\n\n", my_rank);
            for(int i = 0; i < (block + offset); ++i){
                for(int j = 0; j < m; ++j){
                    printf("%d ", my_values[i * m + j]);
                }
                printf("\n");
            }
            printf("\n");
            break;
        }
        default:
        {
            // Declare my values
            // int *my_values = (int*)malloc(sizeof(int) * (block + offset) * m);
            MPI_Scatterv(NULL, NULL, NULL, MPI_INT, my_values, block * m, MPI_INT, root_rank, MPI_COMM_WORLD);
            
            
            printf("Process %d\n\n", my_rank);
            for(int i = 0; i < block; ++i){
                for(int j = 0; j < m; ++j){
                    printf("%d ", my_values[i * m + j]);
                }
                printf("\n");
            }
            printf("\n");
            break;
        }
    }
 
    MPI_Finalize();
 
    return EXIT_SUCCESS;
}
