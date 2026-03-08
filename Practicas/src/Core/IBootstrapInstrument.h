#ifndef IBOOTSTRAPINSTRUMENT_H
#define IBOOTSTRAPINSTRUMENT_H

#include <boost/date_time/gregorian/gregorian.hpp>

/**
 * @brief Interfaz para instrumentos que participan en el bootstrapping de la curva de descuento.
 * Cada instrumento aporta un factor de descuento en su fecha de vencimiento a partir de la
 * annuity acumulada (suma de tau_i * DF_i) de los nodos ya calibrados.
 */
class IBootstrapInstrument {
public:
    virtual ~IBootstrapInstrument() = default;

    /**
     * @brief Fecha de vencimiento del instrumento (nodo de la curva que este instrumento calibra).
     */
    virtual boost::gregorian::date getMaturityDate() const = 0;

    /**
     * @brief Fracción de año del último periodo (desde el nodo anterior hasta el vencimiento).
     * Se usa para actualizar la annuity tras añadir este nodo: annuity += getTau() * DF.
     */
    virtual double getTau() const = 0;

    /**
     * @brief Calcula el factor de descuento en el vencimiento dado la annuity previa.
     * @param pre_annuity Suma de (tau_i * DF_i) para todos los periodos ya calibrados.
     * @return Factor de descuento P(0,T) en el vencimiento de este instrumento.
     */
    virtual double computeDiscountFactor(double pre_annuity) const = 0;
};

#endif
