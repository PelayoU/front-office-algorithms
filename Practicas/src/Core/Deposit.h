#ifndef DEPOSIT_H
#define DEPOSIT_H

#include "Core/IBootstrapInstrument.h"
#include <boost/date_time/gregorian/gregorian.hpp>

/**
 * @brief Depósito a tipo simple (un solo flujo en el vencimiento).
 * Usado como primer instrumento en bootstrapping: DF = 1 / (1 + rate * tau).
 */
class Deposit : public IBootstrapInstrument {
public:
    /**
     * @param maturity Fecha de vencimiento.
     * @param tau Fracción de año desde hoy hasta maturity (Act/360 o 30/360).
     * @param rate Tipo de interés simple anual (decimal, p. ej. 0.05 para 5%).
     */
    Deposit(const boost::gregorian::date& maturity, double tau, double rate);

    /** @brief Fecha de vencimiento del depósito (único flujo). */
    boost::gregorian::date getMaturityDate() const override;
    /** @brief Fracción de año desde hoy hasta maturity; se usa en annuity y en la fórmula DF = 1/(1+r·τ). */
    double getTau() const override;
    /** @brief Factor de descuento en maturity: 1 / (1 + rate * tau). El parámetro pre_annuity no se usa en depósito. */
    double computeDiscountFactor(double pre_annuity) const override;

private:
    boost::gregorian::date maturity_;
    double tau_;
    double rate_;
};

#endif
