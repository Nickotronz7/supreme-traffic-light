#include "../headers/poisson.h"


double ran_expo(double lambda){
    double u;
    u = rand() / (RAND_MAX + 1.0);
    return (-log(1- u) / lambda)*100;
}