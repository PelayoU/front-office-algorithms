#include <boost/test/unit_test.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "Core/Factory.h"
#include "Core/ZeroCouponCurve.h"
#include "Core/instrumentdescription.h"
#include "Rates/BondBuilder.h"
#include "Rates/ScheduleUtils.h"
#include "Rates/SwapBuilder.h"

/**
 * Tests unitarios para Factory, FactoryRegistrator (vía Factory), SwapBuilder, BondBuilder y ScheduleUtils.
 * Los datos de swap/bono cuando se necesitan coinciden con el README (Tabla 4) y con el ejemplo del bono del PDF.
 *
 * Los builders ya no generan el schedule internamente; el schedule se pasa explícito en LegDescription.
 * Se usa Rates::make_schedule para construirlo en los tests, igual que haría cualquier cliente.
 */

// Schedules reutilizados en varios tests.
// Swap README: 01/04/2016 – 02/04/2018, semestral (6m), Actual/360.
static const std::vector<boost::gregorian::date> kSwapSchedule =
    Rates::make_schedule("2016-04-01", "2018-04-02", 6);

// Bono PDF: 01/04/2014 – 01/04/2016, semestral (6m), 30/360.
static const std::vector<boost::gregorian::date> kBondSchedule =
    Rates::make_schedule("2014-04-01", "2016-04-01", 6);

// ─────────────────────────────────────────────────────────────────────────────

BOOST_AUTO_TEST_SUITE(ScheduleUtils_tests)

/**
 * make_semiannual_schedule: un periodo de 6 meses devuelve exactamente [inicio, fin].
 */
BOOST_AUTO_TEST_CASE(semiannual_schedule_one_period) {
    std::vector<boost::gregorian::date> s = Rates::make_semiannual_schedule("2016-04-01", "2016-10-01");
    BOOST_TEST(s.size() == 2u);
    BOOST_TEST(s[0] == boost::gregorian::date(2016, 4, 1));
    BOOST_TEST(s[1] == boost::gregorian::date(2016, 10, 1));
}

/**
 * make_semiannual_schedule 2 años: primera fecha = inicio, última = fin, >= 5 fechas.
 */
BOOST_AUTO_TEST_CASE(semiannual_schedule_two_years) {
    std::vector<boost::gregorian::date> s = Rates::make_semiannual_schedule("2016-04-01", "2018-04-02");
    BOOST_TEST(s.size() >= 5u);
    BOOST_TEST(s.front() == boost::gregorian::date(2016, 4, 1));
    BOOST_TEST(s.back()  == boost::gregorian::date(2018, 4, 2));
}

/**
 * make_schedule con frecuencia trimestral (3 meses): 1 año → 5 fechas.
 */
BOOST_AUTO_TEST_CASE(quarterly_schedule_one_year) {
    std::vector<boost::gregorian::date> s = Rates::make_schedule("2016-04-01", "2017-04-01", 3);
    BOOST_TEST(s.size() >= 5u);
    BOOST_TEST(s.front() == boost::gregorian::date(2016, 4, 1));
    BOOST_TEST(s.back()  == boost::gregorian::date(2017, 4, 1));
    BOOST_TEST(s[1] == boost::gregorian::date(2016, 7, 1));
    BOOST_TEST(s[2] == boost::gregorian::date(2016, 10, 1));
    BOOST_TEST(s[3] == boost::gregorian::date(2017, 1, 1));
}

BOOST_AUTO_TEST_SUITE_END()

// ─────────────────────────────────────────────────────────────────────────────

BOOST_AUTO_TEST_SUITE(SwapBuilder_tests)

/**
 * SwapBuilder::getId() debe devolver InstrumentDescription::Type::swap.
 */
BOOST_AUTO_TEST_CASE(swap_builder_get_id_returns_swap) {
    BOOST_TEST(static_cast<int>(SwapBuilder::getId()) == static_cast<int>(InstrumentDescription::Type::swap));
}

/**
 * SwapBuilder::build con schedule explícito (datos README) devuelve instrumento no nulo y precio finito.
 * La curva tiene nodos en las fechas del schedule para que el descuento funcione.
 */
