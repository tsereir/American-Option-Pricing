#include <iostream>
#include <ctime>
#include <cmath>
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "PricingResults.hpp"

using namespace std;

int main()
{

    std::cout<< "TEST COMPILATION\n";

    PnlVect *G = pnl_vect_new();
    PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
    int M = 1E5;
    int dim = 2;
    pnl_rng_sseed(rng, time(NULL));

    double acc = 0.;
    double acc_square = 0.;

    for (int i = 0; i < M; i++)
    {
        pnl_vect_rng_normal(G, dim, rng);
        double tmp = pnl_vect_norm_two(G);
        acc += tmp;
        acc_square += tmp * tmp;
    }

    acc /= M;
    acc_square /= M;
    double stdDev = sqrt((acc_square - acc) / M);

    cout << "E = " << acc << "; stdDev = " << stdDev << endl;

    pnl_vect_free(&G);
    pnl_rng_free(&rng);
    return 0;
}
