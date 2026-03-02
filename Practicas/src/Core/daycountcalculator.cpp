#include "daycountcalculator.h"


boost::gregorian::date DayCountCalculator::make_date(const std::string& date){

    // funcion from_string de boost::gregorian::date, que convierte un string en un objeto boost::gregorian::date.
    return boost::gregorian::from_string(date);
}