BOOST_AUTO_TEST_CASE(swap_builder_build_returns_valid_swap) {
    const boost::gregorian::date today(2016, 4, 1);
    std::vector<ZeroCouponCurve::Node> nodes = {
        {today,                                  0.0},
        {boost::gregorian::date(2016, 10, 1), 0.0474},
        {boost::gregorian::date(2017, 4, 1),  0.05},
        {boost::gregorian::date(2017, 10, 1), 0.051},
        {boost::gregorian::date(2018, 4, 1),  0.052},
        {boost::gregorian::date(2018, 4, 2),  0.052}
    };
    ZeroCouponCurve curve(today, nodes, "Actual/360");

    InstrumentDescription desc(InstrumentDescription::Type::swap);
    desc.payer.schedule  = kSwapSchedule;
    desc.payer.notional  = 100e6;
    desc.payer.rate      = 0.05;
    desc.payer.day_count = "Actual/360";
    desc.payer.is_fixed  = true;
    desc.receiver.schedule  = kSwapSchedule;
    desc.receiver.notional  = 100e6;
    desc.receiver.day_count = "Actual/360";
    desc.receiver.is_fixed  = false;

    std::unique_ptr<Instrument> inst = SwapBuilder::build(desc, curve);
    BOOST_TEST(inst != nullptr);
    BOOST_TEST(std::isfinite(inst->price()));
}

BOOST_AUTO_TEST_SUITE_END()

// ─────────────────────────────────────────────────────────────────────────────

BOOST_AUTO_TEST_SUITE(BondBuilder_tests)

/**
 * BondBuilder::getId() debe devolver InstrumentDescription::Type::bond.
 */
BOOST_AUTO_TEST_CASE(bond_builder_get_id_returns_bond) {
    BOOST_TEST(static_cast<int>(BondBuilder::getId()) == static_cast<int>(InstrumentDescription::Type::bond));
}

/**
 * BondBuilder::build con schedule explícito (bono PDF 2 años, 6%, nominal 100) devuelve precio ≈ 98.39.
 */
BOOST_AUTO_TEST_CASE(bond_builder_build_returns_valid_bond_pv_matches_pdf) {
    const boost::gregorian::date today(2014, 4, 1);
    std::vector<ZeroCouponCurve::Node> nodes = {
        {boost::gregorian::date(2014, 10, 1), 0.05},
        {boost::gregorian::date(2015, 4, 1),  0.058},
        {boost::gregorian::date(2015, 10, 1), 0.064},
        {boost::gregorian::date(2016, 4, 1),  0.068}
    };
    ZeroCouponCurve curve(today, nodes, "30/360");

    InstrumentDescription desc(InstrumentDescription::Type::bond);
    desc.receiver.schedule  = kBondSchedule;
    desc.receiver.notional  = 100.0;
    desc.receiver.rate      = 0.06;
    desc.receiver.day_count = "30/360";

    std::unique_ptr<Instrument> inst = BondBuilder::build(desc, curve);
    BOOST_TEST(inst != nullptr);
    BOOST_TEST(inst->price() == 98.3850627729396, boost::test_tools::tolerance(1e-10));
}

BOOST_AUTO_TEST_SUITE_END()

// ─────────────────────────────────────────────────────────────────────────────

BOOST_AUTO_TEST_SUITE(Factory_tests)

/**
 * Factory::instance() devuelve siempre la misma referencia (singleton).
 */
BOOST_AUTO_TEST_CASE(factory_singleton_same_reference) {
    Factory& a = Factory::instance();
    Factory& b = Factory::instance();
    BOOST_TEST(&a == &b);
}

/**
 * Factory construye un swap con schedule explícito (datos README) y precio finito.
 */
BOOST_AUTO_TEST_CASE(factory_builds_swap_with_readme_like_data) {
    const boost::gregorian::date today(2016, 4, 1);
    std::vector<ZeroCouponCurve::Node> nodes = {
        {today,                                  0.0},
        {boost::gregorian::date(2016, 10, 1), 0.0474},
        {boost::gregorian::date(2017, 4, 1),  0.05},
        {boost::gregorian::date(2017, 10, 1), 0.051},
        {boost::gregorian::date(2018, 4, 1),  0.052},
        {boost::gregorian::date(2018, 4, 2),  0.052}
    };
    ZeroCouponCurve curve(today, nodes, "Actual/360");

    InstrumentDescription desc(InstrumentDescription::Type::swap);
    desc.payer.schedule  = kSwapSchedule;
    desc.payer.notional  = 100e6;
    desc.payer.rate      = 0.05;
    desc.payer.day_count = "Actual/360";
    desc.payer.is_fixed  = true;
    desc.receiver.schedule  = kSwapSchedule;
    desc.receiver.notional  = 100e6;
    desc.receiver.day_count = "Actual/360";
    desc.receiver.is_fixed  = false;

    std::unique_ptr<Instrument> inst = Factory::instance()(desc, curve);
    BOOST_TEST(inst != nullptr);
    BOOST_TEST(std::isfinite(inst->price()));
}

