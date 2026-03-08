#include <boost/test/unit_test.hpp>
#include <stdexcept>

#include "Core/instrumentdescription.h"

namespace {

static const std::vector<boost::gregorian::date> kValidSchedule = {
    boost::gregorian::date(2025, 1, 1),
    boost::gregorian::date(2025, 7, 1),
    boost::gregorian::date(2026, 1, 1)
};

/**
 * @brief Crea una pata (leg) válida con schedule explícito.
 */
LegDescription make_valid_leg() {
    LegDescription leg;
    leg.schedule  = kValidSchedule;
    leg.notional  = 1'000'000.0;
    leg.rate      = 0.03;
    leg.day_count = "30/360";
    leg.is_fixed  = true;
    return leg;
}

} // namespace

BOOST_AUTO_TEST_SUITE(InstrumentDescription_validate_tests)

/**
 * Swap válido (payer flotante, receiver fija) no lanza.
 */
BOOST_AUTO_TEST_CASE(valid_swap_does_not_throw) {
    InstrumentDescription desc(InstrumentDescription::Type::swap);
    desc.payer          = make_valid_leg();
    desc.payer.is_fixed = false;
    desc.receiver       = make_valid_leg();
    BOOST_CHECK_NO_THROW(desc.validate());
}

/**
 * Bono válido no lanza.
 */
BOOST_AUTO_TEST_CASE(valid_bond_does_not_throw) {
    InstrumentDescription desc(InstrumentDescription::Type::bond);
    desc.receiver = make_valid_leg();
    BOOST_CHECK_NO_THROW(desc.validate());
}

/**
 * Schedule vacío en la pata payer debe lanzar.
 */
BOOST_AUTO_TEST_CASE(swap_with_empty_payer_schedule_throws) {
    InstrumentDescription desc(InstrumentDescription::Type::swap);
    desc.payer          = make_valid_leg();
    desc.payer.schedule = {};   // vacío → < 2 fechas
    desc.receiver       = make_valid_leg();
    BOOST_CHECK_THROW(desc.validate(), std::invalid_argument);
}

/**
 * Schedule con solo una fecha en la pata receiver debe lanzar.
 */
BOOST_AUTO_TEST_CASE(swap_with_single_date_receiver_schedule_throws) {
    InstrumentDescription desc(InstrumentDescription::Type::swap);
    desc.payer          = make_valid_leg();
    desc.receiver       = make_valid_leg();
    desc.receiver.schedule = { boost::gregorian::date(2025, 1, 1) };
    BOOST_CHECK_THROW(desc.validate(), std::invalid_argument);
}

/**
 * Notional cero en pata payer debe lanzar.
 */
BOOST_AUTO_TEST_CASE(swap_with_zero_notional_throws) {
    InstrumentDescription desc(InstrumentDescription::Type::swap);
    desc.payer          = make_valid_leg();
    desc.payer.notional = 0.0;
    desc.receiver       = make_valid_leg();
    BOOST_CHECK_THROW(desc.validate(), std::invalid_argument);
}

/**
 * Notional negativo en pata payer debe lanzar.
 */
BOOST_AUTO_TEST_CASE(swap_with_negative_notional_throws) {
    InstrumentDescription desc(InstrumentDescription::Type::swap);
    desc.payer          = make_valid_leg();
    desc.payer.notional = -1000.0;
    desc.receiver       = make_valid_leg();
    BOOST_CHECK_THROW(desc.validate(), std::invalid_argument);
}

/**
 * day_count inválido en pata receiver debe lanzar.
 */
BOOST_AUTO_TEST_CASE(swap_with_invalid_day_count_throws) {
    InstrumentDescription desc(InstrumentDescription::Type::swap);
    desc.payer              = make_valid_leg();
    desc.receiver           = make_valid_leg();
    desc.receiver.day_count = "Actual/365";
    BOOST_CHECK_THROW(desc.validate(), std::invalid_argument);
}

/**
 * Schedule con fecha inicial posterior a la final debe lanzar.
 */
BOOST_AUTO_TEST_CASE(swap_with_reversed_schedule_throws) {
    InstrumentDescription desc(InstrumentDescription::Type::swap);
    desc.payer          = make_valid_leg();
    desc.payer.schedule = {
        boost::gregorian::date(2026, 1, 1),  // inicio > fin
        boost::gregorian::date(2025, 1, 1)
    };
    desc.receiver = make_valid_leg();
    BOOST_CHECK_THROW(desc.validate(), std::invalid_argument);
}

/**
 * Bono con day_count Actual/360 no lanza.
 */
BOOST_AUTO_TEST_CASE(valid_day_count_actual_360_accepted) {
    InstrumentDescription desc(InstrumentDescription::Type::bond);
    desc.receiver           = make_valid_leg();
    desc.receiver.day_count = "Actual/360";
    BOOST_CHECK_NO_THROW(desc.validate());
}

/**
 * Bono con rate negativo debe lanzar.
 */
BOOST_AUTO_TEST_CASE(bond_with_negative_rate_throws) {
    InstrumentDescription desc(InstrumentDescription::Type::bond);
    desc.receiver      = make_valid_leg();
    desc.receiver.rate = -0.01;
    BOOST_CHECK_THROW(desc.validate(), std::invalid_argument);
}

/**
 * Bono con schedule vacío en receiver debe lanzar.
 */
BOOST_AUTO_TEST_CASE(bond_with_empty_receiver_schedule_throws) {
    InstrumentDescription desc(InstrumentDescription::Type::bond);
    desc.receiver          = make_valid_leg();
    desc.receiver.schedule = {};
    BOOST_CHECK_THROW(desc.validate(), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()
