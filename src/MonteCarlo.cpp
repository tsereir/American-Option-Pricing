// Created by Tariq on 26/12/23.
#include "MonteCarlo.hpp"
#include "BasketOption.hpp"
#include <pnl/pnl_matrix.h>
#include <iostream>
#include <pnl/pnl_vector.h>

/*
   Simulation des prix d'une option américaine par Monte Carlo avec l'algorithme de Longstaff-Schwartz basé sur la technique de régression linéaire.
*/
MonteCarlo::MonteCarlo(BlackScholesModel* mod, Option* opt, PnlRng* rng)
        : mod_(mod), opt_(opt), rng_(rng) {}

MonteCarlo::~MonteCarlo() { mod_->~BlackScholesModel(); pnl_rng_free(&rng_);}

void MonteCarlo::price(int dates, int nbSimulation, int polynomial_degree, double& price, double& price_resimule, double& stdDevPrice) {
    int nb_actifs = mod_-> size_;
    double maturite = opt_->T_;
    int dates_1 = dates + 1;
    PnlBasis  *polynome = pnl_basis_create_from_degree(PNL_BASIS_HERMITE, polynomial_degree, nb_actifs);
    int nb_var_polynome = polynome->nb_func;
    PnlVect *alpha = pnl_vect_create_from_zero(nb_var_polynome);
    PnlVect *stopping_times = pnl_vect_create_from_scalar(nbSimulation, maturite);
    PnlMat* path = pnl_mat_create((dates_1) *nbSimulation, nb_actifs);
/*Génère les trajectoires des prix des actifs et rempli la matrice path*/
    generatePaths(dates, nbSimulation, path);
    PnlMat *X = pnl_mat_create(nbSimulation, nb_actifs);
    PnlVect* Y = pnl_vect_create(nbSimulation);
    solveLSM(dates, nbSimulation, path, X, Y, stopping_times, alpha, polynome, true);
    computeOptionPrice(path, stopping_times, dates, nbSimulation, price, stdDevPrice);
/*-----------------------------RESIMULATION ----------------------------- */
    PnlBasis  *polynome2 = pnl_basis_create_from_degree(PNL_BASIS_HERMITE, polynomial_degree, nb_actifs);
    PnlMat* path2= pnl_mat_create((dates_1) * nbSimulation, nb_actifs);
    generatePaths(dates, nbSimulation, path2);
    PnlVect *stopping_times2 = pnl_vect_create_from_scalar(nbSimulation, maturite);
    PnlMat *X_2 = pnl_mat_create(nbSimulation, nb_actifs);
    PnlVect* Y_2 = pnl_vect_create(nbSimulation);
    solveLSM(dates, nbSimulation, path2, X_2, Y_2, stopping_times2, alpha, polynome2, false);
    computeOptionPrice(path2, stopping_times2, dates, nbSimulation, price_resimule, stdDevPrice);
//libération de la mémoire
    pnl_vect_free(&alpha);
    pnl_mat_free(&path);
    pnl_mat_free(&path2);
    pnl_vect_free(&Y);
    pnl_mat_free(&X), pnl_basis_free(&polynome);
    pnl_vect_free(&Y_2), pnl_mat_free(&X_2),pnl_basis_free(&polynome2);
    delete mod_;
}
/* Génère les trajectoires des prix des actifs pour un nombre donné de simulations et de dates. */
void MonteCarlo::generatePaths(int dates, int nbSimulation, PnlMat* path) {
    int nb_actifs = mod_-> size_;
    PnlMat* res1 = pnl_mat_create(dates + 1, nb_actifs);
    for (int i= 0; i< nbSimulation; i++) {
        mod_->asset(res1, opt_->T_, dates, rng_);
        pnl_mat_set_subblock(path, res1, i*(dates + 1), 0);
    }
    pnl_mat_free(&res1);
}
/* Applique l'algorithme de Longstaff-Schwartz pour résoudre le problème de régression linéaire. */
void MonteCarlo::solveLSM(int dates, int nbSimulation, PnlMat* path, PnlMat* X, PnlVect* Y, PnlVect* stopping_times, PnlVect* alpha, PnlBasis* polynome, bool shouldFitLS) {
    int nb_actifs = mod_->size_;
    double maturite = opt_->T_;
    PnlMat* partialMatrix = pnl_mat_create(dates + 1, nb_actifs);
    PnlVect* sampleVector = pnl_vect_create(nbSimulation);
    for (int n = dates - 1 ; n > 0; n--) {
        for (int i = 0; i < nbSimulation; i++) {
            pnl_mat_get_row(sampleVector, path, n + i * (dates + 1));
            pnl_mat_set_row(X, sampleVector, i);
            pnl_mat_extract_subblock(partialMatrix, path, i * (dates + 1), dates + 1, 0, nb_actifs);
            double payoff = opt_->payoff(partialMatrix, pnl_vect_get(stopping_times, i));
            pnl_vect_set(Y, i, payoff * exp(-(mod_->r_) * (pnl_vect_get(stopping_times, i) - maturite * n/dates)));
        }
        if (shouldFitLS) {
            pnl_basis_fit_ls(polynome, alpha, X, Y);
        }
        for (int j = 0; j < nbSimulation; j++) {
            pnl_mat_extract_subblock(partialMatrix, path, j * (dates + 1), dates + 1, 0, nb_actifs);
            double payoff_j = opt_->payoff(partialMatrix, n * maturite / dates);
            pnl_mat_get_row(sampleVector, X, j);
            double continuation = pnl_basis_eval_vect(polynome, alpha, sampleVector);
            //Comparaison de la valeur de continuation au payoff
            double payoff_j_minus_continuation = payoff_j - continuation;
            if (payoff_j_minus_continuation >= 0 && continuation > 0) {
                pnl_vect_set(stopping_times, j, n *maturite/dates);
            }
        }
    }
    pnl_mat_free(&partialMatrix);
    pnl_vect_free(&sampleVector);
}

/* Calcule le prix de l'option en fonction des trajectoires générées et des temps d'arrêt, tout en estimant l'écart type du prix. */
void MonteCarlo::computeOptionPrice(PnlMat* path, PnlVect* stopping_times, int dates, int nbSimulation, double& price, double& stdDevPrice) {
    double varPrice = 0.0;
    int nb_actifs = mod_->size_;
    double interest_rate = mod_-> r_;
    PnlMat *partialMatrix = pnl_mat_create(dates + 1, nb_actifs);
    for (int j = 0; j < nbSimulation; j++) {
        pnl_mat_extract_subblock(partialMatrix, path, j * (dates+1), dates+1, 0, nb_actifs);
        double tau = pnl_vect_get(stopping_times, j);
        double payoff = opt_->payoff(partialMatrix, tau);
        price += opt_->payoff(partialMatrix, tau) * exp( - interest_rate*tau);
        varPrice += payoff * payoff * exp(-2 *interest_rate*tau);
    }
    price = price/nbSimulation;
    varPrice =  ((varPrice / nbSimulation) - price * price);
    stdDevPrice = sqrt(varPrice/nbSimulation);
    pnl_mat_free(&partialMatrix);
}