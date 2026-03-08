#include "Rates/FloatingLeg.h"

#include <cmath>
#include <stdexcept>

#include "Core/actual_360.h"
#include "Core/thirty_360.h"

FloatingLeg::FloatingLeg(const std::vector<boost::gregorian::date>& schedule, double notional, const std::string& day_count,double spread)
    : schedule_(schedule), notional_(notional), day_count_(day_count), spread_(spread) {}





double FloatingLeg::year_fraction(const boost::gregorian::date& from, const boost::gregorian::date& to) const {
    if (day_count_ == "Actual/360") {
        Actual_360 conv;
        return conv(from, to);
    }
    if (day_count_ == "30/360") {
        Thirty_360 conv;
        return conv(from, to);
    }
    throw std::invalid_argument("FloatingLeg: day_count desconocido");
}



/** 
 * @brief Calcula el tipo forward continuamente compuesto para el periodo [start, end] y con el saca el Rm (Inteses Simple que genera el mismo factor de descuento o capitalizacion continua).
 * @note Es decir,get_period_rate no devuelve el forward en continua, sino el tipo simple del periodo (el “Euribor” del periodo): la tasa simple que tiene el mismo factor de capitalización que el forward continuamente compuesto.
 * @param curve Curva de descuento.
 * @param start Fecha de inicio.
 * @param end Fecha de fin.
 * @return Tipo forward continuamente compuesto para el periodo [start, end].
 */
double FloatingLeg::get_period_rate(const ZeroCouponCurve& curve, const boost::gregorian::date& start, const boost::gregorian::date& end) const {
    const double T1 = curve.get_dcf(start); //Calcula el day count fraction desde today_ (la fecha de valoracion) hasta start. si es el inicio de la pata flotante, T1 = 0 de tal manera que el forward continuamente compuesto es RF_i = ZC_i*T_i.
    const double T2 = curve.get_dcf(end); //Calcula el day count fraction desde today_ (la fecha de valoracion) hasta end.
    const double tau = year_fraction(start, end); //Calcula la fracción de año entre start y end usando la convención de días definida en el constructor. Ya que el flujo sera: Nominal * Rate_Period_futuro * tau (El numero de dias entre los dos flujos). Es decir, Euribor6m al 6% anual, pues es 3%. Es decir, 6% * 0,5(tau). Si eran 100M pues es 100M * 6% * 0,5(tau).

    if (T2 <= T1) return 0.0; //Si la fecha de fin es menor o igual a la fecha de inicio, se salta el periodo.

    const double r1 = curve.get_zero_rate(start);
    const double r2 = curve.get_zero_rate(end);
    // Forward continuamente compuesto (PDF ec. 2.3): RF_i = (ZC_i*T_i - ZC_{i-1}*T_{i-1}) / (T_i - T_{i-1})
    const double forward_cc = (r2 * T2 - r1 * T1) / (T2 - T1);

    // Conversión a semestral (PDF ec. 2.2): R_6m = 2*(e^{Rc/2} - 1). Sustituyo 2 por 1/tau para generalizar a cualquier periodo.
    const double R_m = (1/tau) * (std::exp(forward_cc* (T2-T1)) - 1.0);
    return R_m;
}



/** 
 * @brief Calcula el cupón para el periodo [start, end]: notional * (tipo_periodo + spread) * tau.
 * @param curve Curva de descuento.
 * @param start Fecha de inicio.
 * @param end Fecha de fin.
 * @return Cupón en unidades de notional.
 */
double FloatingLeg::get_period_coupon(const ZeroCouponCurve& curve, const boost::gregorian::date& start, const boost::gregorian::date& end) const {
    const double rate_period = get_period_rate(curve, start, end) + spread_;
    const double tau = year_fraction(start, end);
    return notional_ * rate_period * tau;
}


/** 
 * @brief Calcula el precio de la pata flotante.
 * @note Para ello necesita la curva de descuento para calcular el factor de descuento para cada fecha de pago.
 * @param curve Curva de descuento.
 * @return Precio de la pata flotante.
 */
double FloatingLeg::price(const ZeroCouponCurve& curve) const {
    double pv = 0.0;

    if (schedule_.size() < 2) return 0.0; //tiene que haber al menos 2 fechas

    for (std::size_t i = 1; i < schedule_.size(); ++i) {
        const auto& start = schedule_[i - 1];
        const auto& end   = schedule_[i];

        const double tau = year_fraction(start, end);
        if (tau <= 0.0) continue; //Si la fecha de fin es menor o igual a la fecha de inicio, se salta el periodo.

        const double coupon = get_period_coupon(curve, start, end); //sacamos el cupon del periodo
        const double df = curve.get_discount_factor(end); //Calculamos el factor de descuento para la fecha de pago.

        pv += coupon * df; //Nos traemos el cupon a valor presente
    }

    return pv;
}

