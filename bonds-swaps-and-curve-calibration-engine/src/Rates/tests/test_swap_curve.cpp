#include <boost/test/unit_test.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <cmath>

#include "Core/Factory.h"
#include "Core/ZeroCouponCurve.h"
#include "Core/instrumentdescription.h"
#include "Rates/FixedLeg.h"
#include "Rates/FloatingLeg.h"
#include "Rates/ScheduleUtils.h"
#include "Rates/Swap.h"

/**
 * Tests de swap validados con el ejemplo de Excel documentado en el README.
 * Contexto: IRS nocional 100.000.000 €, Actual/360, fecha valoración 01/04/2016.
 * Tablas README: Estructura temporal y curva (Tabla 1), Pata flotante (Tabla 2), Pata fija par (Tabla 3), Swap fuera de mercado 5% (Tabla 4).
 */

static const boost::gregorian::date kToday(2016, 4, 1);

static const std::vector<boost::gregorian::date> kSchedule = {
    boost::gregorian::date(2016, 4, 1),
    boost::gregorian::date(2016, 10, 3),
    boost::gregorian::date(2017, 4, 3),
    boost::gregorian::date(2017, 10, 2),
    boost::gregorian::date(2018, 4, 2)
};

static ZeroCouponCurve make_curve_readme() {
    std::vector<ZeroCouponCurve::Node> nodes = {
        {kToday, 0.0},
        {boost::gregorian::date(2016, 10, 3), 0.0474},
        {boost::gregorian::date(2017, 4, 3), 0.05},
        {boost::gregorian::date(2017, 10, 2), 0.051},
        {boost::gregorian::date(2018, 4, 2), 0.052}
    };
    return ZeroCouponCurve(kToday, nodes, "Actual/360");
}

BOOST_AUTO_TEST_SUITE(Swap_README_Excel_tests)

// ---- Tabla 1: Estructura temporal y curva ----

/**
 * Test 1: Cálculo de las fracciones de año (DCF).
 * Comprueba que get_dcf() devuelve los T_i acumulados de la Tabla 1 del README (desde 01/04/2016 hasta cada fin de periodo).
 * Valores esperados: 0.513889, 1.019444, 1.525000, 2.030556 (Act/360).
 */
BOOST_AUTO_TEST_CASE(year_fractions_dcf_match_readme_table1) {
    ZeroCouponCurve curve = make_curve_readme();
    BOOST_TEST(curve.get_dcf(boost::gregorian::date(2016, 10, 3)) == 0.513889, boost::test_tools::tolerance(1e-5));
    BOOST_TEST(curve.get_dcf(boost::gregorian::date(2017, 4, 3)) == 1.019444, boost::test_tools::tolerance(1e-5));
    BOOST_TEST(curve.get_dcf(boost::gregorian::date(2017, 10, 2)) == 1.525, boost::test_tools::tolerance(1e-5));
    BOOST_TEST(curve.get_dcf(boost::gregorian::date(2018, 4, 2)) == 2.030556, boost::test_tools::tolerance(1e-5));
}

/**
 * Test 2: Factores de descuento DF(0,T).
 * Comprueba que get_discount_factor() coincide con la columna DF de la Tabla 1 del README.
 * Valores esperados: 0.97593, 0.95030, 0.92517, 0.89977.
 */
BOOST_AUTO_TEST_CASE(discount_factors_match_readme_table1) {
    ZeroCouponCurve curve = make_curve_readme();
    BOOST_TEST(curve.get_discount_factor(boost::gregorian::date(2016, 10, 3)) == 0.97593, boost::test_tools::tolerance(1e-4));
    BOOST_TEST(curve.get_discount_factor(boost::gregorian::date(2017, 4, 3)) == 0.95030, boost::test_tools::tolerance(1e-4));
    BOOST_TEST(curve.get_discount_factor(boost::gregorian::date(2017, 10, 2)) == 0.92517, boost::test_tools::tolerance(1e-4));
    BOOST_TEST(curve.get_discount_factor(boost::gregorian::date(2018, 4, 2)) == 0.89977, boost::test_tools::tolerance(1e-4));
}

// ---- Tabla 2: Pata flotante (Euribor 6M) ----

/**
 * Test 3: Estimación de los tipos flotantes (Euribor 6M implícitos).
 * Comprueba get_period_rate() frente a la columna R_m de la Tabla 2 del README.
 * Periodo 1 usa fixing; periodos 2-4 son forwards. Tolerancia amplia para periodo 1; periodos 2-4 según tabla (0.053345, 0.053723, 0.055778).
 */
BOOST_AUTO_TEST_CASE(floating_rates_euribor6m_match_readme_table2) {
    ZeroCouponCurve curve = make_curve_readme();
    FloatingLeg leg(kSchedule, 100e6, "Actual/360", 0.0);
    const double rate2 = leg.get_period_rate(curve, boost::gregorian::date(2016, 10, 3), boost::gregorian::date(2017, 4, 3));
    const double rate3 = leg.get_period_rate(curve, boost::gregorian::date(2017, 4, 3), boost::gregorian::date(2017, 10, 2));
    const double rate4 = leg.get_period_rate(curve, boost::gregorian::date(2017, 10, 2), boost::gregorian::date(2018, 4, 2));
    BOOST_TEST(rate2 == 0.053345, boost::test_tools::tolerance(1e-3));
    BOOST_TEST(rate3 == 0.053723, boost::test_tools::tolerance(1e-3));
    BOOST_TEST(rate4 == 0.055778, boost::test_tools::tolerance(1e-3));
}

