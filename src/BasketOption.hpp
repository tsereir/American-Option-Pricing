#include "Option.hpp"
class BasketOption : public Option
{
public:
    double strike_;
    PnlVect* lambdas_;
    BasketOption(double T, int dates, int size, double strike, PnlVect *lambda );
    double payoff(const PnlMat* path, double t) override;
};
