#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

using ll = long long int;

ll global_number_in_circle = 0;
ll tosses, threadNum;

void *count_pi(void *arg){
    int pos = *(int*)arg;
    ll local_numbers_in_circle = 0;
    
    unsigned seed = (int)time(NULL) + pos;    
    for(int i = pos; i < tosses; i += threadNum){
        double x = (double)rand_r(&seed) / RAND_MAX;        
        double y = (double)rand_r(&seed) / RAND_MAX;        
        if(x * x + y * y <= 1.0){
            local_numbers_in_circle++;
        }
    }
    
    *(ll*)arg = local_numbers_in_circle;
    return arg;
}

int main(int argc, char *argv[]){

    threadNum = atoll(argv[1]);
    tosses = atoll(argv[2]);

    pthread_t thds[threadNum];
    for(int i = 0; i < threadNum; ++i){
        int *ptr = new int(i);
        pthread_create(&thds[i], NULL, count_pi, (void*)ptr);
    }

    for(int i = 0; i < threadNum; ++i){
        ll *res;
        pthread_join(thds[i], (void**)&res);
        global_number_in_circle += *res;
        free(res);
    }
    
    double pi_estimate = 4 * global_number_in_circle / ((double)tosses);
    printf("%f\n", pi_estimate);
    
    return 0;
}