#ifndef RATES_SCHEDULEUTILS_H
#define RATES_SCHEDULEUTILS_H

#include <string>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace Rates {

/**
 * @brief Genera un schedule entre start_date y end_date con periodo dado en meses (formato "YYYY-MM-DD").
 * Incluye la fecha de inicio, fechas cada months_per_period meses y la fecha de fin.
 * @param months_per_period 1=mensual, 3=trimestral, 6=semestral, 12=anual.
 */
std::vector<boost::gregorian::date> make_schedule(const std::string& start_date, const std::string& end_date, int months_per_period);

/**
 * @brief Genera un schedule semestral entre start_date y end_date (formato "YYYY-MM-DD").
 * Incluye la fecha de inicio, fechas cada 6 meses y la fecha de fin.
 */
std::vector<boost::gregorian::date> make_semiannual_schedule(const std::string& start_date, const std::string& end_date);

}

#endif
