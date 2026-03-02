#ifndef THIRTY_360_H
#define THIRTY_360_H
#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "daycountcalculator.h"



/**
 * @brief Clase que hereda de DayCountCalculator, es decir, hereda el metodo make_date de DayCountCalculator, asi, los metodos de esta clase podran usarlo tambien.
 * El objetivo de esta clase es calcular el DCF (day count fraction) entre dos fechas usando la convencion 30/360 con el metodo operator(), tanto si estas son de tipo boost::gregorian::date como de tipo string.
 * La convencion 30/360 asume que cada mes tiene 30 dias y el año tiene 360 dias.
 * Es decir, si es del 11 de enero a 15 de abril. Serian 30*3 + 4 = 94 dias.
 */
class Thirty_360 : public DayCountCalculator{
public:

    /**
    * @brief Calcula el numero de dias entre dos fechas utilizando la convencion 30/360 dando como argumento el numero de años, meses, dias desde el inicio del mes de la fecha de inicio y dias desde el inicio del mes de la fecha de fin.
    *- Retorna (360 * years) + 30 * (months - 1) + std::max(0, 30 - days_from) + std::min(30, days_to), es muy sencillo:
    * - Sumas la diferencia de años (years)*360 (cada año tiene 360 dias en esta convencion)
    * - Restas la diferencia de meses (months). Si from = mes 7 y to = mes 4 quiere decir que entre los dos hay 1 año completo menos 3 meses, un total de 9 meses. Luego le tienes que restas al año 3 meses ademas de 1 mes mas porque luego meteras los dias que faltan. Es de
    *   decir, le restas 4 meses: + 30 * (months - 1) -> 30 * (-4 - 1)
    * - Finalmente suma los días que quedan del primer mes (max(0, 30 - days_from))y los días transcurridos del último (std::min(30, days_to))
    * @param years Numero de años entre las dos fechas.
    * @param months Numero de meses entre las dos fechas.
    * @param days_from Numero de dias desde el inicio del mes de la fecha de inicio.
    * @param days_to Numero de dias desde el inicio del mes de la fecha de fin.
    * @return Numero de dias entre las dos fechas.
    */
    static short compute_daycount(const short years, const short months, const short days_from, const short days_to);



    /**
    * @brief Calcula el numero de dias entre dos fechas utilizando la convencion 30/360 entre dos fechas en formato boost::gregorian::date.
    * @param from Fecha de inicio en formato boost::gregorian::date.
    * @param to Fecha de fin en formato boost::gregorian::date.
    * @return Numero de dias entre las dos fechas.
    */
    static short compute_daycount(const boost::gregorian::date & from, const boost::gregorian::date & to);




    /**
    * @brief Calcula el numero de dias entre dos fechas utilizando la convencion 30/360 entre dos fechas en formato string. He cambiado un poco el codigo utilizando el metodo make_date de DayCountCalculator
    * @param from Fecha de inicio en formato string.
    * @param to Fecha de fin en formato string.
    * @return Numero de dias entre las dos fechas.
    */
    static short compute_daycount(const std::string& from, const std::string& to);



    /**
    * @brief El metodo operator() es el que se encarga de sacar el DFC (day count fraction) entre dos fechas.
    * Para ello utiliza el metodo compute_daycount() que calcula el numero de días entre dos fechas y luuego lo divide por 360.0 para obtener el DFC.
    * Como compute_daycount está sobrecargado para aceptar tanto string como boost::gregorian::date, usamos un template en operator() para poder llamarlo con cualquier tipo de fecha.
    * Los metodos template se definen y declaran en el .h a diferencia de los metodos normales que se declaran en el .h y se definen en el .cpp
    * @param start Fecha de inicio en formato DATE o String.
    * @param end Fecha de fin en formato DATE o String.
    * @return DFC (day count fraction) entre las dos fechas usando el metodo sobrecargado compute_daycount(). Para devolver un double ya que el DFC es una fraccion, dividimos un short por un double (360.0) para obtener un calculo double. 
    */
    template<class DATE> double operator () (const DATE& start, const DATE& end) const { 
        
        // Retorna el DFC (day count fraction) entre las dos fechas usando el metodo sobrecargado compute_daycount(). Para devolver un double ya que el DFC es una fraccion, dividimos un short por un double (360.0) para obtener un calculo double. 
        return compute_daycount(start, end)/ 360.0; 
    
    }



};


#endif