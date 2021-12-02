#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(){
    int block = 6;
    // int m = 4;
    int m = 4;
    int l = 5;
    int c1 = 6;
    int c2 = 5;

    int buf[24] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3}; // 6x4
    int b_mat_local[20] = {0, 1, 2, 3, 4, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 4, 3, 2, 1, 0}; // 4x5
    int result[30]; // 6x5

    // for(int i = 0; i < a1; i++){
    //     for(int j = 0; j < b2; j++){
    //         c[i * c2 + j] = 0;
    //         for(int k = 0; k < a2; k++){
    //             c[i * c2 + j] += a[i * a2 + k] * b[k * b2 + j];
    //         }
    //     }
    // }

    for(int row = 0; row < block; ++row){
        for(int col = 0; col < l; ++col){
            result[row * l + col] = 0;
            for(int k = 0; k < m; ++k){
                result[row * l + col] += buf[row * m + k] * b_mat_local[k * l + col];
            }
        }
    }

    for(int i = 0; i < c1; i++){
        for(int j = 0; j < c2; j++){
            printf("%d ", result[i*c2+j]);
        }
        printf("\n");
    }
    return 0;
}