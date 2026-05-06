#ifndef BOOTSTRAPPING_H
#define BOOTSTRAPPING_H

#include <map>
#include <memory>

#include <boost/date_time/gregorian/gregorian.hpp>

#include "Core/IBootstrapInstrument.h"

/**
 * @brief Calibración iterativa de la curva de factores de descuento a partir de instrumentos
 * (depósitos, swaps a la par, etc.) ordenados por vencimiento (Tema 3, bootstrapping).
 */
class Bootstrapping {
public:
    using Date = boost::gregorian::date;
    using DiscountCurveType = std::map<Date, double>;
    using InstrumentsMapType = std::map<Date, std::shared_ptr<IBootstrapInstrument>>;

    Bootstrapping() = default;
    virtual ~Bootstrapping();

    /**
     * @brief Construye la curva de factores de descuento iterativamente.
     * Los instrumentos deben estar ordenados por fecha de vencimiento (clave del map).
     * En cada paso se calcula DF con la annuity acumulada y se actualiza annuity += tau * DF.
     * @param instruments Mapa (fecha vencimiento -> instrumento bootstrap).
     * @param today Fecha de valoración; se inserta DF(today)=1.0 al inicio de la curva.
     * @return Curva (fecha -> DF). Incluye today con valor 1.0 y cada vencimiento de instruments.
     */
    DiscountCurveType operator()(const InstrumentsMapType& instruments, const Date& today) const;
};

#endif
