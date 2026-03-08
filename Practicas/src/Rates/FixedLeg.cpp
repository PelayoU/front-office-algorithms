#include "Rates/FixedLeg.h"

#include <stdexcept>

#include "Core/actual_360.h"
#include "Core/thirty_360.h"



FixedLeg::FixedLeg(const std::vector<boost::gregorian::date>& schedule, double notional, double rate, const std::string& day_count, bool include_notional)
    : schedule_(schedule), notional_(notional), rate_(rate), day_count_(day_count), include_notional_(include_notional) {}








double FixedLeg::year_fraction(const boost::gregorian::date& from, const boost::gregorian::date& to) const {
    if (day_count_ == "Actual/360") {
        Actual_360 conv;
        return conv(from, to);
    }
    if (day_count_ == "30/360") {
        Thirty_360 conv;
        return conv(from, to);
    }

    throw std::invalid_argument("FixedLeg: day_count desconocido");
}





/**
 * @brief Calcula el precio de la pata fija. Por simplicidad asumiremos que la fecha de pago coincide con la fecha de final de periodo.
 * @note Para ello necesita la curva de descuento para calcular el factor de descuento para cada fecha de pago.
 * @param curve Curva de descuento.
 * @return Precio de la pata fija.
 */
double FixedLeg::price(const ZeroCouponCurve& curve) const {
    double pv = 0.0; //Valor presente del bono (Sera la suma de los valores presentes de los cupones y del principal).

    //Si el vector de fechas de pago tiene menos de 2 fechas, el precio es 0.
    if (schedule_.size() < 2) {
        return 0.0;
    }


    //Recorremos el vector de fechas de pago y calculamos el precio de cada pago.
    for (std::size_t i = 1; i < schedule_.size(); ++i) {
        const auto& start = schedule_[i - 1];
        const auto& end   = schedule_[i];

        const double tau = year_fraction(start, end); //Calcula la fracción de año entre dos fechas usando la convención de días definida en el constructor.
        const double coupon = notional_ * rate_ * tau; //Calculamos el cupon de interes que recibimos en cada pago.
        const double df = curve.get_discount_factor(end); //Calculamos el factor de descuento para la fecha de pago.

        pv += coupon * df; //Pasamos a valor presente el cupon multiplicando por el factor de descuento y lo sumamos al valor presente del bono total.
    }

    //Si se incluye el notional, se añade al valor presente del bono.
    if (include_notional_) {
        const auto& maturity = schedule_.back();
        pv += notional_ * curve.get_discount_factor(maturity);
    }

    return pv;
}

