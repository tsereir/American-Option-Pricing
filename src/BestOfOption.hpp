#include "Option.hpp"
class BestOfOption : public Option {
public:
    PnlVect* lambdas_;
    double strike_;
    BestOfOption(double T, int dates, int size, double strike, PnlVect *lambda);
    double payoff(const PnlMat* path, double t) override;
};



