#include "Rates/Swap.h"

Swap::Swap(std::unique_ptr<FixedLeg> fixed_leg,
           std::unique_ptr<FloatingLeg> floating_leg,
           const ZeroCouponCurve& discount_curve,
           bool payer_fixed)
    : fixed_leg_(std::move(fixed_leg)),
      floating_leg_(std::move(floating_leg)),
      discount_curve_(discount_curve),
      payer_fixed_(payer_fixed) {}

double Swap::price() const {
    const double pv_fixed  = fixed_leg_->price(discount_curve_);
    const double pv_float  = floating_leg_->price(discount_curve_);

    if (payer_fixed_) {
        // Valor desde el punto de vista de quien paga fijo y recibe flotante.
        return pv_float - pv_fixed;
    }

    // Punto de vista inverso: recibe fijo y paga flotante.
    return pv_fixed - pv_float;
}

