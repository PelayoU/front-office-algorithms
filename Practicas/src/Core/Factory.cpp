#include "Core/Factory.h"
#include <stdexcept>

Factory::Factory() = default;

Factory::~Factory() = default;

InstrumentDescription::Type Factory::getBuilderId(const InstrumentDescription& description) const {
    return description.type;
}

void Factory::register_constructor(const InstrumentDescription::Type& id, const Builder& builder) {
    buildersMap_.insert(std::make_pair(id, builder));
}

std::unique_ptr<Instrument> Factory::operator()(const InstrumentDescription& description, const ZeroCouponCurve& curve) const {
    auto builder = buildersMap_.find(getBuilderId(description));
    if (builder == buildersMap_.end()) {
        throw std::runtime_error("invalid payoff descriptor");
    }
    return (builder->second)(description, curve);
}
