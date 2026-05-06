#include "Rates/BondBuilder.h"
#include "Core/FactoryRegistrator.h"
#include "Core/instrumentdescription.h"
#include "Core/instrument.h"
#include "Rates/Bond.h"
#include "Rates/FixedLeg.h"

namespace {
    FactoryRegistrator<BondBuilder> registration;
}

InstrumentDescription::Type BondBuilder::getId() {
    return InstrumentDescription::Type::bond;
}

std::unique_ptr<Instrument> BondBuilder::build(const InstrumentDescription& description, const ZeroCouponCurve& curve) {
    description.validate();

    const LegDescription& receiver = description.receiver;

    // El schedule ya viene explícito en la descripción; no se regenera aquí.
    const std::vector<boost::gregorian::date>& schedule = receiver.schedule;

    auto fixed_leg = std::make_unique<FixedLeg>(schedule, receiver.notional, receiver.rate, receiver.day_count, true);
    return std::make_unique<Bond>(std::move(fixed_leg), curve);
}
