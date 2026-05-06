#ifndef LEG_H
#define LEG_H

#include "Core/ZeroCouponCurve.h"

/**
 * @brief Interfaz abstracta para una pata (leg) de un instrumento de tipos de interés.
 *
 * Una leg sabe valorarse (price) dado una curva de descuento. Las implementaciones concretas
 * serán FixedLeg, FloatingLeg, etc.
 */
class Leg {
public:
    virtual ~Leg() = default;

    /// Valor presente de la pata usando la curva de descuento proporcionada.
    virtual double price(const ZeroCouponCurve& curve) const = 0;
};

#endif

