#include <boost/test/unit_test.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <stdexcept>

#include "Core/ZeroCouponCurve.h"

BOOST_AUTO_TEST_SUITE(ZeroCouponCurve_tests)




/**
 * @brief Test para verificar que el metodo get_dcf() devuelve la fracción de año entre la fecha de valoracion (today_) y la fecha de vencimiento (boost::gregorian::date(2014, 10, 1)) correctamente..
 */
BOOST_AUTO_TEST_CASE(get_dcf_6_months_is_half_year) {
    
    const boost::gregorian::date today(2014, 4, 1);


    std::vector<ZeroCouponCurve::Node> nodes = {
        {boost::gregorian::date(2014, 10, 1), 0.05}
    };


    ZeroCouponCurve curve(today, nodes, "30/360");


    const double dcf = curve.get_dcf(boost::gregorian::date(2014, 10, 1));
    

    BOOST_TEST(dcf == 0.5, boost::test_tools::tolerance(1e-15));
}





/**
 * @brief Test para verificar que el metodo get_zero_rate() devuelve el tipo cero cupón en la fecha exacta correctamente.
 */
BOOST_AUTO_TEST_CASE(get_zero_rate_exact_match) {

    const boost::gregorian::date today(2014, 4, 1);
    
    std::vector<ZeroCouponCurve::Node> nodes = {
        {boost::gregorian::date(2014, 10, 1), 0.05},
        {boost::gregorian::date(2016, 4, 1), 0.068}
    };
    
    ZeroCouponCurve curve(today, nodes, "30/360");
    
    BOOST_TEST(curve.get_zero_rate(boost::gregorian::date(2014, 10, 1)) == 0.05, boost::test_tools::tolerance(1e-15));
    BOOST_TEST(curve.get_zero_rate(boost::gregorian::date(2016, 4, 1)) == 0.068, boost::test_tools::tolerance(1e-15));

}






/**
 * @brief Test para verificar que el metodo get_zero_rate() lanza una excepción si la fecha de vencimiento no existe en la curva.
 */
BOOST_AUTO_TEST_CASE(get_zero_rate_throws_if_date_not_in_nodes) {
    const boost::gregorian::date today(2014, 4, 1);
    std::vector<ZeroCouponCurve::Node> nodes = {
        {boost::gregorian::date(2014, 10, 1), 0.05},
        {boost::gregorian::date(2016, 4, 1), 0.068}
    };
    ZeroCouponCurve curve(today, nodes, "30/360");
    BOOST_CHECK_THROW(curve.get_zero_rate(boost::gregorian::date(2015, 4, 1)), std::invalid_argument);
}








/**
 * @brief Test para verificar que el metodo get_discount_factor() devuelve el factor de descuento DF(0,T) = exp(-r*T) con capitalización continua correctamente.
 */
BOOST_AUTO_TEST_CASE(get_discount_factor_continuous_compounding) {
    const boost::gregorian::date today(2014, 4, 1);
    std::vector<ZeroCouponCurve::Node> nodes = {
        {boost::gregorian::date(2014, 10, 1), 0.05}
    };
    ZeroCouponCurve curve(today, nodes, "30/360");
    const double zc = curve.get_discount_factor(boost::gregorian::date(2014, 10, 1));

    BOOST_TEST(zc == std::exp(-0.05 * 0.5), boost::test_tools::tolerance(1e-15));
}





/**
 * @brief Test para verificar que el metodo get_dcf() devuelve la fracción de año entre la fecha de valoracion (today_) y la fecha de vencimiento (boost::gregorian::date(2014, 10, 1)) correctamente.
 */
BOOST_AUTO_TEST_CASE(curve_with_actual_360) {
    const boost::gregorian::date today(2014, 4, 1);
    std::vector<ZeroCouponCurve::Node> nodes = {
        {boost::gregorian::date(2014, 10, 1), 0.05}
    };
    ZeroCouponCurve curve(today, nodes, "Actual/360");
    const double dcf = curve.get_dcf(boost::gregorian::date(2014, 10, 1));

    BOOST_TEST(dcf == 183.0 / 360.0, boost::test_tools::tolerance(1e-15));
}




/**
 * @brief Test para verificar que el constructor lanza una excepción si la convención de días no es válida.
 */
BOOST_AUTO_TEST_CASE(invalid_day_count_throws) {
    const boost::gregorian::date today(2014, 4, 1);

    //Si la convención de días no es válida, lanzamos una excepción.
    std::vector<ZeroCouponCurve::Node> nodes = {
        {boost::gregorian::date(2014, 10, 1), 0.05}
    };

    BOOST_CHECK_THROW(ZeroCouponCurve(today, nodes, "Actual/365"), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()

