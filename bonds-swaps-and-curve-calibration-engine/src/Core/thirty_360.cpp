#include "thirty_360.h"




short Thirty_360::compute_daycount(const short years, 
    const short months, 
    const short days_from, 
    const short days_to)
    { 
    
    return (360 * years) + 30 * (months -1) 
        + std::max<short>(0, 30 - days_from) 
        + std::min<short>(30, days_to); 
        
}



short Thirty_360::compute_daycount(const boost::gregorian::date & from, const boost::gregorian::date & to) { 
    
    auto from_date(from.year_month_day()); //Estructura de 3 componentes: year, month, day de from
    auto to_date(to.year_month_day()); //Estructura de 3 componentes: year, month, day de to.
    auto years = to_date.year - from_date.year; //Calculamos el numero de años entre las dos fechas
    auto months = to_date.month - from_date.month; //Calculamos el numero de meses entre las dos fechas
    return Thirty_360::compute_daycount(years, months, from_date.day, to_date.day); //Llamamos a la funcion compute_daycount() que calcula el numero de dias entre las dos fechas dando como argumento el numero de años, meses, dias desde el inicio del mes de la fecha de inicio y dias desde el inicio del mes de la fecha de fin.

}



short Thirty_360::compute_daycount(const std::string& from, const std::string& to) { 
    // parse from string + build date class + get a structure date structure. 
    boost::gregorian::date from_date = DayCountCalculator::make_date(from); 
    boost::gregorian::date to_date = DayCountCalculator::make_date(to); 
    return Thirty_360::compute_daycount(from_date, to_date); 

}













