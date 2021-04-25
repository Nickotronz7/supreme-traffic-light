#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double ran_expo(double lambda){
    double u;
    u = rand() / (RAND_MAX + 1.0);
    return -log(1- u) / lambda;
}

int main(void)
{
    int i;
    srand((unsigned)time(NULL));
    printf("%f\n", ran_expo(0.05));
    return 0;
}