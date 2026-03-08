#ifndef BOND_H
#define BOND_H

#include <memory>

#include "Core/instrument.h"
#include "Rates/FixedLeg.h"

/**
 * @brief Bono simple construido a partir de una FixedLeg.
 *
 * El bono delega toda la lógica de cálculo de flujos en la FixedLeg y simplemente
 * llama a price() con la curva de descuento proporcionada en el constructor.
 */
class Bond : public Instrument {

    private:
    std::unique_ptr<FixedLeg> coupons_; //Puntero inteligente a la pata fija
    const ZeroCouponCurve& discount_curve_;

public:
    /** 
    * @brief Constructor para inicializar el bono.
    * @param coupons Puntero inteligente a la pata fija.
    * @param discount_curve Curva de tipos cero cupon.
    * @note Con punteros inteligentes estoy obligado a usar std::move() para transferir la propiedad de la pata fija al bono.
    */
    Bond(std::unique_ptr<FixedLeg> coupons,const ZeroCouponCurve& discount_curve); //Con punteros inteligentes estoy obligado a usar std::move() para transferir la propiedad de la pata fija al bono.

    double price() const override;


};

#endif

