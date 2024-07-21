#pragma once
#include <iostream>
#include "pnl/pnl_vector.h"

class PricingResults
{
  private:
    double directPrice; // Prix Longstaff Schwartz
    double newSimulationPrice; // Prix obtenu avec de nouvelles simulations
    double newSimulationPriceStdDev; // Ecart type de l'estimateur du prix obtenu sur de nouvelles simulations

  public:
    PricingResults(double directPrice, double newSimulationPrice, double newSimulationPriceStdDev);

    friend std::ostream& operator<<(std::ostream& o, const PricingResults& res);
};
