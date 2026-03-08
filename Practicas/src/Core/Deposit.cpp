#include "Core/Deposit.h"

Deposit::Deposit(const boost::gregorian::date& maturity, double tau, double rate)
    : maturity_(maturity), tau_(tau), rate_(rate) {}

boost::gregorian::date Deposit::getMaturityDate() const {
    return maturity_;
}

double Deposit::getTau() const {
    return tau_;
}

double Deposit::computeDiscountFactor(double /* pre_annuity */) const {
    return 1.0 / (1.0 + rate_ * tau_);
}
