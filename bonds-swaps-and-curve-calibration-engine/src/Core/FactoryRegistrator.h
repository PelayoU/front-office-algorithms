#ifndef FACTORYREGISTRATOR_H
#define FACTORYREGISTRATOR_H

#include "Core/Factory.h"

/**
 * @brief Registra un builder concreto en la Factory en construcción.
 * Al instanciar FactoryRegistrator<SwapBuilder>, se registra SwapBuilder en el singleton Factory.
 */
template <typename BuilderClass>
class FactoryRegistrator {
public:
    FactoryRegistrator() {
        Factory::instance().register_constructor(
            BuilderClass::getId(),
            [](const InstrumentDescription& d, const ZeroCouponCurve& c) { return BuilderClass::build(d, c); }
        );
    }
};

#endif
