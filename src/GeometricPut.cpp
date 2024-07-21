#include "GeometricPut.hpp"
/* Option de type put géométrique. */
GeometricPut::GeometricPut(double T, int dates, int size, double strike) {
    T_ = T;
    dates_ = dates;
    size_ = size;
    strike_ = strike;
}
double GeometricPut::payoff(const PnlMat* path, double t) {
    int index = t * dates_ / T_;
    PnlVect* row = pnl_vect_create(size_);
    double inverse = 1 / static_cast<double>(size_);
    pnl_mat_get_row(row, path, index);
    double produit = pnl_vect_prod(row);
    double payoff = std::max(strike_ - pow(produit, inverse), 0.0);
    pnl_vect_free(&row);
    return payoff;
}
