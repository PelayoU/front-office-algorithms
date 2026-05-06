#define BOOST_TEST_MODULE test_daycount
#include <boost/test/unit_test.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <exception>

#include "Core/daycountcalculator.h"
#include "Core/actual_360.h"
#include "Core/thirty_360.h"



/**
 * @brief Suite de pruebas para la clase DayCountCalculator.
 */
BOOST_AUTO_TEST_SUITE(DayCountCalculator_tests)

    /**
    * @brief Prueba para verificar que la funcion make_date() de la clase DayCountCalculator devuelve un objeto boost::gregorian::date correcto para una fecha en formato string valido.
    */
    BOOST_AUTO_TEST_CASE(make_date_valid_format) {
        auto d = DayCountCalculator::make_date("2026-01-21");
        BOOST_TEST(d.year() == 2026);
        BOOST_TEST(d.month() == 1);
        BOOST_TEST(d.day() == 21);
    }

    /**
    * @brief Prueba para verificar que la funcion make_date() de la clase DayCountCalculator devuelve un objeto boost::gregorian::date correcto para una fecha en formato string valido en un año bisiesto.
    */
    BOOST_AUTO_TEST_CASE(make_date_leap_year) {
        auto d = DayCountCalculator::make_date("2024-02-29");
        BOOST_TEST(d.month() == 2);
        BOOST_TEST(d.day() == 29);
    }

    /**
    * @brief Prueba para verificar que make_date() lanza excepcion con formatos de fecha invalidos.
    */
    BOOST_AUTO_TEST_CASE(make_date_invalid_format_throws) {
        BOOST_CHECK_THROW(DayCountCalculator::make_date("invalid"), std::exception);
        BOOST_CHECK_THROW(DayCountCalculator::make_date("2025-13-01"), std::exception);
        BOOST_CHECK_THROW(DayCountCalculator::make_date("2025-01-32"), std::exception);
        BOOST_CHECK_THROW(DayCountCalculator::make_date(""), std::exception);
    }


BOOST_AUTO_TEST_SUITE_END()

// ---

