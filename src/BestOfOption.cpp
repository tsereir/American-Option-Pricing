#include "BestOfOption.hpp"
#include "pnl/pnl_vector.h"
using namespace std;
BestOfOption::BestOfOption(double T, int dates, int size, double strike, PnlVect *lambda) {
    T_ = T;
    size_ = size;
    dates_ = dates;
    strike_ = strike;
    lambdas_ = lambda;
}
double BestOfOption::payoff(const PnlMat *path, double t) {
    int index = t * dates_ / T_;
    PnlVect *produit = pnl_vect_create(size_);
    for (int i = 0; i < size_; i++) {
        pnl_vect_set(produit, i, GET(lambdas_, i) * MGET(path, index, i));
    }
    double max_vecteur_produit = pnl_vect_max(produit);
    return std::max(max_vecteur_produit - strike_, 0.0);
}
