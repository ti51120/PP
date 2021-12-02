#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define root_rank 0
#define DEBUG 0

void construct_matrices(int *n_ptr, int *m_ptr, int *l_ptr, int **a_mat_ptr, int **b_mat_ptr){

    int world_size, world_rank, n, m, l;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if(world_rank == root_rank){
        //input parameters
        scanf("%d %d %d", n_ptr, m_ptr, l_ptr);
        n = *n_ptr;
        m = *m_ptr;
        l = *l_ptr;

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

        // send necessary parameters to other processes
        for (int dest = 1; dest < world_size; ++dest){
            MPI_Send(n_ptr, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Send(m_ptr, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Send(l_ptr, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
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
    else{
        // receive parameters sent from root_rank
        MPI_Recv(n_ptr, 1, MPI_INT, root_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(m_ptr, 1, MPI_INT, root_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(l_ptr, 1, MPI_INT, root_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

void matrix_multiply(const int n, const int m, const int l, const int *a_mat, const int *b_mat){
    int world_size, world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int block = n / world_size;
    int offset = n % world_size;
    int *buf = (int*)malloc(sizeof(int) * (block + offset) * m);

    switch(world_rank)
    {
        case 0:
        {
            int *counts = (int*)malloc(sizeof(int) * world_size);
            int *displacements = (int*)malloc(sizeof(int) * world_size);

            // setting counts[] and displacements[] for Scatterv()
            for(int p = 0; p < world_size; ++p){
                counts[p] = (!offset) ? block * m : ((!p) ? (block + offset) * m : block * m);
                displacements[p] = (!offset || !p) ? block * p * m : ((p == 1) ? (block + offset) * m : (block * p + offset) * m);
            }
            // dispatch partial block of a_mat to other processes
            int recv_count = (offset == 0) ? block * m : (block + offset) * m;
            MPI_Scatterv(a_mat, counts, displacements, MPI_INT, buf, recv_count, MPI_INT, root_rank, MPI_COMM_WORLD);

            // dispatch b_mat to other processes   
            for(int dest = 1; dest < world_size; ++dest){
                MPI_Send(b_mat, m * l, MPI_INT, dest, 0, MPI_COMM_WORLD);  
            }

            // do matrix multiplication
            int *c_mat = (int*)malloc(sizeof(int) * n * l);
            int *result = (int*)malloc(sizeof(int) * (block + offset) * l);
            for(int row = 0; row < (block+offset); ++row){
                for(int col = 0; col < l; ++col){
                    result[row * l + col] = 0;
                    for(int k = 0; k < m; ++k){
                        result[row * l + col] += buf[row * m + k] * b_mat[k * l + col];
                    }
                }
            }

            // setting counts[] and displacements[] for Gatherv()
            for(int p = 0; p < world_size; ++p){
                counts[p] = (!offset) ? block * l : ((!p) ? (block + offset) * l : block * l); 
                displacements[p] = (!offset || !p) ? block * p * l : ((p == 1) ? (block + offset) * l : (block * p + offset) * l);
            }

            // receive results from other processes
            MPI_Gatherv(result, (block+offset) * l, MPI_INT, c_mat, counts, displacements, MPI_INT, root_rank, MPI_COMM_WORLD);
    
            // print result
            for(int i = 0; i < n; i++){
                for(int j = 0; j < l; j++){
                    printf("%d ", c_mat[i*l + j]);
                }
                printf("\n");
            }
            free(c_mat);
            free(result);

            if(DEBUG){
                printf("print counts array....\n");
                for(int i = 0; i < world_size; i++){
                    printf("%d\n", counts[i]);
                }

                printf("\n");

                printf("print displacements array....\n");
                for(int i = 0; i < world_size; i++){
                    printf("%d\n", displacements[i]);
                }
        
                printf("\n");
        
                printf("part of a_mat for process %d\n\n", world_rank);
                for(int i = 0; i < 5; ++i){
                    for(int j = 0; j < 5; ++j){
                        printf("%d ", buf[i * m + j]);
                    }
                    printf("\n");
                }
               
                printf("\n");

                printf("print part of b_mat in root rank....\n");
                for(int i = 0; i < 5; i++){
                    for(int j = 0; j < 5; j++){
                        printf("%d ", b_mat[i*l + j]);
                    }
                    printf("\n");
                }

                printf("\n");

                printf("print part of c_mat in root rank....\n");
                for(int i = 0; i < 5; i++){
                    for(int j = 0; j < 5; j++){
                        printf("%d ", result[i*l + j]);
                    }
                    printf("\n");
                }
            }
            
            free(counts);
            free(displacements);
            break;
        }
        default:
        {
            int *b_mat_local = (int*)malloc(sizeof(int) * m * l);
            int *result = (int*)malloc(sizeof(int) * block * l);
            MPI_Scatterv(NULL, NULL, NULL, MPI_INT, buf, block * m, MPI_INT, root_rank, MPI_COMM_WORLD);
            MPI_Recv(b_mat_local, m * l, MPI_INT, root_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            //do matrix multiplication
            for(int row = 0; row < block; ++row){
                for(int col = 0; col < l; ++col){
                    result[row * l + col] = 0;
                    for(int k = 0; k < m; ++k){
                        result[row * l + col] += buf[row * m + k] * b_mat_local[k * l + col];
                    }
                }
            }
            
            // send result to root rank
            MPI_Gatherv(result, block * l, MPI_INT, NULL, NULL, NULL, MPI_INT, root_rank, MPI_COMM_WORLD);
            
            free(b_mat_local);
            free(result);
            
            if(DEBUG){
                printf("part of a_mat for process %d\n\n", world_rank);
                for(int i = 0; i < 5; ++i){
                    for(int j = 0; j < 5; ++j){
                        printf("%d ", buf[i * m + j]);
                    }
                    printf("\n");
                }
                
                printf("\n");

                printf("print part of b_mat in process %d....\n", world_rank);
                for(int i = 0; i < 5; i++){
                    for(int j = 0; j < 5; j++){
                        printf("%d ", b_mat_local[i*l + j]);
                    }
                    printf("\n");
                }

                printf("\n");

                printf("print part of c_mat in root process %d....\n", world_rank);
                for(int i = 0; i < 5; i++){
                    for(int j = 0; j < 5; j++){
                        printf("%d ", result[i*l + j]);
                    }
                    printf("\n");
                }
            }

            break;
        }
    }
    free(buf);
}

void destruct_matrices(int *a_mat, int *b_mat){
    int world_size, world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    if(world_rank == root_rank){
        free(a_mat);
        free(b_mat);
    }
}