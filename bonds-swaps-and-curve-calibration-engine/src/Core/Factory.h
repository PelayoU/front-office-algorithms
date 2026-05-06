#ifndef FACTORY_H
#define FACTORY_H

#include <functional>
#include <map>
#include <memory>

#include "Core/instrument.h"
#include "Core/instrumentdescription.h"
#include "Core/ZeroCouponCurve.h"

/**
 * @brief Factoría genérica que delega la construcción de instrumentos en builders registrados.
 * Dados unos inputs (InstrumentDescription y curva), invoca al constructor (builder) correspondiente al tipo.
 */
class Factory {
public:

    /**
     * @brief Alias para la función que construye un instrumento a partir de un InstrumentDescription y una ZeroCouponCurve. Es decir, es un puntero a la función que construye un instrumento.
     */
    using Builder = std::function<std::unique_ptr<Instrument>(const InstrumentDescription&, const ZeroCouponCurve&)>;

    /**
     * @brief Construye un instrumento (Swap o Bond) a partir de su descripción y la curva de descuento.
     * @param description Descripción del instrumento (tipo, schedule, notional, tipo de interés, etc.).
     * @param curve Curva de tipos cero cupón para descontar los flujos.
     * @return Puntero al instrumento creado (unique_ptr; quien llama es responsable de su gestión).
     * @throws std::runtime_error si el tipo de instrumento no tiene builder registrado.
     */
    std::unique_ptr<Instrument> operator()(const InstrumentDescription& description, const ZeroCouponCurve& curve) const;

    /**
     * @brief Registra un builder para un tipo de instrumento (bond o swap).
     * @param id Tipo de instrumento (InstrumentDescription::Type::bond o ::swap).
     * @param builder Función que construye el instrumento a partir de descripción y curva.
     */
    void register_constructor(const InstrumentDescription::Type& id, const Builder& builder);

    virtual ~Factory();

    /**
     * @brief Devuelve la instancia única de la factoría (singleton).
     */
    static Factory& instance() {
        static Factory factory;
        return factory;
    }

private:
    Factory();
    InstrumentDescription::Type getBuilderId(const InstrumentDescription& description) const;
    std::map<InstrumentDescription::Type, Builder> buildersMap_;
};

#endif
