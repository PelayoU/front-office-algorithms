#include <boost/test/unit_test.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <cmath>

#include "Core/Bootstrapping.h"
#include "Core/Deposit.h"
#include "Core/ParSwapBootstrap.h"

/**
 * Tests de bootstrapping validados con la Tabla 6 del README (Ejemplo Bootstrapping).
 * Contexto: Depósito 6m y tres IRS (12m, 18m, 2y), Actual/360, fecha valoración 01/04/2016.
 * Se comprueba que la curva de DF y los tipos cero continuos coinciden con la tabla.
 */

static const boost::gregorian::date kToday(2016, 4, 1);

BOOST_AUTO_TEST_SUITE(Bootstrapping_README_Table6_tests)

/**
 * Test 1: La curva bootstrappeada tiene DF(today)=1.0 y los DF de la Tabla 6 en cada vencimiento.
 */
BOOST_AUTO_TEST_CASE(bootstrap_curve_discount_factors_match_table6) {
    Bootstrapping::InstrumentsMapType instruments;

    instruments.insert(std::make_pair(
        boost::gregorian::date(2016, 10, 3),
        std::make_shared<Deposit>(boost::gregorian::date(2016, 10, 3), 185.0 / 360.0, 0.05)
    ));

    std::vector<boost::gregorian::date> schedule_12m = {
        boost::gregorian::date(2016, 4, 1),
        boost::gregorian::date(2016, 10, 3),
        boost::gregorian::date(2017, 4, 3)
    };
    instruments.insert(std::make_pair(
        boost::gregorian::date(2017, 4, 3),
        std::make_shared<ParSwapBootstrap>(schedule_12m, 0.055, "Actual/360")
    ));

    std::vector<boost::gregorian::date> schedule_18m = {
        boost::gregorian::date(2016, 4, 1),
        boost::gregorian::date(2016, 10, 3),
        boost::gregorian::date(2017, 4, 3),
        boost::gregorian::date(2017, 10, 2)
    };
    instruments.insert(std::make_pair(
        boost::gregorian::date(2017, 10, 2),
        std::make_shared<ParSwapBootstrap>(schedule_18m, 0.06, "Actual/360")
    ));

    std::vector<boost::gregorian::date> schedule_2y = {
        boost::gregorian::date(2016, 4, 1),
        boost::gregorian::date(2016, 10, 3),
        boost::gregorian::date(2017, 4, 3),
        boost::gregorian::date(2017, 10, 2),
        boost::gregorian::date(2018, 4, 2)
    };
    instruments.insert(std::make_pair(
        boost::gregorian::date(2018, 4, 2),
        std::make_shared<ParSwapBootstrap>(schedule_2y, 0.064, "Actual/360")
    ));

    Bootstrapping bootstrap;
    Bootstrapping::DiscountCurveType curve = bootstrap(instruments, kToday);

    BOOST_TEST(curve.at(kToday) == 1.0, boost::test_tools::tolerance(1e-10));
    BOOST_TEST(curve.at(boost::gregorian::date(2016, 10, 3)) == 0.9749492, boost::test_tools::tolerance(1e-6));
    BOOST_TEST(curve.at(boost::gregorian::date(2017, 4, 3)) == 0.9461363, boost::test_tools::tolerance(1e-6));
    BOOST_TEST(curve.at(boost::gregorian::date(2017, 10, 2)) == 0.9135292, boost::test_tools::tolerance(1e-6));
    BOOST_TEST(curve.at(boost::gregorian::date(2018, 4, 2)) == 0.8793138, boost::test_tools::tolerance(1e-6));
}

/**
 * Test 2: A partir de la curva de DF bootstrappeada, los tipos cero continuos R_c = -ln(DF)/T
 * coinciden con la columna "Tipo ZC Continuo" de la Tabla 6.
 */
