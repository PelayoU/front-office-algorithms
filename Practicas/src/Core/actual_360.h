#ifndef ACTUAL_360_H
#define ACTUAL_360_H
#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "daycountcalculator.h"





/**
 * @brief Clase que hereda de DayCountCalculator, es decir, hereda el metodo make_date de DayCountCalculator, asi, los metodos de esta clase podran usarlo tambien.
 * El objetivo de esta clase es calcular el DCF (day count fraction) entre dos fechas utilizando la convencion Actual/360 con el metodo operator(), tanto si estas son de tipo boost::gregorian::date como de tipo string.
 */
class Actual_360 : public DayCountCalculator{ 
public:

    /**
     * @brief Calcula el número de días entre dos fechas en formato boost::gregorian::date 
     * La funcion es static simplemente por si necesito llamarla rapidamente para un calculo puntual, pero el uso principal de esta funcion es para calcular el DFC con el operator().
     * @param from Fecha de inicio en formato boost::gregorian::date 
     * @param to Fecha de fin en formato boost::gregorian::date 
     * @return Número de días entre las dos fechas, para ello utilizamos la funcion days() de boost::gregorian::date. Devuelve un short, que es un entero de 16 bits para optimizar el espacio en memoria.
     */
    static short compute_daycount(const boost::gregorian::date & from, const boost::gregorian::date & to);



    /**
     * @brief Calcula el número de días entre dos fechas en formato string, para ello utiliza la funcion make_date de DayCountCalculator
     * para convertir el string en un objeto boost::gregorian::date, y luego calcula el numero de días etre esas dos fechas usando sobrecarga de funciones, es decir, llamando a la funcion compute_daycount() que calcula el numero de días entre dos fechas de tipo boost::gregorian::date.
     * @param from Fecha de inicio en formato string (YYYY-MM-DD).
     * @param to Fecha de fin en formato string (YYYY-MM-DD).
     * @return Número de días entre las dos fechas. Devuelve un short, que es un entero de 16 bits.
     */
    static short compute_daycount(const std::string& from, const std::string& to); 
   


    /**
     * @brief El metodo operator() es el que se encarga de sacar el DFC (day count fraction) entre dos fechas.
     * Para ello utiliza el metodo compute_daycount() que calcula el numero de días entre dos fechas y luuego lo divide por 360.0 para obtener el DFC.
     * Como compute_daycount está sobrecargado para aceptar tanto string como boost::gregorian::date, usamos un template en operator() para poder llamarlo con cualquier tipo de fecha.
     * Los metodos template se definen y declaran en el .h a diferencia de los metodos normales que se declaran en el .h y se definen en el .cpp
     * @param start Fecha de inicio en formato DATE o String.
     * @param end Fecha de fin en formato DATE o String.
     * @return DCF (day count fraction) entre las dos fechas usando el metodo sobrecargado compute_daycount(). Para devolver un double ya que el DFC es una fraccion, dividimos un short por un double (360.0) para obtener un calculo double. 
     */
    template<class DATE> double operator () (const DATE& start, const DATE& end) const { 
       
        return compute_daycount(start, end)/ 360.0; 
    } 


};


#endif
