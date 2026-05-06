#ifndef RATES_BONDBUILDER_H
#define RATES_BONDBUILDER_H

#include <memory>

#include "Core/instrumentdescription.h"
#include "Core/ZeroCouponCurve.h"

class Instrument;

/**
 * @brief Builder especializado en la construcción de Bond a partir de InstrumentDescription.
 * Para un bono se usa solo receiver como definición del cupón.
 */
class BondBuilder {
public:
    /**
     * @brief Construye un Bond a partir de la descripción y la curva. Usa receiver como definición del cupón.
     * @param description Descripción con type bond; receiver debe tener schedule, notional, rate y day_count.
     * @param curve Curva de descuento (las fechas del schedule deben coincidir con nodos de la curva).
     * @return Puntero al Bond creado.
     */
    static std::unique_ptr<Instrument> build(const InstrumentDescription& description, const ZeroCouponCurve& curve);

    /**
     * @brief Identificador del tipo que este builder construye (InstrumentDescription::Type::bond).
     */
    static InstrumentDescription::Type getId();
};

#endif