BOOST_AUTO_TEST_CASE(bootstrap_implied_zero_rates_continuous_match_table6) {
    Bootstrapping::InstrumentsMapType instruments;
    instruments.insert(std::make_pair(
        boost::gregorian::date(2016, 10, 3),
        std::make_shared<Deposit>(boost::gregorian::date(2016, 10, 3), 185.0 / 360.0, 0.05)
    ));
    std::vector<boost::gregorian::date> s12 = {
        boost::gregorian::date(2016, 4, 1), boost::gregorian::date(2016, 10, 3), boost::gregorian::date(2017, 4, 3)
    };
    instruments.insert(std::make_pair(
        boost::gregorian::date(2017, 4, 3),
        std::make_shared<ParSwapBootstrap>(s12, 0.055, "Actual/360")
    ));
    std::vector<boost::gregorian::date> s18 = {
        boost::gregorian::date(2016, 4, 1), boost::gregorian::date(2016, 10, 3),
        boost::gregorian::date(2017, 4, 3), boost::gregorian::date(2017, 10, 2)
    };
    instruments.insert(std::make_pair(
        boost::gregorian::date(2017, 10, 2),
        std::make_shared<ParSwapBootstrap>(s18, 0.06, "Actual/360")
    ));
    std::vector<boost::gregorian::date> s2y = {
        boost::gregorian::date(2016, 4, 1), boost::gregorian::date(2016, 10, 3),
        boost::gregorian::date(2017, 4, 3), boost::gregorian::date(2017, 10, 2), boost::gregorian::date(2018, 4, 2)
    };
    instruments.insert(std::make_pair(
        boost::gregorian::date(2018, 4, 2),
        std::make_shared<ParSwapBootstrap>(s2y, 0.064, "Actual/360")
    ));

    Bootstrapping bootstrap;
    Bootstrapping::DiscountCurveType curve = bootstrap(instruments, kToday);

    const double T1 = 185.0 / 360.0;
    const double T2 = 367.0 / 360.0;
    const double T3 = 549.0 / 360.0;
    const double T4 = 731.0 / 360.0;

    const double df1 = curve.at(boost::gregorian::date(2016, 10, 3));
    const double df2 = curve.at(boost::gregorian::date(2017, 4, 3));
    const double df3 = curve.at(boost::gregorian::date(2017, 10, 2));
    const double df4 = curve.at(boost::gregorian::date(2018, 4, 2));

    const double rc1 = -std::log(df1) / T1;
    const double rc2 = -std::log(df2) / T2;
    const double rc3 = -std::log(df3) / T3;
    const double rc4 = -std::log(df4) / T4;

    BOOST_TEST(rc1 == 0.0493684, boost::test_tools::tolerance(1e-6));
    BOOST_TEST(rc2 == 0.0543126, boost::test_tools::tolerance(1e-6));
    BOOST_TEST(rc3 == 0.0593049, boost::test_tools::tolerance(1e-6));
    BOOST_TEST(rc4 == 0.0633390, boost::test_tools::tolerance(1e-6));
}

/**
 * Test 3: Deposit aislado devuelve el DF del depósito 6m de la Tabla 6.
 */
BOOST_AUTO_TEST_CASE(deposit_6m_discount_factor_match_table6) {
    Deposit depo(boost::gregorian::date(2016, 10, 3), 185.0 / 360.0, 0.05);
    const double df = depo.computeDiscountFactor(0.0);
    BOOST_TEST(df == 0.9749492, boost::test_tools::tolerance(1e-6));
}

/**
 * Test 4: ParSwapBootstrap (swap 12m) con annuity previa correcta devuelve el DF del nodo 12m.
 */
BOOST_AUTO_TEST_CASE(par_swap_12m_discount_factor_match_table6) {
    std::vector<boost::gregorian::date> s = {
        boost::gregorian::date(2016, 4, 1),
        boost::gregorian::date(2016, 10, 3),
        boost::gregorian::date(2017, 4, 3)
    };
    ParSwapBootstrap swap12m(s, 0.055, "Actual/360");
    const double annuity1 = (185.0 / 360.0) * 0.9749492;
    const double df2 = swap12m.computeDiscountFactor(annuity1);
    BOOST_TEST(df2 == 0.9461363, boost::test_tools::tolerance(1e-6));
}

BOOST_AUTO_TEST_SUITE_END()
