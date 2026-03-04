#include "instrumentdescription.h"
#include <stdexcept>



namespace {

/**
 * @brief Función auxiliar para validar la integridad de una pata (leg).
 * @note He puesto validate_leg dentro de unnamespace porque sirve como funcion auxiliar para la funcion validate() de InstrumentDescription, 
 * nadie va a usarla explicitamente, usaran validate() para validar el instrumento completo.
 * * @details Se utiliza un puntero (const char*) para el nombre de la pata por eficiencia.
 * En C++, un literal como "payer" reside en la zona de memoria Read-Only Data.
 * Al pasar "payer", el compilador no copia la cadena, sino que pasa la dirección 
 * de memoria (puntero) del primer carácter ('p'). Esto evita la sobrecarga de 
 * crear objetos std::string innecesarios durante la validación.
 * Por tanto, cuando pasas "payer" a la función, no estás pasando la palabra entera, estás pasando la dirección de memoria (el puntero) donde esa palabra está guardada.
 * @param leg Referencia constante a la descripción de la pata a validar.
 * @param leg_name Puntero a la cadena de texto con el nombre de la pata (ej. "payer").
 * @throws std::invalid_argument Si algún campo (fechas, nominal, etc.) es inconsistente.
 */
void validate_leg(const LegDescription& leg, const char* leg_name) {
    if (leg.start_date.empty())
        throw std::invalid_argument(std::string(leg_name) + ": start_date vacío");
    if (leg.end_date.empty())
        throw std::invalid_argument(std::string(leg_name) + ": end_date vacío");
    if (leg.notional <= 0.0)
        throw std::invalid_argument(std::string(leg_name) + ": notional debe ser positivo");
    if (leg.day_count != "30/360" && leg.day_count != "Actual/360") 
        throw std::invalid_argument(std::string(leg_name) + ": day_count debe ser \"30/360\" o \"Actual/360\"");
    if (leg.start_date > leg.end_date)
        throw std::invalid_argument(std::string(leg_name) + ": start_date no puede ser posterior a end_date");
}

} // namespace



/**
 * @brief Valida la descripción del instrumento.
 * @note Se valida el instrumento completo, comprobamos que no lanza excepciones.
 * @throws std::invalid_argument si los datos son inconsistentes o incompletos.
 */
void InstrumentDescription::validate() const {
    if (type == Type::swap) {
        validate_leg(payer, "payer"); // llama a la funcion validate_leg para validar la pata payer.
        validate_leg(receiver, "receiver"); // llama a la funcion validate_leg para validar la pata receiver.
    } else if (type == Type::bond) {
        validate_leg(receiver, "receiver (cupón)");
        if (receiver.rate < 0.0)
            throw std::invalid_argument("receiver (cupón): rate no puede ser negativo");
    }
}

