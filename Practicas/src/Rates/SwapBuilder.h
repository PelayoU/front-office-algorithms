#ifndef RATES_SWAPBUILDER_H
#define RATES_SWAPBUILDER_H

#include <memory>

#include "Core/instrumentdescription.h"
#include "Core/ZeroCouponCurve.h"

class Instrument;

/**
 * @brief Builder especializado en la construcción de Swap a partir de InstrumentDescription.
 */
class SwapBuilder {
public:
    /**
     * @brief Construye un Swap a partir de la descripción y la curva. Usa payer y receiver (schedule común, notional, rate, day_count, is_fixed).
     * @param description Descripción con type swap; payer y receiver deben tener el mismo schedule y datos coherentes.
     * @param curve Curva de descuento (las fechas del schedule deben coincidir con nodos de la curva).
     * @return Puntero al Swap creado.
     */
    static std::unique_ptr<Instrument> build(const InstrumentDescription& description, const ZeroCouponCurve& curve);

    /**
     * @brief Identificador del tipo que este builder construye (InstrumentDescription::Type::swap).
     */
    static InstrumentDescription::Type getId();
};

#endif
