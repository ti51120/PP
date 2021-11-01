// #include <pthread.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <iostream>

// int primes[10] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};

// // void *test(void *arg){
// //     int index = *(int*)arg;
// //     printf("%d ", primes[index]);
// // }

// int main(int argc, char *argv[]){

//     unsigned int g_seed = time(NULL) * 214013 + 2531011;
//     double x, y;
//     for(int i = 0; i < 5; i++){
//         int a = (g_seed >> 16) & 0x7FFF;
//         std::cout << a << '\n';
//     }

//     // pthread_t th[10];
//     // int i;
//     // for(i = 0; i < 10; i++){
//     //     int *ptr = (int*)malloc(sizeof(int));
//     //     *ptr = i;
//     //     if(pthread_create(&th[i], NULL, &test, (void*)ptr) != 0){
//     //         perror("Failed to create threads!\n");
//     //     }
//     // }

//     // for(i = 0; i < 10; i++){
//     //     if(pthread_join(th[i], NULL) != 0){
//     //         perror("Failed to join threads!\n");
//     //     }
//     // }
//     return 0;
// }
#include <iostream>

int main(int argc, char* argv[]){
    long long int threadNum = atoll(argv[1]);
    long long int tosses = atoll(argv[2]);
    std::cout << 3.1415926 << '\n';
}