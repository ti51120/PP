#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "fasttime.h"

using ll = long long int;

struct interval{
    ll start;
    ll end;
};

void *count_pi(void *arg){
    interval *index = (interval*)arg;
    ll start = (*index).start;
    ll end = (*index).end;
    unsigned int seed = time(NULL);
    double x, y;
    ll local_numbers_in_circle = 0;
    for(int pos = start; pos < end; ++pos){
        x = (double)rand_r(&seed) / RAND_MAX;        
        y = (double)rand_r(&seed) / RAND_MAX;        
        if(x * x + y * y <= 1.0){
            local_numbers_in_circle++;
        }
    }
    *(ll*)arg = local_numbers_in_circle;
    return arg;
}

int main(int argc, char *argv[]){
    fasttime_t start = gettime();

    int threadNum = atoi(argv[1]);
    long long int tosses = atoll(argv[2]);
    pthread_t *thds = (pthread_t*)malloc(sizeof(pthread_t)*threadNum);

    for(int i = 0; i < threadNum; ++i){
        interval *ptr = (interval*)malloc(sizeof(interval));
        (*ptr).start = (tosses/threadNum)*i;
        (*ptr).end = (i == threadNum-1) ? (tosses-1) : (tosses/threadNum)*(i+1) - 1;
        if(pthread_create(&thds[i], NULL, count_pi, (void*)ptr)){
            perror("Failed to create thread!\n");
        }
    }
    ll global_number_in_circle = 0;
    for(int i = 0; i < threadNum; ++i){
        ll *res;
        if(pthread_join(thds[i], (void**)&res)){
            perror("Failed to join thread!\n");
        }
        global_number_in_circle += *res;
        free(res);
    }
    double pi_estimate = 4 * global_number_in_circle / ((double)tosses);
    printf("numbers in circle: %lld\n", global_number_in_circle);
    printf("pi: %f\n", pi_estimate);
    free(thds);

    fasttime_t end = gettime();
    printf("time: %f\n", tdiff(start, end));
    
    return 0;
}