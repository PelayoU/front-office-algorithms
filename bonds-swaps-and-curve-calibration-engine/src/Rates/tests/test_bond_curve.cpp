#include <boost/test/unit_test.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "Core/Factory.h"
#include "Core/instrumentdescription.h"
#include "Core/ZeroCouponCurve.h"
#include "Rates/FixedLeg.h"
#include "Rates/Bond.h"
#include "Rates/ScheduleUtils.h"

/**
 * Ejemplo del PDF tema 2 (1.3.1): bono 2 años, principal 100, cupón 6% anual semestral.
 * Curva cero cupón (continuamente compuesta): 0.5y 5%, 1y 5.8%, 1.5y 6.4%, 2y 6.8%.
 * PV = 3*exp(-0.05*0.5) + 3*exp(-0.058*1) + 3*exp(-0.064*1.5) + 103*exp(-0.068*2) = 98.39
 */
BOOST_AUTO_TEST_SUITE(Bond_curve_PDF_tests)




/**
 * @brief Test para verificar que el precio de la pata fija coincide con el precio del bono del ejemplo del PDF tema 2 (1.3.1).
 * 

 */
BOOST_AUTO_TEST_CASE(bond_pv_matches_pdf_example) {
   
   
   //Creamos la curva de tipos cero cupón con la convención de días "30/360".
   const boost::gregorian::date today(2014, 4, 1);

    std::vector<ZeroCouponCurve::Node> nodes = {
        {boost::gregorian::date(2014, 10, 1), 0.05},
        {boost::gregorian::date(2015, 4, 1), 0.058},
        {boost::gregorian::date(2015, 10, 1), 0.064},
        {boost::gregorian::date(2016, 4, 1), 0.068}
    };
    ZeroCouponCurve curve(today, nodes, "30/360");



    //Creamos el vector de fechas de pago para la pata fija, las fechas tienen  que ser las mismas porque luego tengo que descontar desde esa fecha y para ello necesito ese interes cero cupon.
    std::vector<boost::gregorian::date> schedule = {
        boost::gregorian::date(2014, 4, 1),
        boost::gregorian::date(2014, 10, 1),
        boost::gregorian::date(2015, 4, 1),
        boost::gregorian::date(2015, 10, 1),
        boost::gregorian::date(2016, 4, 1)
    };


    //Creamos la pata fija con el vector de fechas de pago, el notional, el tipo de interes anual y la convención de días "30/360" (En el ejemplo para el sacar el cupon usamos convencion de días "30/360").
    FixedLeg leg(schedule, 100.0, 0.06, "30/360", true);
    double pv = leg.price(curve);

    //Comprobamos que el precio de la pata fija coincide con el precio del bono del ejemplo del PDF tema 2 (1.3.1).
    //PV = 3*exp(-0.05*0.5) + 3*exp(-0.058*1) + 3*exp(-0.064*1.5) + 103*exp(-0.068*2) = 98.3850627729396
    BOOST_TEST(pv == 98.3850627729396, boost::test_tools::tolerance(1e-15));

}




/**
 * @brief Test para verificar que el precio del bono coincide con el precio del bono del ejemplo del PDF tema 2 (1.3.1). 
 */
