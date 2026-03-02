#include "actual_360.h"



short Actual_360::compute_daycount(const boost::gregorian::date & from, const boost::gregorian::date & to){

    //retorna el dumero de dias entre dos fechas utilizando la funcion days() de boost::gregorian::date.
    return (to - from).days();
}



short Actual_360::compute_daycount(const std::string& from, const std::string& to){

    // parse from string + build date class
    boost::gregorian::date from_date = DayCountCalculator::make_date(from); 
    boost::gregorian::date to_date = DayCountCalculator::make_date(to); 

    //llama a la funcion compute_daycount() pero la que pide dos argumentos de tipo boost::gregorian::date.
    return Actual_360::compute_daycount(from_date, to_date);
}






