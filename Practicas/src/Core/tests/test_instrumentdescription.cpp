#include <boost/test/unit_test.hpp>
#include <stdexcept>

#include "Core/instrumentdescription.h"

namespace {



/** 
 * @brief Funcion auxiliar para crear una pata (leg) valida.
 * @note Se usa para crear una pata (leg) valida para los tests.
 * @return LegDescription pata (leg) valida.
 */
LegDescription make_valid_leg() {
    LegDescription leg;
    leg.start_date = "2025-01-01";
    leg.end_date = "2026-01-01";
    leg.notional = 1'000'000.0;
    leg.rate = 0.03;
    leg.day_count = "30/360";
    leg.is_fixed = true;
    return leg;
}

} // namespace

BOOST_AUTO_TEST_SUITE(InstrumentDescription_validate_tests)


/** 
 * @brief Prueba para verificar que un swap valido no lanza excepciones.
 * @note Se crea un InstrumentDescription con tipo swap, se le asigna una pata payer y receiver validas, y se valida el instrumento completo.
 * @note Se comprueba que no lanza excepciones.
 * @note El resto de tests tendran la misma estructura cambiando algunos atributos de la pata payer o receiver para comprobar que lanza excepciones.
 */
BOOST_AUTO_TEST_CASE(valid_swap_does_not_throw) {
    InstrumentDescription desc(InstrumentDescription::Type::swap);
    desc.payer = make_valid_leg(); // se crea una pata payer valida.
    desc.payer.is_fixed = false;
    desc.receiver = make_valid_leg();
    BOOST_CHECK_NO_THROW(desc.validate()); // valida el instrumento completo, comprobamos que no lanza excepciones.
}



// Prueba para verificar que un bono valido no lanza excepciones.
BOOST_AUTO_TEST_CASE(valid_bond_does_not_throw) {
    InstrumentDescription desc(InstrumentDescription::Type::bond);
    desc.receiver = make_valid_leg();
    BOOST_CHECK_NO_THROW(desc.validate());
}



//Prueba para verificar que un swap con una pata payer con start_date vacia lanza excepciones.
BOOST_AUTO_TEST_CASE(swap_with_empty_payer_start_date_throws) {
    InstrumentDescription desc(InstrumentDescription::Type::swap);
    desc.payer = make_valid_leg(); 
    desc.payer.start_date.clear(); // clear es un metodo de std::string que limpia el string y lo pone en "".
    desc.receiver = make_valid_leg();
    BOOST_CHECK_THROW(desc.validate(), std::invalid_argument);
}




//Prueba para verificar que un swap con una pata receiver con end_date vacia lanza excepciones.
BOOST_AUTO_TEST_CASE(swap_with_empty_receiver_end_date_throws) {
    InstrumentDescription desc(InstrumentDescription::Type::swap);
    desc.payer = make_valid_leg();
    desc.receiver = make_valid_leg();
    desc.receiver.end_date.clear();
    BOOST_CHECK_THROW(desc.validate(), std::invalid_argument);
}



//Prueba para verificar que un swap con una pata payer con notional 0 lanza excepciones.
BOOST_AUTO_TEST_CASE(swap_with_zero_notional_throws) {
    InstrumentDescription desc(InstrumentDescription::Type::swap);
    desc.payer = make_valid_leg();
    desc.payer.notional = 0.0;
    desc.receiver = make_valid_leg();
    BOOST_CHECK_THROW(desc.validate(), std::invalid_argument);
}



//Prueba para verificar que un swap con una pata payer con notional negativa lanza excepciones.
BOOST_AUTO_TEST_CASE(swap_with_negative_notional_throws) {
    InstrumentDescription desc(InstrumentDescription::Type::swap);
    desc.payer = make_valid_leg();
    desc.payer.notional = -1000.0;
    desc.receiver = make_valid_leg();
    BOOST_CHECK_THROW(desc.validate(), std::invalid_argument);
}



//Prueba para verificar que un swap con una pata payer con day_count invalido lanza excepciones.
BOOST_AUTO_TEST_CASE(swap_with_invalid_day_count_throws) {
    InstrumentDescription desc(InstrumentDescription::Type::swap);
    desc.payer = make_valid_leg();
    desc.receiver = make_valid_leg();
    desc.receiver.day_count = "Actual/365";
    BOOST_CHECK_THROW(desc.validate(), std::invalid_argument);
}


//Prueba para verificar que un swap con una pata payer con start_date posterior a end_date lanza excepciones.
BOOST_AUTO_TEST_CASE(swap_with_start_date_after_end_date_throws) {
    InstrumentDescription desc(InstrumentDescription::Type::swap);
    desc.payer = make_valid_leg();
    desc.payer.start_date = "2026-01-01";
    desc.payer.end_date = "2025-01-01";
    desc.receiver = make_valid_leg();
    BOOST_CHECK_THROW(desc.validate(), std::invalid_argument);
}


//Prueba para verificar que un bono con day_count Actual/360 no lanza excepciones.
BOOST_AUTO_TEST_CASE(valid_day_count_actual_360_accepted) {
    InstrumentDescription desc(InstrumentDescription::Type::bond);
    desc.receiver = make_valid_leg();
    desc.receiver.day_count = "Actual/360";
    BOOST_CHECK_NO_THROW(desc.validate());
}



//Prueba para verificar que un bono con rate negativo lanza excepciones.
BOOST_AUTO_TEST_CASE(bond_with_negative_rate_throws) {
    InstrumentDescription desc(InstrumentDescription::Type::bond);
    desc.receiver = make_valid_leg();
    desc.receiver.rate = -0.01;
    BOOST_CHECK_THROW(desc.validate(), std::invalid_argument);
}


//Prueba para verificar que un bono con una pata receiver con start_date vacia lanza excepciones.
BOOST_AUTO_TEST_CASE(bond_with_empty_receiver_start_date_throws) {
    InstrumentDescription desc(InstrumentDescription::Type::bond);
    desc.receiver = make_valid_leg();
    desc.receiver.start_date.clear();
    BOOST_CHECK_THROW(desc.validate(), std::invalid_argument);
}


BOOST_AUTO_TEST_SUITE_END()

