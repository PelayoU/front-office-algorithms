#include "Rates/SwapBuilder.h"
#include "Core/FactoryRegistrator.h"
#include "Core/instrumentdescription.h"
#include "Core/instrument.h"
#include "Rates/FixedLeg.h"
#include "Rates/FloatingLeg.h"
#include "Rates/Swap.h"

namespace {
    FactoryRegistrator<SwapBuilder> registration;
}

InstrumentDescription::Type SwapBuilder::getId() {
    return InstrumentDescription::Type::swap;
}

std::unique_ptr<Instrument> SwapBuilder::build(const InstrumentDescription& description, const ZeroCouponCurve& curve) {
    description.validate();

    const LegDescription& payer = description.payer;
    const LegDescription& receiver = description.receiver;

    // El schedule ya viene explícito en la descripción; no se regenera aquí.
    const std::vector<boost::gregorian::date>& schedule = payer.schedule;

    std::unique_ptr<FixedLeg> fixed_leg;
    std::unique_ptr<FloatingLeg> floating_leg;
    bool payer_fixed;

    if (payer.is_fixed) {
        fixed_leg    = std::make_unique<FixedLeg>(schedule, payer.notional, payer.rate, payer.day_count, false);
        floating_leg = std::make_unique<FloatingLeg>(schedule, receiver.notional, receiver.day_count, 0.0);
        payer_fixed  = true;
    } else {
        fixed_leg    = std::make_unique<FixedLeg>(schedule, receiver.notional, receiver.rate, receiver.day_count, false);
        floating_leg = std::make_unique<FloatingLeg>(schedule, payer.notional, payer.day_count, 0.0);
        payer_fixed  = false;
    }

    return std::make_unique<Swap>(std::move(fixed_leg), std::move(floating_leg), curve, payer_fixed);
}
