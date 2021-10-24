#include <iostream>
#include <random>
#include "fasttime.h"

using namespace std;

int main(int argc, char *argv[]){
    fasttime_t start = gettime();
    int number_in_circle = 0;
    std::random_device rd;
    std::default_random_engine gen = std::default_random_engine(rd());
    std::uniform_real_distribution<double> dis(-1, 1);

    long long int tosses = atoi(argv[1]);
    double x, y, dis_squr;
    for (int i=0; i<tosses; ++i){
        x = dis(gen);
        y = dis(gen);
        dis_squr = x * x + y * y;
        if(dis_squr <= 1) 
            number_in_circle++; 
    }
    double pi_estimate = 4 * number_in_circle / ((double)tosses);
    cout << "pi: " << pi_estimate << endl;
    fasttime_t end = gettime();
    printf("time: %f\n", tdiff(start, end));
    return 0;
}