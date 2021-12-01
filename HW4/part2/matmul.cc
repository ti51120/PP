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

        for (int dest = 1; dest < world_size; ++dest){
            MPI_Send(n_ptr, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Send(m_ptr, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Send(l_ptr, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            // MPI_Send(*b_mat_ptr, m * l, MPI_INT, dest, 0, MPI_COMM_WORLD);
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
    
    printf("n:%d m:%d l:%d\n", n, m, l);
    
    switch(world_rank)
    {
        case 0:
        {
            int *counts = (int*)malloc(sizeof(int) * world_size);
            int *displacements = (int*)malloc(sizeof(int) * world_size);
            
            for(int p = 0; p < world_size; ++p){
                counts[p] = (!offset) ? block * m : ((!p) ? (block + offset) * m : block * m);
                displacements[p] = (!offset || !p) ? block * p * m : ((p == 1) ? (block + offset) * m : (block * p + offset) * m);
            }
            

            int recv_count = (offset == 0) ? block * m : (block + offset) * m;
            MPI_Scatterv(a_mat, counts, displacements, MPI_INT, buf, recv_count, MPI_INT, root_rank, MPI_COMM_WORLD);

            if(DEBUG){
                printf("print counts array....\n");
                for(int i = 0; i < world_size; i++){
                    printf("%d\n", counts[i]);
                }

                printf("print displacements array....\n");
                for(int i = 0; i < world_size; i++){
                    printf("%d\n", displacements[i]);
                }
                
                printf("Matrix for process %d\n\n", world_rank);
                for(int i = 0; i < (block + offset); ++i){
                    for(int j = 0; j < m; ++j){
                        printf("%d ", buf[i * m + j]);
                    }
                    printf("\n");
                }
                printf("\n");
            }
           
            break;
        }
        default:
        {
            // int *buf = (int*)malloc(sizeof(int) * (block + offset) * m);
            MPI_Scatterv(NULL, NULL, NULL, MPI_INT, buf, block * m, MPI_INT, root_rank, MPI_COMM_WORLD);

            if(DEBUG){
                printf("Process %d\n\n", world_rank);
                for(int i = 0; i < block; ++i){
                    for(int j = 0; j < m; ++j){
                        printf("%d ", buf[i * m + j]);
                    }
                    printf("\n");
                }
                printf("\n");
            }

            break;
        }
    }
}

void destruct_matrices(int *a_mat, int *b_mat){

}


// void matrix_multiply(const int n, const int m, const int l,
//                      const int *a_mat, const int *b_mat){
//     int size, rank;
//     MPI_Status status;
//     MPI_Comm_size(MPI_COMM_WORLD, &size);
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     int numworker, source, dest, mtype, rows, averow, extra, offset;
//     int N, M, L;
//     int i, j, k;
//     numworker = size - 1;
//     if (rank == MASTER){
// 	int *c;
//     	c = (int*)malloc(sizeof(int) * n * l);
//         /* Send matrix data to the worker tasks */
//         averow = n / numworker;
//         extra = n % numworker;
//         offset = 0;
//         mtype = FROM_MASTER;
//         for (dest = 1; dest <= numworker; dest++){
//             MPI_Send(&n, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
//             MPI_Send(&m, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
//             MPI_Send(&l, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
//             rows = (dest <= extra)? averow + 1: averow;
//             MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
//             MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
// 	    // printf("master send info to rank %d\n", dest);
//             MPI_Send(&a_mat[offset * m], rows * m, MPI_INT, dest, mtype, MPI_COMM_WORLD);
//             MPI_Send(&b_mat[0], m * l, MPI_INT, dest, mtype, MPI_COMM_WORLD);
//             offset += rows;
//         }
//         /* Receive results from worker tasks */
//         mtype = FROM_WORKER;
//         for (i = 1; i <= numworker; i++){
//             source = i;
//             MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
//             MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
//             MPI_Recv(&c[offset * l], rows * l, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
// 	    // printf("master receive from %d\n", source);
//         }
//         /* Print results */
//         // for (i = 0; i < n; i++){
//         //     for (j = 0; j < l; j++){
//         // 	printf("%d", c[i * l + j]);
// 		// if (j != l-1) printf(" ");
//         //     }
//         //     printf("\n");
//         // }
// 	free(c);
//     }
//     if (rank > MASTER){
//         mtype = FROM_MASTER;
//         MPI_Recv(&N, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
//         MPI_Recv(&M, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
//         MPI_Recv(&L, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
// 	int *a;
//     	int *b;
//     	int *c;
//     	a = (int*)malloc(sizeof(int) * N * M);
//     	b = (int*)malloc(sizeof(int) * M * L);
//     	c = (int*)malloc(sizeof(int) * N * L);
// 	// printf("n: %d, m: %d, l: %d\n", N, M, L);
//         MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
//         MPI_Recv(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
// 	// printf("rank %d receive from master\n", rank);
//         MPI_Recv(&a[0], rows * M, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
//         MPI_Recv(&b[0], M * L, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
// 	// printf("\n");
// 	// for (int i = 0; i < rows * M; i++) printf("a[%d]: %d\n", i, a[i]);
// 	// for (int i = 0; i < L * M; i++) printf("b[%d]: %d\n", i, b[i]);
// 	// printf("\n");

//         for (k = 0; k < L; k++){
//             for (i = 0; i < rows; i++){
//         	c[i * L + k] = 0;
//         	for (j = 0; j < M; j++){
//         	    c[i * L + k] += a[i * M + j] * b[j * L + k];
// 		    // printf("a[%d][%d] = %d\n", i, j, a[i*M + j]);
// 		    // printf("b[%d][%d] = %d\n", j, k, b[j*L + k]);
//         	}
//             }
//         }

//         mtype = FROM_WORKER;
//         MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
//         MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
//         MPI_Send(&c[0], rows * L, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
// 	// printf("rank %d send result\n", rank);
// 	free(a);
//     	free(b);
// 	free(c);
//     }
// }