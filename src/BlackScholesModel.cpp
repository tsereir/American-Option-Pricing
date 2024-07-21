// Created by tariq on 16/01/24.

#include "BlackScholesModel.hpp"
#include <iostream>
#include <cmath>
using namespace  std;

/*
   Implémentation d'un modèle Black-Scholes pour la simulation des trajectoires des prix d'actifs.
   Le modèle prend en compte les paramètres tels que taux d'intérêt, volatilité, dividendes, et effectue une simulation en utilisant la décomp de Cholesky.
*/

BlackScholesModel::BlackScholesModel(int size, double r, double rho, PnlVect *sigma, PnlVect *spot, PnlVect *divid) {
    divid_ = divid;
    spot_ = spot;
    size_ = size;
    r_ = r;
    rho_ = rho;
    sigma_ = sigma;
    //Crée une matrice diagonale avec des éléments égaux à 'rho' et effectue ensuite une décomposition de Cholesky sur cette matrice.
    pnl_mat_set_diag(choleskyMatrice_ = pnl_mat_create_from_scalar(size, size, rho), 1, 0);
    pnl_mat_chol(choleskyMatrice_);
}
BlackScholesModel::~BlackScholesModel() { pnl_mat_free(&choleskyMatrice_); }

void BlackScholesModel::asset(PnlMat *path, double T, int dates, PnlRng *rng) {
    PnlVect* G = pnl_vect_create(size_);
    PnlVect *ligneCholesky = pnl_vect_create(size_);
    int nb_dates = path->m;
    pnl_mat_set_row(path,spot_, 0);
    if (nb_dates != dates + 1) {
        std::cerr << "Erreur : le nombre de dates doit être égal à dates + 1." << std::endl;
    }
    double t = T/dates;
    //boucle sur les instants (lignes)
    for (int i=1; i <=dates; i++) {
        //G est un vecteur d'échantillons iid de loi normale
        pnl_vect_rng_normal(G, size_, rng);
        //boucle sur les actifs
        for (int d= 0; d< size_; d++) {
            double dividende_actif = pnl_vect_get(divid_, d);
            double sigma_actif = pnl_vect_get(sigma_, d);
            double drift = r_ - dividende_actif - 0.5*sigma_actif*sigma_actif;
            double dernierprix = pnl_mat_get(path, i-1, d);
            pnl_mat_get_row(ligneCholesky, choleskyMatrice_, d);
            double prix_actif = dernierprix * exp((drift)*t + sqrt(t)*sigma_actif* pnl_vect_scalar_prod(ligneCholesky, G));
            pnl_mat_set(path, i, d, prix_actif);
        }
    }
    pnl_vect_free(&G);
    pnl_vect_free(&ligneCholesky);
}