BOOST_AUTO_TEST_SUITE(Actual_360_tests)

    //Test para verificar que el metodo compute_daycount() devuelve el numero de dias entre dos fechas en formato boost::gregorian::date correctamente.
    BOOST_AUTO_TEST_CASE(compute_daycount_same_day) {
        boost::gregorian::date from(2025, 1, 15);
        boost::gregorian::date to(2025, 1, 15);
        BOOST_TEST(Actual_360::compute_daycount(from, to) == 0);
        BOOST_TEST(Actual_360::compute_daycount("2025-01-15", "2025-01-15") == 0);
    }


    //Test para verificar que el metodo compute_daycount() devuelve el numero de dias entre dos fechas en formato boost::gregorian::date correctamente.
    BOOST_AUTO_TEST_CASE(compute_daycount_90_days) {
        boost::gregorian::date from(2025, 1, 1);
        boost::gregorian::date to(2025, 4, 1);
        BOOST_TEST(Actual_360::compute_daycount(from, to) == 90);
        BOOST_TEST(Actual_360::compute_daycount("2025-01-01", "2025-04-01") == 90);
    }



    /** @brief Fechas que voy a comprobar:
    * 01/04/2016	03/10/2016
    * 03/10/2016	03/04/2017
    * 03/04/2017	02/10/2017
    * 02/10/2017	02/04/2018
    */


    //Test para verificar que el metodo compute_daycount() devuelve el numero de dias entre dos fechas en formato string correctamente.
    BOOST_AUTO_TEST_CASE(compute_daycount_periods_string) {

        BOOST_TEST(Actual_360::compute_daycount("2016-04-01", "2016-10-03") == 185);
        BOOST_TEST(Actual_360::compute_daycount("2016-10-03", "2017-04-03") == 182);
        BOOST_TEST(Actual_360::compute_daycount("2017-04-03", "2017-10-02") == 182);
        BOOST_TEST(Actual_360::compute_daycount("2017-10-02", "2018-04-02") == 182);

    }


    //Test para verificar que el metodo compute_daycount() devuelve el numero de dias entre dos fechas en formato boost::gregorian::date correctamente.
    BOOST_AUTO_TEST_CASE(compute_daycount_periods_gregorian_date) {
        
        BOOST_TEST(Actual_360::compute_daycount(boost::gregorian::date(2016, 4, 1), boost::gregorian::date(2016, 10, 3)) == 185);
        BOOST_TEST(Actual_360::compute_daycount(boost::gregorian::date(2016, 10, 3), boost::gregorian::date(2017, 4, 3)) == 182);
        BOOST_TEST(Actual_360::compute_daycount(boost::gregorian::date(2017, 4, 3), boost::gregorian::date(2017, 10, 2)) == 182);
        BOOST_TEST(Actual_360::compute_daycount(boost::gregorian::date(2017, 10, 2), boost::gregorian::date(2018, 4, 2)) == 182);
        
        
    }




    //Test para verificar que el metodo operator() devuelve el DFC (day count fraction) entre dos fechas en formato string correctamente.
    BOOST_AUTO_TEST_CASE(operator_returns_dcf_string_args) {
        Actual_360 calc;

        BOOST_TEST(calc("2016-04-01", "2016-10-03") == 185/360.0, boost::test_tools::tolerance(1e-15));
        BOOST_TEST(calc("2016-10-03", "2017-04-03") == 182/360.0, boost::test_tools::tolerance(1e-10));
        BOOST_TEST(calc("2017-04-03", "2017-10-02") == 182/360.0, boost::test_tools::tolerance(1e-10));
        BOOST_TEST(calc("2017-10-02", "2018-04-02") == 182/360.0, boost::test_tools::tolerance(1e-10));


        //Acumulado de dfc
        BOOST_TEST(calc("2016-04-01", "2017-04-03") == (185 +182)/360.0, boost::test_tools::tolerance(1e-15));
        BOOST_TEST(calc("2016-04-01", "2017-10-02") == (185 +182 +182)/360.0, boost::test_tools::tolerance(1e-15));
        BOOST_TEST(calc("2016-04-01", "2018-04-02") == (185 +182 +182 +182)/360.0, boost::test_tools::tolerance(1e-15));




    }


    BOOST_AUTO_TEST_CASE(operator_returns_dcf_gregorian_date_args) {
        Actual_360 calc;

        BOOST_TEST(calc(boost::gregorian::date(2016, 4, 1), boost::gregorian::date(2016, 10, 3)) == 185/360.0, boost::test_tools::tolerance(1e-15));
        BOOST_TEST(calc(boost::gregorian::date(2016, 10, 3), boost::gregorian::date(2017, 4, 3)) == 182/360.0, boost::test_tools::tolerance(1e-10));
        BOOST_TEST(calc(boost::gregorian::date(2017, 4, 3), boost::gregorian::date(2017, 10, 2)) == 182/360.0, boost::test_tools::tolerance(1e-10));
        BOOST_TEST(calc(boost::gregorian::date(2017, 10, 2), boost::gregorian::date(2018, 4, 2)) == 182/360.0, boost::test_tools::tolerance(1e-10));
    

        //Acumulado de dfc
        BOOST_TEST(calc(boost::gregorian::date(2016, 4, 1), boost::gregorian::date(2017, 4, 3)) == (185 +182)/360.0, boost::test_tools::tolerance(1e-15));
        BOOST_TEST(calc(boost::gregorian::date(2016, 4, 1), boost::gregorian::date(2017, 10, 2)) == (185 +182 +182)/360.0, boost::test_tools::tolerance(1e-15));
        BOOST_TEST(calc(boost::gregorian::date(2016, 4, 1), boost::gregorian::date(2018, 4, 2)) == (185 +182 +182 +182)/360.0, boost::test_tools::tolerance(1e-15));

    }


    //Test para verificar el comportamiento cuando from > to (fecha inicio posterior a fecha fin).
    BOOST_AUTO_TEST_CASE(compute_daycount_from_after_to) {
        boost::gregorian::date from(2025, 4, 15);
        boost::gregorian::date to(2025, 1, 11);
        BOOST_TEST(Actual_360::compute_daycount(from, to) < 0);
        BOOST_TEST(Actual_360::compute_daycount("2025-04-15", "2025-01-11") < 0);
    }


    

BOOST_AUTO_TEST_SUITE_END()

// ---

