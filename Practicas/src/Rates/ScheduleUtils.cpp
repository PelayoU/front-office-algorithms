#include "Rates/ScheduleUtils.h"
#include "Core/daycountcalculator.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <stdexcept>

namespace Rates {

namespace {
    boost::gregorian::date add_months(const boost::gregorian::date& d, int months) {
        using namespace boost::gregorian;
        int y = d.year();
        int m = d.month();
        int day = d.day();
        m += months;
        y += (m - 1) / 12;
        m = (m - 1) % 12 + 1;
        unsigned short last = gregorian_calendar::end_of_month_day(static_cast<unsigned short>(y), static_cast<unsigned short>(m));
        if (day > static_cast<int>(last)) day = last;
        return date(static_cast<unsigned short>(y), static_cast<unsigned short>(m), static_cast<unsigned short>(day));
    }
}

std::vector<boost::gregorian::date> make_schedule(const std::string& start_date, const std::string& end_date, int months_per_period) {
    if (months_per_period <= 0)
        throw std::invalid_argument("make_schedule: months_per_period debe ser positivo");
    boost::gregorian::date start = DayCountCalculator::make_date(start_date);
    boost::gregorian::date end = DayCountCalculator::make_date(end_date);
    std::vector<boost::gregorian::date> schedule;
    schedule.push_back(start);
    boost::gregorian::date d = add_months(start, months_per_period);
    while (d < end) {
        schedule.push_back(d);
        d = add_months(d, months_per_period);
    }
    if (schedule.back() != end) {
        schedule.push_back(end);
    }
    return schedule;
}

std::vector<boost::gregorian::date> make_semiannual_schedule(const std::string& start_date, const std::string& end_date) {
    return make_schedule(start_date, end_date, 6);
}

}