/**
 * Factory construye un bono con schedule explícito (ejemplo PDF) y precio ≈ 98.39.
 */
BOOST_AUTO_TEST_CASE(factory_builds_bond_with_pdf_like_data) {
    const boost::gregorian::date today(2014, 4, 1);
    std::vector<ZeroCouponCurve::Node> nodes = {
        {boost::gregorian::date(2014, 10, 1), 0.05},
        {boost::gregorian::date(2015, 4, 1),  0.058},
        {boost::gregorian::date(2015, 10, 1), 0.064},
        {boost::gregorian::date(2016, 4, 1),  0.068}
    };
    ZeroCouponCurve curve(today, nodes, "30/360");

    InstrumentDescription desc(InstrumentDescription::Type::bond);
    desc.receiver.schedule  = kBondSchedule;
    desc.receiver.notional  = 100.0;
    desc.receiver.rate      = 0.06;
    desc.receiver.day_count = "30/360";

    std::unique_ptr<Instrument> inst = Factory::instance()(desc, curve);
    BOOST_TEST(inst != nullptr);
    BOOST_TEST(inst->price() == 98.3850627729396, boost::test_tools::tolerance(1e-10));
}

BOOST_AUTO_TEST_SUITE_END()

// ─────────────────────────────────────────────────────────────────────────────

BOOST_AUTO_TEST_SUITE(FactoryRegistrator_tests)

/**
 * Los builders están registrados al arrancar; Factory puede construir swap y bono sin errores.
 */
BOOST_AUTO_TEST_CASE(registrator_effect_swap_and_bond_buildable) {
    const boost::gregorian::date today(2016, 4, 1);
    std::vector<ZeroCouponCurve::Node> nodes = {
        {today,                                  0.0},
        {boost::gregorian::date(2016, 10, 1), 0.0474},
        {boost::gregorian::date(2017, 4, 1),  0.05},
        {boost::gregorian::date(2017, 10, 1), 0.051},
        {boost::gregorian::date(2018, 4, 1),  0.052},
        {boost::gregorian::date(2018, 4, 2),  0.052}
    };
    ZeroCouponCurve curve(today, nodes, "Actual/360");

    InstrumentDescription desc_swap(InstrumentDescription::Type::swap);
    desc_swap.payer.schedule      = kSwapSchedule;
    desc_swap.payer.notional      = 1e6;
    desc_swap.payer.rate          = 0.05;
    desc_swap.payer.day_count     = "Actual/360";
    desc_swap.payer.is_fixed      = true;
    desc_swap.receiver.schedule   = kSwapSchedule;
    desc_swap.receiver.notional   = 1e6;
    desc_swap.receiver.day_count  = "Actual/360";
    desc_swap.receiver.is_fixed   = false;

    // Bono: usa la misma curva pero schedule y convención del bono PDF
    const boost::gregorian::date today_bond(2014, 4, 1);
    std::vector<ZeroCouponCurve::Node> bond_nodes = {
        {boost::gregorian::date(2014, 10, 1), 0.05},
        {boost::gregorian::date(2015, 4, 1),  0.058},
        {boost::gregorian::date(2015, 10, 1), 0.064},
        {boost::gregorian::date(2016, 4, 1),  0.068}
    };
    ZeroCouponCurve bond_curve(today_bond, bond_nodes, "30/360");

    InstrumentDescription desc_bond(InstrumentDescription::Type::bond);
    desc_bond.receiver.schedule  = kBondSchedule;
    desc_bond.receiver.notional  = 100.0;
    desc_bond.receiver.rate      = 0.05;
    desc_bond.receiver.day_count = "30/360";

    std::unique_ptr<Instrument> swap_inst = Factory::instance()(desc_swap, curve);
    std::unique_ptr<Instrument> bond_inst = Factory::instance()(desc_bond, bond_curve);
    BOOST_TEST(swap_inst != nullptr);
    BOOST_TEST(bond_inst != nullptr);
    BOOST_TEST(std::isfinite(swap_inst->price()));
    BOOST_TEST(std::isfinite(bond_inst->price()));
}

BOOST_AUTO_TEST_SUITE_END()