BOOST_AUTO_TEST_SUITE(Thirty_360_tests)

    //Test para verificar que el metodo compute_daycount() devuelve el numero de dias entre dos fechas en formato boost::gregorian::date correctamente.
    BOOST_AUTO_TEST_CASE(compute_daycount_same_day) {
        boost::gregorian::date from(2025, 1, 15);
        boost::gregorian::date to(2025, 1, 15);
        BOOST_TEST(Thirty_360::compute_daycount(from, to) == 0);
        BOOST_TEST(Thirty_360::compute_daycount("2025-01-15", "2025-01-15") == 0);
    }


    //Test para verificar que el metodo compute_daycount() devuelve el numero de dias entre dos fechas en formato boost::gregorian::date correctamente (11 enero a 15 abril = 94 días en 30/360).
    BOOST_AUTO_TEST_CASE(compute_daycount_example_from_header) {
        boost::gregorian::date from(2025, 1, 11);
        boost::gregorian::date to(2025, 4, 15);
        BOOST_TEST(Thirty_360::compute_daycount(from, to) == 94);
        BOOST_TEST(Thirty_360::compute_daycount("2025-01-11", "2025-04-15") == 94);
    }


    /** @brief Fechas que voy a comprobar:
    * 01/04/2016	03/10/2016
    * 03/10/2016	03/04/2017
    * 03/04/2017	02/10/2017
    * 02/10/2017	02/04/2018
    */


    //Test para verificar que el metodo compute_daycount() devuelve el numero de dias entre dos fechas en formato string correctamente.
    BOOST_AUTO_TEST_CASE(compute_daycount_periods_string) {

        BOOST_TEST(Thirty_360::compute_daycount("2016-04-01", "2016-10-03") == 182);
        BOOST_TEST(Thirty_360::compute_daycount("2016-10-03", "2017-04-03") == 180);
        BOOST_TEST(Thirty_360::compute_daycount("2017-04-03", "2017-10-02") == 179);
        BOOST_TEST(Thirty_360::compute_daycount("2017-10-02", "2018-04-02") == 180);

    }


    //Test para verificar que el metodo compute_daycount() devuelve el numero de dias entre dos fechas en formato boost::gregorian::date correctamente.
    BOOST_AUTO_TEST_CASE(compute_daycount_periods_gregorian_date) {

        BOOST_TEST(Thirty_360::compute_daycount(boost::gregorian::date(2016, 4, 1), boost::gregorian::date(2016, 10, 3)) == 182);
        BOOST_TEST(Thirty_360::compute_daycount(boost::gregorian::date(2016, 10, 3), boost::gregorian::date(2017, 4, 3)) == 180);
        BOOST_TEST(Thirty_360::compute_daycount(boost::gregorian::date(2017, 4, 3), boost::gregorian::date(2017, 10, 2)) == 179);
        BOOST_TEST(Thirty_360::compute_daycount(boost::gregorian::date(2017, 10, 2), boost::gregorian::date(2018, 4, 2)) == 180);


    }




    //Test para verificar que el metodo operator() devuelve el DFC (day count fraction) entre dos fechas en formato string correctamente.
    BOOST_AUTO_TEST_CASE(operator_returns_dcf_string_args) {
        Thirty_360 calc;

        BOOST_TEST(calc("2016-04-01", "2016-10-03") == 182/360.0, boost::test_tools::tolerance(1e-15));
        BOOST_TEST(calc("2016-10-03", "2017-04-03") == 180/360.0, boost::test_tools::tolerance(1e-10));
        BOOST_TEST(calc("2017-04-03", "2017-10-02") == 179/360.0, boost::test_tools::tolerance(1e-10));
        BOOST_TEST(calc("2017-10-02", "2018-04-02") == 180/360.0, boost::test_tools::tolerance(1e-10));


        //Acumulado de dfc
        BOOST_TEST(calc("2016-04-01", "2017-04-03") == (182 +180)/360.0, boost::test_tools::tolerance(1e-15));
        BOOST_TEST(calc("2016-04-01", "2017-10-02") == (182 +180 +179)/360.0, boost::test_tools::tolerance(1e-15));
        BOOST_TEST(calc("2016-04-01", "2018-04-02") == (182 +180 +179 +180)/360.0, boost::test_tools::tolerance(1e-15));




    }


    BOOST_AUTO_TEST_CASE(operator_returns_dcf_gregorian_date_args) {
        Thirty_360 calc;

        BOOST_TEST(calc(boost::gregorian::date(2016, 4, 1), boost::gregorian::date(2016, 10, 3)) == 182/360.0, boost::test_tools::tolerance(1e-15));
        BOOST_TEST(calc(boost::gregorian::date(2016, 10, 3), boost::gregorian::date(2017, 4, 3)) == 180/360.0, boost::test_tools::tolerance(1e-10));
        BOOST_TEST(calc(boost::gregorian::date(2017, 4, 3), boost::gregorian::date(2017, 10, 2)) == 179/360.0, boost::test_tools::tolerance(1e-10));
        BOOST_TEST(calc(boost::gregorian::date(2017, 10, 2), boost::gregorian::date(2018, 4, 2)) == 180/360.0, boost::test_tools::tolerance(1e-10));


        //Acumulado de dfc
        BOOST_TEST(calc(boost::gregorian::date(2016, 4, 1), boost::gregorian::date(2017, 4, 3)) == (182 +180)/360.0, boost::test_tools::tolerance(1e-15));
        BOOST_TEST(calc(boost::gregorian::date(2016, 4, 1), boost::gregorian::date(2017, 10, 2)) == (182 +180 +179)/360.0, boost::test_tools::tolerance(1e-15));
        BOOST_TEST(calc(boost::gregorian::date(2016, 4, 1), boost::gregorian::date(2018, 4, 2)) == (182 +180 +179 +180)/360.0, boost::test_tools::tolerance(1e-15));

    }


    //Test para verificar el comportamiento cuando from > to (fecha inicio posterior a fecha fin).
    BOOST_AUTO_TEST_CASE(compute_daycount_from_after_to) {
        boost::gregorian::date from(2025, 4, 15);
        boost::gregorian::date to(2025, 1, 11);
        BOOST_TEST(Thirty_360::compute_daycount(from, to) < 0);
        BOOST_TEST(Thirty_360::compute_daycount("2025-04-15", "2025-01-11") < 0);
    }


    

BOOST_AUTO_TEST_SUITE_END()

