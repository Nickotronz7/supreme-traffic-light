#include "../headers/poisson.h"
//=-------------------------------------------------------------------------=
//=  Author: Ken Christensen                                                =
//=          University of South Florida                                    =
//=          WWW: http://www.csee.usf.edu/~christen                         =
//=          Email: christen@csee.usf.edu                                   =
//=-------------------------------------------------------------------------=
//declare functions
double funUniformSingle();           //generate uniform random variables on (0,1)
int funPoissonSingle(double lambda); //generate Poisson variables with parameter (ie mean) lambda

//START Main
// int main()
// {
//     time_t s; //use CPU time for seed
//     //intializes random number generator
//     srand((unsigned)time(&s));

//     double lambda = 4.0; //lambda is the Poisson parameter (that is, its mean)

//     int numbSim = 100; // number of variables

//     //START Collect statistists on Poisson variables
//     //initialize statistics
//     int numbPoissonTemp;
//     double sumPoisson = 0;
//     double sumPoissonSquared = 0;

//     //loop through for each random variable
//     for (int i = 0; i < numbSim; i++)
//     {
//         //generate a single poisson variable
//         numbPoissonTemp = funPoissonSingle(lambda);

//         //total sum of variables
//         sumPoisson += numbPoissonTemp;
//         //total sum of squared variables
//         sumPoissonSquared += pow(numbPoissonTemp, 2);
//     }
//     return (numbPoissonTemp);
// }
//END Main

//START Function definitions
//Poisson function -- returns a single Poisson random variable
int funPoissonSingle(double lambda)
{
    double exp_lambda = exp(-lambda); //constant for terminating loop
    double randUni;                   //uniform variable
    double prodUni;                   //product of uniform variables
    int randPoisson; //Poisson variable

    //initialize variables
    randPoisson = -1; 
    prodUni = 1;          
    do
    {
        randUni = funUniformSingle(); //generate uniform variable
        prodUni = prodUni * randUni;  //update product
        randPoisson++;                //increase Poisson variable

    } while (prodUni > exp_lambda); 
    return randPoisson;
}

////Uniform function -- returns a standard uniform random variable
double funUniformSingle()
{
    double randUni;
    randUni = (double)rand() / (double)((unsigned)RAND_MAX + 1); //generate random variables on (0,1)
    return randUni;
}
