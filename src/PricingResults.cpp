#include <iostream>
#include "PricingResults.hpp"
#include "json_helper.hpp"


PricingResults::PricingResults(double directPrice, double newSimulationPrice, double newSimulationPriceStdDev)
    : directPrice(directPrice)
    , newSimulationPrice(newSimulationPrice)
    , newSimulationPriceStdDev(newSimulationPriceStdDev)
{ }

std::ostream& operator<<(std::ostream& o, const PricingResults& res)
{
    nlohmann::json j = {
        {"direct american price", res.directPrice},
        {"new simulation american price", res.newSimulationPrice},
        {"new simulation american price stdDev", res.newSimulationPriceStdDev},
    };
    o << std::setw(4) << j << std::endl;
    return o;
}