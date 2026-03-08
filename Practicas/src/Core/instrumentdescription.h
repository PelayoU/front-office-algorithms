#ifndef INSTRUMENTDESCRIPTION_H
#define INSTRUMENTDESCRIPTION_H

#include <string>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>



/**
 * @brief Descripción de una pata (leg) de un instrumento.
 *
 * Contiene el schedule explícito de fechas de pago (ya generado por quien crea la descripción,
 * por ejemplo con Rates::make_schedule), el nominal, tipo, convención de días y si es fija o flotante.
 *
 * El schedule es un vector de fechas [inicio, pago1, pago2, ..., vencimiento].
 * Las fechas deben coincidir con los nodos de la ZeroCouponCurve para poder descontar.
 *
 * He puesto struct en lugar de class porque la descripción no tiene comportamiento; solo datos.
 */
struct LegDescription {
    std::vector<boost::gregorian::date> schedule; // fechas [inicio, ..., vencimiento]; mínimo 2 fechas.
    double notional = 0.0;
    double rate = 0.0;
    std::string day_count = "30/360";  // "30/360" o "Actual/360"
    bool is_fixed = true;              // true = pata fija, false = pata flotante
};







/**
 * @brief Descripción de un instrumento para su construcción por la factoría.
 * Contiene el tipo (bond/swap) y los datos de las patas (payer/receiver).
 * Para un bono, solo se usa receiver como definición del cupón; para un swap, payer y receiver.
 */
struct InstrumentDescription {
    
    /**
     * @brief Enum para el tipo de instrumento.
     * @note enum a efectos del compilador es como 0, 1 . Podria meter una string "bond" o "swap" pero computacionalmente es mucho menos eficiente que enum, porque es 0 o 1 no una string que es mucho mas pesada y crea ineficiencias de memoria y cpu.
     * @note Ademas al meter type en el constructor de InstrumentDescription te aseguras de que el type es valido, no puedes meter un type que no sea bond o swap. En cambio con una string podrias meter un error como "boond"
     */
    enum class Type { bond, swap }; 

    Type type;


    /**
     * @brief Constructor para inicializar el tipo de instrumento.
     * @param t Tipo de instrumento.
     * @note explicit es una buena practica para que a una funcion que acepta un InstrumentDescription no acepte un type, porque como ve que con un type puede crear un instrumenDescription pues te lo aceptaria y crearia un InstrumentDescription con ese type si no pones explicit.
     * @note Es realmente para que no cometas un error y metas en una funcion que espera un InstrumentDescription completo un type que no tiene sentido y te va a crear un InstrumentDescription vacio. El problema principal es que no te darias cuenta de tu error, pasaria desapercibido.
     */
    explicit InstrumentDescription(Type t) : type(t) {} //constructor para inicializar el tipo de instrumento.

    LegDescription payer; // objeto LegDescription para la pata payer del instrumento.
    LegDescription receiver; // objeto LegDescription para la pata receiver del instrumento.

    /**
     * @brief Comprueba que la descripción sea coherente y válida para construir el instrumento.
     * @throws std::invalid_argument si los datos son inconsistentes o incompletos.
     */
    void validate() const;
};

#endif
