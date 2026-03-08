#include "Core/ParSwapBootstrap.h"
#include "Core/actual_360.h"
#include "Core/thirty_360.h"
#include <stdexcept>

ParSwapBootstrap::ParSwapBootstrap(const std::vector<boost::gregorian::date>& schedule,
                                   double rate,
                                   const std::string& day_count)
    : schedule_(schedule), rate_(rate), day_count_(day_count), tau_last_(0.0)
{
    if (schedule_.size() < 2u)
        throw std::invalid_argument("ParSwapBootstrap: schedule debe tener al menos 2 fechas");
    if (day_count_ == "Actual/360") {
        Actual_360 conv;
        tau_last_ = conv(schedule_[schedule_.size() - 2u], schedule_.back());
    } else if (day_count_ == "30/360") {
        Thirty_360 conv;
        tau_last_ = conv(schedule_[schedule_.size() - 2u], schedule_.back());
    } else {
        throw std::invalid_argument("ParSwapBootstrap: day_count debe ser Actual/360 o 30/360");
    }
}

boost::gregorian::date ParSwapBootstrap::getMaturityDate() const {
    return schedule_.back();
}

double ParSwapBootstrap::getTau() const {
    return tau_last_;
}

double ParSwapBootstrap::computeDiscountFactor(double pre_annuity) const {
    return (1.0 - rate_ * pre_annuity) / (1.0 + rate_ * tau_last_);
}
