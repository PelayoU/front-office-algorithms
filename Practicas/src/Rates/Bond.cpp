#include "Rates/Bond.h"



Bond::Bond(std::unique_ptr<FixedLeg> coupons,const ZeroCouponCurve& discount_curve)
    : coupons_(std::move(coupons)),discount_curve_(discount_curve) {}


/**   
 * @brief Calcula el precio del bono.
 * @note Delega el cálculo del precio de la pata fija en la FixedLeg. (coupons_ es un objeto de la clase FixedLeg que tiene el metodo price())
 * @return Precio del bono.
 */
double Bond::price() const {
    return coupons_->price(discount_curve_);
}

