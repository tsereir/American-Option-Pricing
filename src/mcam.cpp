#include "json_helper.hpp"
#include "BlackScholesModel.hpp"
#include "pnl/pnl_random.h"
#include <fstream>
#include <iostream>
#include "MonteCarlo.hpp"
#include "pnl/pnl_vector.h"
#include "BasketOption.hpp"
#include "PricingResults.hpp"
#include "GeometricPut.hpp"
#include "BestOfOption.hpp"
#include "nlohmann/json.hpp"
/* Main du projet, chargement des paramètres depuis un fichier JSON et calcul du prix d'une option americaine */
std::string option_type;
PnlVect* volatility, *spot, *dividende;
double strike, maturity, r, rho, reference_price;
int size, dates, degree, n_samples;
double directPrice, newSimulationPrice, newSimulationPriceStdDev  = 0.0;

void basketCompute(PnlVect* lambda) {
    PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    BasketOption* option = new BasketOption(maturity, dates, size, strike, lambda);
    BlackScholesModel* mod = new BlackScholesModel(size, r, rho, volatility, spot, dividende);
    MonteCarlo* monteCarlo_ = new MonteCarlo(mod, option, rng);
    monteCarlo_->price(dates, n_samples, degree, directPrice, newSimulationPrice, newSimulationPriceStdDev);
    std::cout << PricingResults(directPrice, newSimulationPrice, newSimulationPriceStdDev) << std::endl;
    delete option, mod, monteCarlo_;
}
void putGeomCompute() {
    PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    GeometricPut* option = new GeometricPut(maturity, dates, size, strike);
    BlackScholesModel* mod = new BlackScholesModel(size, r, rho, volatility, spot, dividende);
    MonteCarlo* monteCarlo_ = new MonteCarlo(mod, option, rng);
    monteCarlo_->price(dates, n_samples, degree, directPrice, newSimulationPrice, newSimulationPriceStdDev);
    std::cout << PricingResults(directPrice, newSimulationPrice, newSimulationPriceStdDev) << std::endl;
    delete option, mod, monteCarlo_;
}
void bestOfCompute(PnlVect* lambdas) {
    PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    BestOfOption* option = new BestOfOption(maturity, dates, size, strike, lambdas);
    BlackScholesModel* mod = new BlackScholesModel(size, r, rho, volatility, spot, dividende);
    MonteCarlo* monteCarlo_ = new MonteCarlo(mod, option, rng);
    monteCarlo_->price(dates, n_samples, degree, directPrice, newSimulationPrice, newSimulationPriceStdDev);
    std::cout << PricingResults(directPrice, newSimulationPrice, newSimulationPriceStdDev) << std::endl;
    delete option, mod, monteCarlo_;
}
/* Vérifie et redimensionne les vecteurs du JSON pour correspondre à la taille du modèle. */
PnlVect* resize_vect(PnlVect* vec, int size, const nlohmann::json json, const std::string key) {
    try {
        json.at(key).get_to(vec);
        if (vec->size != size) {
            vec = pnl_vect_create_from_zero(size);
            double coefficientValue = json.at(key)[0];
            pnl_vect_set_double(vec, coefficientValue);
        }
    } catch (const std::out_of_range &e) {
        std::cerr << "Exception : " << e.what() << std::endl;}
    return vec;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <json_file>" << std::endl;
        return 1;
    }
    std::ifstream ifs(argv[1]);
    if (!ifs.is_open()) {
        std::cerr << "Error opening JSON file: " << argv[1] << std::endl;
        return 1;
    }
    nlohmann::json j = nlohmann::json::parse(ifs);
    option_type = j.at("option type").get<std::string>();
    j.at("model size").get_to(size);
    j.at("strike").get_to(strike);
    j.at("maturity").get_to(maturity);
    j.at("interest rate").get_to(r);
    j.at("correlation").get_to(rho);
    j.at("MC iterations").get_to(n_samples);
    j.at("reference price").get_to(reference_price);
    volatility = resize_vect(volatility, size, j, "volatility");
    dividende = resize_vect(dividende, size, j, "dividend rate");
    spot = resize_vect(spot, size, j, "spot");
    j.at("dates").get_to(dates);
    j.at("degree for polynomial regression").get_to(degree);
//----------------- BASKET ----------
    if (option_type == "basket") {
        PnlVect *lambdas;
        lambdas = resize_vect(lambdas, size, j, "payoff coefficients");
        basketCompute(lambdas);
    }
//-----------------GEOMETRIC PUT---
    if (option_type == "geometric_put") {
        putGeomCompute();
    }
//----------------- BESTOF ---------
    if (option_type == "bestof") {
        PnlVect *lambdas;
        lambdas = resize_vect(lambdas, size, j, "payoff coefficients");
        bestOfCompute(lambdas);
    }
//libération
    if (volatility != nullptr) pnl_vect_free(&volatility);
    if (dividende != nullptr) pnl_vect_free(&dividende);
    if (spot != nullptr) pnl_vect_free(&spot);
}