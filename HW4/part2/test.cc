#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define root_rank 0
#define DEBUG 0

void construct_matrices(int *n_ptr, int *m_ptr, int *l_ptr, int **a_mat_ptr, int **b_mat_ptr){

    int world_size, world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if(world_rank == root_rank){
        scanf("%d %d %d", n_ptr, m_ptr, l_ptr);
        int n = *n_ptr, m = *m_ptr, l = *l_ptr;

        *a_mat_ptr = (int*)malloc(sizeof(int) * n * m);
        *b_mat_ptr = (int*)malloc(sizeof(int) * m * l);
    
        for(int row = 0; row < n; ++row){
            for(int col = 0; col < m; ++col){
                scanf("%d", *a_mat_ptr + row * m + col);
            }
        }
        for(int row = 0; row < m; ++row){
            for(int col = 0; col < l; ++col){
                scanf("%d", *b_mat_ptr + row * l + col);
            }
        }

        if(DEBUG){
            printf("n:%d m:%d l:%d\n", *n_ptr, *m_ptr, *l_ptr);
            
            printf("print the matrix...\n\n");
            for(int row = 0; row < n; ++row){
                for(int col = 0; col < m; ++col){
                    printf("%d ", *(*a_mat_ptr + row * m + col));
                }
                printf("\n");
            }
            printf("\n");
            
            for(int row = 0; row < m; ++row){
                for(int col = 0; col < l; ++col){
                    printf("%d ", *(*b_mat_ptr + row * l + col));
                }
                printf("\n");
            }
        }
    }
}

void matrix_multiply(const int n, const int m, const int l, const int *a_mat, const int *b_mat){
    int size;
    int my_rank;
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int block = n / size;
    int offset = n % size;

    switch(my_rank)
    {
        case 0:
        {
            // Define my value
            int *my_values = (int*)malloc(sizeof(int) * (block + offset) * m);
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
            int *my_values = (int*)malloc(sizeof(int) * (block + offset) * m);
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
}

int main(){
    int n, m, l;
    int *a_mat, *b_mat;

    MPI_Init(NULL, NULL);

    construct_matrices(&n, &m, &l, &a_mat, &b_mat);
    matrix_multiply(n, m, l, a_mat, b_mat);
    
    
    MPI_Finalize();
    return 0;
}