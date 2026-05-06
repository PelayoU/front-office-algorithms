#include "instrumentdescription.h"
#include <stdexcept>



namespace {

/**
 * @brief Valida la integridad de una pata (leg).
 * Comprueba que el schedule tiene al menos 2 fechas, el notional es positivo y la convención de días es válida.
 * @throws std::invalid_argument si algún campo es inconsistente.
 */
void validate_leg(const LegDescription& leg, const char* leg_name) {
    if (leg.schedule.size() < 2)
        throw std::invalid_argument(std::string(leg_name) + ": schedule debe tener al menos 2 fechas (inicio y vencimiento)");
    if (leg.schedule.front() >= leg.schedule.back())
        throw std::invalid_argument(std::string(leg_name) + ": la primera fecha del schedule debe ser anterior a la última");
    if (leg.notional <= 0.0)
        throw std::invalid_argument(std::string(leg_name) + ": notional debe ser positivo");
    if (leg.day_count != "30/360" && leg.day_count != "Actual/360")
        throw std::invalid_argument(std::string(leg_name) + ": day_count debe ser \"30/360\" o \"Actual/360\"");
}

} // namespace



/**
 * @brief Valida la descripción del instrumento.
 * @throws std::invalid_argument si los datos son inconsistentes o incompletos.
 */
void InstrumentDescription::validate() const {
    if (type == Type::swap) {
        validate_leg(payer, "payer");
        validate_leg(receiver, "receiver");
    } else if (type == Type::bond) {
        validate_leg(receiver, "receiver (cupón)");
        if (receiver.rate < 0.0)
            throw std::invalid_argument("receiver (cupón): rate no puede ser negativo");
    }
}
