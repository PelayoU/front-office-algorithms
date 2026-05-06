#ifndef DAYCOUNTCALCULATOR_H
#define DAYCOUNTCALCULATOR_H
#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>

/**
 * @brief Clase base para convenciones de conteo de días (day count).
 * Las clases derivadas (Actual_360, Thirty_360) implementan el cálculo del DCF (day count fraction)
 * entre dos fechas. Proporciona make_date() estático para convertir strings en fechas.
 */
class DayCountCalculator {
public:

    /**
     * @brief Crea un objeto date a partir de una fecha en formato string y devuelve un objeto boost::gregorian::date. Es una función static para
     * llamarla sin instanciar la clase.
     * @param date Fecha en formato string (por ejemplo "2026-01-21").
     * @return Objeto boost::gregorian::date que representa una fecha en el
     * calendario gregoriano, con métodos para día, mes, año,
     * día de la semana, año bisiesto, etc...
     */
    static boost::gregorian::date make_date(const std::string& date); 
    
    virtual ~DayCountCalculator() = default; // buena práctica en clases base
};

#endif


