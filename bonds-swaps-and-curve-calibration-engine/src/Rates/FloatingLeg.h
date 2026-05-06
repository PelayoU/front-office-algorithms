#ifndef FLOATINGLEG_H
#define FLOATINGLEG_H

#include <vector>
#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "Rates/Leg.h"

/**
 * @brief Pata flotante: tipos estimados por forwards desde la curva (PDF ec. 2.3 y 2.2).
 *
 * Para cada periodo [start, end] se calcula el tipo forward continuamente compuesto
 * y se convierte a semestral R_6m = 2*(exp(Rc/2)-1). Opcionalmente se aplica un spread.
 */
class FloatingLeg : public Leg {


private:
    std::vector<boost::gregorian::date> schedule_;
    double notional_;
    std::string day_count_;
    double spread_;


    double year_fraction(const boost::gregorian::date& from,const boost::gregorian::date& to) const;


public:
    /**
     * @param schedule Fechas de inicio/fin de cada periodo (y de pago).
     * @param notional Nominal.
     * @param day_count "Actual/360" o "30/360".
     * @param spread Resta sobre el tipo flotante (p. ej. 0.001 para -10 bps). Por defecto a 0.0.
     */
    FloatingLeg(const std::vector<boost::gregorian::date>& schedule, double notional, const std::string& day_count, double spread = 0.0);



    /** 
    * @brief Calcula el precio de la pata flotante.
    * @param curve Curva de descuento.
    * @return Precio de la pata flotante.
    */
    double price(const ZeroCouponCurve& curve) const override;



    /** 
    * @brief Calcula el tipo forward continuamente compuesto para el periodo [start, end] y con el saca el Rm (Inteses Simple que genera el mismo factor de descuento o capitalizacion continua).
    * @note Es decir,get_period_rate no devuelve el forward en continua, sino el tipo simple del periodo (el “Euribor” del periodo): la tasa simple que tiene el mismo factor de capitalización que el forward continuamente compuesto.
    * @param curve Curva de descuento.
    * @param start Fecha de inicio.
    * @param end Fecha de fin.
    * @return Tipo forward continuamente compuesto para el periodo [start, end].
    */
    double get_period_rate(const ZeroCouponCurve& curve, const boost::gregorian::date& start, const boost::gregorian::date& end) const;



    /** 
    * @brief Calcula el cupón para el periodo [start, end]: notional * (tipo_periodo + spread) * tau.
    * @param curve Curva de descuento.
    * @param start Fecha de inicio.
    * @param end Fecha de fin.
    * @return Cupón en unidades de notional.
    */
    double get_period_coupon(const ZeroCouponCurve& curve, const boost::gregorian::date& start, const boost::gregorian::date& end) const;

};

#endif
