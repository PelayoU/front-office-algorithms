#ifndef PARSWAPBOOTSTRAP_H
#define PARSWAPBOOTSTRAP_H

#include <string>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "Core/IBootstrapInstrument.h"

/**
 * @brief Swap a la par usado en bootstrapping: calibra el DF en el último vencimiento
 * dado la annuity previa. Fórmula: DF_n = (1 - K * pre_annuity) / (1 + K * tau_n).
 */
class ParSwapBootstrap : public IBootstrapInstrument {
public:
    /**
     * @param schedule Fechas de los flujos (incluye inicio y todos los vencimientos).
     * @param rate Tipo fijo del swap (K) en decimal.
     * @param day_count "Actual/360" o "30/360" para calcular tau del último periodo.
     */
    ParSwapBootstrap(const std::vector<boost::gregorian::date>& schedule,
                     double rate,
                     const std::string& day_count = "Actual/360");

    /** @brief Última fecha del schedule (vencimiento del swap). */
    boost::gregorian::date getMaturityDate() const override;
    /** @brief Fracción de año del último periodo (desde el penúltimo al último nodo del schedule). */
    double getTau() const override;
    /** @brief DF en el vencimiento: (1 - K*pre_annuity) / (1 + K*tau_n). pre_annuity es la annuity de los nodos ya calibrados. */
    double computeDiscountFactor(double pre_annuity) const override;

private:
    std::vector<boost::gregorian::date> schedule_;
    double rate_;
    std::string day_count_;
    double tau_last_;
};

#endif
