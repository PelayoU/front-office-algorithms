#include "Core/Bootstrapping.h"

Bootstrapping::~Bootstrapping() = default;

Bootstrapping::DiscountCurveType Bootstrapping::operator()(
    const InstrumentsMapType& instruments,
    const Date& today) const
{
    DiscountCurveType curve;
    curve.insert(std::make_pair(today, 1.0));

    double annuity = 0.0;
    for (const auto& pair : instruments) {
        const Date& maturity = pair.first;
        const std::shared_ptr<IBootstrapInstrument>& inst = pair.second;
        const double df = inst->computeDiscountFactor(annuity);
        curve.insert(std::make_pair(maturity, df));
        annuity += inst->getTau() * df;
    }

    return curve;
}