BOOST_AUTO_TEST_CASE(bond_instrument_pv_matches_pdf) {
    const boost::gregorian::date today(2014, 4, 1);

    std::vector<ZeroCouponCurve::Node> nodes = {
        {boost::gregorian::date(2014, 10, 1), 0.05},
        {boost::gregorian::date(2015, 4, 1), 0.058},
        {boost::gregorian::date(2015, 10, 1), 0.064},
        {boost::gregorian::date(2016, 4, 1), 0.068}
    };
    ZeroCouponCurve curve(today, nodes, "30/360");

    std::vector<boost::gregorian::date> schedule = {
        boost::gregorian::date(2014, 4, 1),
        boost::gregorian::date(2014, 10, 1),
        boost::gregorian::date(2015, 4, 1),
        boost::gregorian::date(2015, 10, 1),
        boost::gregorian::date(2016, 4, 1)
    };



    auto fixed_leg = std::make_unique<FixedLeg>(schedule, 100.0, 0.06, "30/360", true); //Equivalente a poner FixedLeg* fixed_leg = new FixedLeg(...) pero con punteros inteligentes.
    


    /*
    Con punteros primitivos copia la dirección de memoria sin problemas. El problema es ¿quién borra la memoria despues? Si el test la borra y el Bond tambien, el programa explota.
    El puntero inteligente tiene el constructor de copia deshabilitado. No permite que dos variables tengan la misma dirección.
    Por eso uso std::move() para convertir fixed_leg en un right value. 
    Realmente esto de los right value es una mentira como un templo, estoy moviendo el puntero a el argumento del bono coupons, que es una variable unique_ptr.
    Es decir, al poner std::move le digo al compilador que elija el constructor de movimiento de unique_ptr en lugar del constructor de copia,
    y mueve fixed_leg a coupons. (vamos ahora coupons es un puntero que apunta a la direccion que apuntaba fixed_leg)
    y fixed_leg apunta a nullptr)
     (el puntero fixed_leg pasa a ser nullptr, ya no apunta a nada)
    Realmente cuando digo que mueve fixed_leg a coupons, ese es el primer paso. 
    Luego, dentro del constructor de Bond, ese coupons (que es el argumento) se tiene que mover otra vez a la variable miembro coupons_.
    Bond::Bond(std::unique_ptr<FixedLeg> coupons,const ZeroCouponCurve& discount_curve)
    : coupons_(std::move(coupons)),discount_curve_(discount_curve) {}
    */
    Bond bond(std::move(fixed_leg), curve); //Creamos un bono con la pata fija y la curva de tipos cero cupon.


    BOOST_TEST(bond.price() == 98.3850627729396, boost::test_tools::tolerance(1e-15));
}







/** 
 * @brief Test para verificar que el metodo get_dcf() (discount count fraction) y get_discount_factor() (factor de descuento DF(0,T) = exp(-r*T) con capitalización continua) de la curva de tipos cero cupón devuelven los valores correctos.
 */
BOOST_AUTO_TEST_CASE(zero_curve_dcf_and_zc_continuous) {
    const boost::gregorian::date today(2014, 4, 1);
    std::vector<ZeroCouponCurve::Node> nodes = {
        {boost::gregorian::date(2014, 10, 1), 0.05},
        {boost::gregorian::date(2016, 4, 1), 0.068}
    };
    ZeroCouponCurve curve(today, nodes, "30/360");

    // 6 meses con 30/360 ≈ 0.5
    const double dcf_6m = curve.get_dcf(boost::gregorian::date(2014, 10, 1));

    BOOST_TEST(dcf_6m == 0.5, boost::test_tools::tolerance(1e-15));

    // Z(0,0.5) = exp(-0.05*0.5)
    const double zc_6m = curve.get_discount_factor(boost::gregorian::date(2014, 10, 1));

    BOOST_TEST(zc_6m == std::exp(-0.05 * 0.5), boost::test_tools::tolerance(1e-15));
}

/**
 * @brief Factory construye Bond con schedule explícito; precio coincide con el del PDF.
 */
BOOST_AUTO_TEST_CASE(factory_builds_bond_and_pv_matches_pdf) {
    const boost::gregorian::date today(2014, 4, 1);
    std::vector<ZeroCouponCurve::Node> nodes = {
        {boost::gregorian::date(2014, 10, 1), 0.05},
        {boost::gregorian::date(2015, 4, 1),  0.058},
        {boost::gregorian::date(2015, 10, 1), 0.064},
        {boost::gregorian::date(2016, 4, 1),  0.068}
    };
    ZeroCouponCurve curve(today, nodes, "30/360");

    const std::vector<boost::gregorian::date> schedule =
        Rates::make_schedule("2014-04-01", "2016-04-01", 6);

    InstrumentDescription desc(InstrumentDescription::Type::bond);
    desc.receiver.schedule  = schedule;
    desc.receiver.notional  = 100.0;
    desc.receiver.rate      = 0.06;
    desc.receiver.day_count = "30/360";

    std::unique_ptr<Instrument> instrument = Factory::instance()(desc, curve);
    BOOST_TEST(instrument != nullptr);
    BOOST_TEST(instrument->price() == 98.3850627729396, boost::test_tools::tolerance(1e-10));
}

BOOST_AUTO_TEST_SUITE_END()
