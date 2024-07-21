#pragma once
#include "pnl/pnl_band_matrix.h"
#include "pnl/pnl_basis.h"
#include "Option.hpp"
#include "BlackScholesModel.hpp"
#include "pnl/pnl_random.h"

class MonteCarlo
{
public:
    BlackScholesModel *mod_;
    Option *opt_;
    PnlRng* rng_;
    MonteCarlo(BlackScholesModel* mod, Option* opt, PnlRng* rng);
    
    ~MonteCarlo();
    void price(int dates, int nbSimulation, int polynomial_degree, double& price, double& price_resimule, double& stdDevPrice);
    void generatePaths(int dates, int nbSimulation, PnlMat* path);
    void solveLSM(int dates, int nbSimulation, PnlMat* path, PnlMat* X,
                  PnlVect* Y, PnlVect* stopping_times, PnlVect* alpha, PnlBasis* polynome, bool shouldFitLS);
    void computeOptionPrice(PnlMat* path, PnlVect* stopping_times, int dates, int nbSimulation, double& price, double& stdDevPrice);
};