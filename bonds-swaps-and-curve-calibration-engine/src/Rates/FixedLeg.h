#ifndef FIXEDLEG_H
#define FIXEDLEG_H

#include <vector>
#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "Rates/Leg.h"

/**
 * @brief Pata fija de un bono o swap. 
 * @note Es el objeto que hace el pricing, es decir, calcula el valor presente de los cupones (y opcionalmente del principal).
 * Dado un calendario de fechas de pago, un nominal y un tipo fijo,
 * calcula el valor presente de los cupones (y opcionalmente del principal).
 */
class FixedLeg : public Leg {

private:
    std::vector<boost::gregorian::date> schedule_; //Vector de fechas de pago.
    double notional_;
    double rate_;
    std::string day_count_;
    bool include_notional_;


    /**
    * @brief Calcula la fracción de año entre dos fechas usando la convención de días especificada. (day count fraction) usando el metodo operator() de la clase Actual_360 o Thirty_360.
    * @param from Fecha de inicio.
    * @param to Fecha de fin.
    * @return Fracción de año entre las dos fechas.
    * @note Utiliza Actual/360 o 30/360 dependiendo de la convención de días especificada utilizando el metodo operator() de la clase Actual_360 o Thirty_360.
    * @note Lanza una excepción si la convención de días no es válida.
    */
    double year_fraction(const boost::gregorian::date& from, const boost::gregorian::date& to) const;



public:
//Cuando te dan un bono tu sabes las fechas en las que vas a recibir pago independientmente de la convencion, lo que no sabes es cuanto en cada pago aunque sea pata fija.
//Cuanto recibes en cada pago depende de la convencion de dias que uses, es decir, tienes que calcular el day count fraction de una fecha a otra y multiplicarlo por el notional y por el tipo de interes anual.


    /**
    * @brief Constructor para inicializar la pata fija.
    * @param schedule Vector de fechas de pago. 
    * @param notional Notional del instrumento.
    * @param rate Tipo de interés anual.
    * @param day_count Convención de días. "Actual/360" o "30/360".
    * @param include_notional Si incluir el notional al vencimiento.
    */
    FixedLeg(const std::vector<boost::gregorian::date>& schedule, double notional, double rate, const std::string& day_count, bool include_notional = true);


        
    /**
    * @brief Calcula el precio de la pata fija.
    * @note Para ello necesita la curva de descuento para calcular el factor de descuento para cada fecha de pago.
    * @param curve Curva de descuento.
    * @return Precio de la pata fija.
    */
    double price(const ZeroCouponCurve& curve) const override;


};

#endif

