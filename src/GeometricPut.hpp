#include "Option.hpp"

class GeometricPut : public Option {
public:
    double strike_;
    GeometricPut(double T, int dates, int size, double strike);
    double payoff(const PnlMat* path, double t) override;
};
