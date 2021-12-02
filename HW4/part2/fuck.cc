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

    // int *my_values = (int*)malloc(sizeof(int) * (block + offset) * m);
    switch(my_rank)
    {
        case 0:
        {
            // Define my value
            int my_value[3] = {100, 100, 100};
 
            // Define the receive counts
            int counts[3] = {3, 2, 2};
 
            // Define the displacements
            int displacements[3] = {0, 3, 5};
 
            int* buffer = (int*)calloc(7, sizeof(int));
            printf("Process %d, my value = %d %d.\n", my_rank, my_value[0], my_value[1]);
            MPI_Gatherv(&my_value, 3, MPI_INT, buffer, counts, displacements, MPI_INT, root_rank, MPI_COMM_WORLD);
            printf("Values gathered in the buffer on process %d:", my_rank);
            for(int i = 0; i < 7; i++)
            {
                printf(" %d", buffer[i]);
            }
            printf("\n");
            free(buffer);
            break;
        }
        case 1:
        {
            // Define my value
            int my_value[2] = {101, 101};
 
            printf("Process %d, my value = %d %d.\n", my_rank, my_value[0], my_value[1]);
            MPI_Gatherv(&my_value, 2, MPI_INT, NULL, NULL, NULL, MPI_INT, root_rank, MPI_COMM_WORLD);
            break;
        }
        case 2:
        {
            // Define my values
            int my_values[2] = {102, 102};
 
            printf("Process %d, my values = %d %d.\n", my_rank, my_values[0], my_values[1]);
            MPI_Gatherv(my_values, 2, MPI_INT, NULL, NULL, NULL, MPI_INT, root_rank, MPI_COMM_WORLD);
            break;
        }
    }
 
    MPI_Finalize();
 
    return EXIT_SUCCESS;
}