#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int primes[10] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};

void *test(void *arg){
    int index = *(int*)arg;
    printf("%d ", primes[index]);
}

int main(int argc, char *argv[]){

    pthread_t th[10];
    int i;
    for(i = 0; i < 10; i++){
        int *ptr = (int*)malloc(sizeof(int));
        *ptr = i;
        if(pthread_create(&th[i], NULL, &test, (void*)ptr) != 0){
            perror("Failed to create threads!\n");
        }
    }

    for(i = 0; i < 10; i++){
        if(pthread_join(th[i], NULL) != 0){
            perror("Failed to join threads!\n");
        }
    }
    return 0;
}