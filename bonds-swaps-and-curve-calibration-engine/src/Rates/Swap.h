#ifndef SWAP_H
#define SWAP_H

#include <memory>

#include "Core/instrument.h"
#include "Rates/FixedLeg.h"
#include "Rates/FloatingLeg.h"

/**
 * @brief Swap de tipos de interés plain-vanilla.
 *
 * Contiene una pata fija y una flotante. El signo se controla con payer_fixed:
 *  - payer_fixed = true  -> pagas fijo y recibes flotante.
 *  - payer_fixed = false -> recibes fijo y pagas flotante.
 */
class Swap : public Instrument {
public:
    /**
     * @brief Constructor del swap.
     * @param fixed_leg Pata fija (cupones fijos).
     * @param floating_leg Pata flotante (tipos implícitos en la curva).
     * @param discount_curve Curva de descuento para valorar ambas patas.
     * @param payer_fixed true si quien paga es la pata fija (recibe flotante), false en caso contrario.
     */
    Swap(std::unique_ptr<FixedLeg> fixed_leg,
         std::unique_ptr<FloatingLeg> floating_leg,
         const ZeroCouponCurve& discount_curve,
         bool payer_fixed);

    /**
     * @brief Valor presente del swap: diferencia entre el PV de la pata que se recibe y el PV de la pata que se paga.
     * @return NPV del swap (en unidades de notional).
     */
    double price() const override;

private:
    std::unique_ptr<FixedLeg> fixed_leg_;
    std::unique_ptr<FloatingLeg> floating_leg_;
    const ZeroCouponCurve& discount_curve_;
    bool payer_fixed_;
};

#endif

