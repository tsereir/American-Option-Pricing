#include "BasketOption.hpp"
#include <pnl/pnl_basis.h>

BasketOption::BasketOption(double T, int dates, int size, double strike, PnlVect* lambda) {
    T_ = T;
    dates_ = dates;
    size_ = size;
    strike_ = strike;
    lambdas_ = lambda; }

double BasketOption::payoff(const PnlMat* path, double t) {
    int index = t * dates_/T_;
    double somme = 0;
    for (int i = 0; i < size_; i++) {
        somme += GET(lambdas_, i) * MGET(path, index, i); }
    return std::max(somme - strike_, 0.0);
}