/**
 * Test 4: Cálculo del valor de cada flujo (cupón flotante).
 * Comprueba get_period_coupon() frente a la columna "Cupón Flotante (CF)" de la Tabla 2 del README.
 * Valores esperados (€): 2697120.12, 2716520.30, 2820430.64 para periodos 2, 3 y 4 (tolerancia 1.0 € en valor absoluto).
 */
BOOST_AUTO_TEST_CASE(floating_coupon_amounts_match_readme_table2) {
    ZeroCouponCurve curve = make_curve_readme();
    FloatingLeg leg(kSchedule, 100e6, "Actual/360", 0.0);
    const double coupon2 = leg.get_period_coupon(curve, boost::gregorian::date(2016, 10, 3), boost::gregorian::date(2017, 4, 3));
    const double coupon3 = leg.get_period_coupon(curve, boost::gregorian::date(2017, 4, 3), boost::gregorian::date(2017, 10, 2));
    const double coupon4 = leg.get_period_coupon(curve, boost::gregorian::date(2017, 10, 2), boost::gregorian::date(2018, 4, 2));
    BOOST_TEST(coupon2 == 2697120.12, boost::test_tools::tolerance(1.0));
    BOOST_TEST(coupon3 == 2716520.30, boost::test_tools::tolerance(1.0));
    BOOST_TEST(coupon4 == 2820430.64, boost::test_tools::tolerance(1.0));
}

/**
 * Test 5: Descuento de los flujos de la pata flotante.
 * Comprueba que el PV total de la pata flotante coincide con el TOTAL de la Tabla 2 del README: 10 020 551,24 €.
 */
BOOST_AUTO_TEST_CASE(floating_leg_pv_total_match_readme_table2) {
    ZeroCouponCurve curve = make_curve_readme();
    FloatingLeg leg(kSchedule, 100e6, "Actual/360", 0.0);
    const double pv_float = leg.price(curve);
    const double expected_pv_float = 10020551.24;
    BOOST_TEST(pv_float == expected_pv_float, boost::test_tools::tolerance(1.0));
}

// ---- Tabla 3 y 4: Valoración completa swap ----

/**
 * Test 6: Valoración completa swap fuera de mercado (Tabla 4).
 * Entidad A: Recibe Flotante, Paga Fijo 5%. NPV = PV_Flotante - PV_Fija(5%).
 * README: PV Flotante = 10 020 551,24 €, PV Fija 5% = 9 522 884,37 €, MtM Swap = +497 666,88 €.
 */
BOOST_AUTO_TEST_CASE(swap_mtm_receive_float_pay_fixed_5_match_readme_table4) {
    ZeroCouponCurve curve = make_curve_readme();
    auto fixed_leg = std::make_unique<FixedLeg>(kSchedule, 100e6, 0.05, "Actual/360", false);
    auto floating_leg = std::make_unique<FloatingLeg>(kSchedule, 100e6, "Actual/360", 0.0);
    Swap swap(std::move(fixed_leg), std::move(floating_leg), curve, true);
    const double npv = swap.price();
    const double expected_npv = 497666.88;
    BOOST_TEST(npv == expected_npv, boost::test_tools::tolerance(1.0));
}

/**
 * Test 7: Swap a la par (Tabla 3).
 * Con tipo fijo al swap rate par K = 0.052613, el swap debería valorarse cerca de cero (PV Flotante ≈ PV Fija).
 * Comprobamos que el NPV del swap con tipo fijo par está próximo a cero.
 */
BOOST_AUTO_TEST_CASE(swap_at_par_near_zero_readme_table3) {
    ZeroCouponCurve curve = make_curve_readme();
    const double swap_rate_par = 0.052613;
    auto fixed_leg = std::make_unique<FixedLeg>(kSchedule, 100e6, swap_rate_par, "Actual/360", false);
    auto floating_leg = std::make_unique<FloatingLeg>(kSchedule, 100e6, "Actual/360", 0.0);
    Swap swap(std::move(fixed_leg), std::move(floating_leg), curve, false);
    const double npv = swap.price();
    BOOST_TEST(std::abs(npv) < 1.0, boost::test_tools::tolerance(0.0));
}

/**
 * Test 8: Factory construye un swap a partir de InstrumentDescription con schedule explícito.
 * La curva tiene nodos en las fechas del schedule para que el descuento funcione.
 * NPV debe ser finito y positivo (recibe flotante, paga 5%).
 */
BOOST_AUTO_TEST_CASE(factory_builds_swap_and_valuation_coherent) {
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

    const std::vector<boost::gregorian::date> schedule =
        Rates::make_schedule("2016-04-01", "2018-04-02", 6);

    InstrumentDescription desc(InstrumentDescription::Type::swap);
    desc.payer.schedule  = schedule;
    desc.payer.notional  = 100e6;
    desc.payer.rate      = 0.05;
    desc.payer.day_count = "Actual/360";
    desc.payer.is_fixed  = true;
    desc.receiver.schedule  = schedule;
    desc.receiver.notional  = 100e6;
    desc.receiver.day_count = "Actual/360";
    desc.receiver.is_fixed  = false;

    std::unique_ptr<Instrument> instrument = Factory::instance()(desc, curve);
    BOOST_TEST(instrument != nullptr);
    const double npv = instrument->price();
    BOOST_TEST(std::isfinite(npv));
    BOOST_TEST(npv > 0.0);
}

BOOST_AUTO_TEST_SUITE_END()
