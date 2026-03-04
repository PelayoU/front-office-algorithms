#ifndef INSTRUMENTDESCRIPTION_H
#define INSTRUMENTDESCRIPTION_H

#include <string>



/**
 * @brief Descripción de una pata (leg) de un instrumento: fechas, nominal, tipo de interés y convención de fechas.
 * @note Se usa tanto para las patas payer/receiver de un swap como para la definición de cupones de un bono.
 * He puesto struct en lugar de class porque como voy a tener una factoria o quien construya la descripcion y necesitará
 * tocar 15 parámetros distintos para configurar un Swap (fechas, nominales, calendarios, convenciones de días, etc.), 
 * poner un get y un set para cada uno es una pérdida de tiempo y hace que el código sea mucho más difícil de leer.
 * Ademas, como es una descripcion de una pata de un instrumento, no tiene sentido que tenga metodos, solo tiene atributos.
 */
struct LegDescription {
    std::string start_date;
    std::string end_date;
    double notional = 0.0; // valor nominal del instrumento, por ejemplo, 1000000 euros. Inicializado a 0.0 para evitar warnings.
    double rate = 0.0; // tipo de interés del instrumento, por ejemplo, 0.05 para un 5% anual. Inicializado a 0.0 para evitar warnings.
    std::string day_count = "30/360";  // "30/360" o "Actual/360".Inicializado a "30/360" para evitar warnings. Quizas mejor un enum para day_count, luego lo miro.
    bool is_fixed = true;              // true = pata fija, false = pata flotante (solo relevante en swaps)
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
