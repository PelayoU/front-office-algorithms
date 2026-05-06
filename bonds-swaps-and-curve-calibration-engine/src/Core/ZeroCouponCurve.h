#ifndef ZEROCOUPONCURVE_H
#define ZEROCOUPONCURVE_H

#include <string>
#include <vector>
#include <utility>
#include <boost/date_time/gregorian/gregorian.hpp>

/**
 * @brief Curva de tipos cero cupón por nodos (fecha, tipo) con capitalización continua.
 *
 * - get_dcf(to): fracción de año entre today y to (convención elegida en el constructor).
 * - get_zero_rate(to): tipo cero cupón en to; solo lookup por fecha exacta (sin interpolación).
 * - get_discount_factor(to): factor de descuento DF(0,T) = exp(-r*T) según ecuación 2.4 del tema.
 */
class ZeroCouponCurve {
public:

    /**
     * @brief Alias para el par (fecha, tipo cero cupón en decimal). Asi no tenemos que poner std::pair<boost::gregorian::date, double> en todos lados.
     */
    using Node = std::pair<boost::gregorian::date, double>;


    /**
    * @brief Constructor para inicializar la curva de tipos cero cupón. 
    * @param today Fecha de valoración t=0.
    * @param nodes Vector de nodos (fecha, tipo). 
    * IMPORTANTE: Los tipos deben estar en formato decimal (0.05 para 5%) 
    * y con CAPITALIZACIÓN CONTINUA para ser coherentes con get_discount_factor().
    * @param day_count Convención de días. "30/360" o "Actual/360".
    */
    ZeroCouponCurve(const boost::gregorian::date& today, std::vector<Node> nodes, const std::string& day_count = "30/360");



    /**
     * @brief Fracción de año entre la fecha de valoración (today_) y to (con la convención pasada al constructor).
     */
    double get_dcf(const boost::gregorian::date& to) const;




    /**
    * @brief Tipo cero cupón en to. No hay interpolacion, tiene que existir el tipo cero cupon entre la fecha de valoracion (today_) y la fecha to. Dejo esto por si en el futuro implementamos interpolacion lineal.
    * @throws std::invalid_argument si to no coincide con ningún nodo.
    */
    double get_zero_rate(const boost::gregorian::date& to) const;




    /**
     * @brief Factor de descuento DF(0,T) = exp(-r*T) con capitalización continua. 
     * @note Se usa get_zero_rate(to) para el tipo cero cupón en to y get_dcf(to) para el tiempo T.
     * @param to Fecha de vencimiento.
     * @return Factor de descuento DF(0,T) en formato decimal.
     * @note El tiempo T se calcula mediante get_dcf(to) respetando la convención (30/360 o Act/360) 
     * seleccionada en el constructor.
     * @throws std::invalid_argument si to no coincide con ningún nodo. Es decir, si la fecha de vencimiento no existe en la curva.
     */
    double get_discount_factor(const boost::gregorian::date& to) const;



private:
    boost::gregorian::date today_;
    std::vector<Node> nodes_;
    std::string day_count_;

    // Metodo privado para calcular la fracción de año entre dos fechas usando la convención pasada al constructor.
    double get_dcf_impl(const boost::gregorian::date& from, const boost::gregorian::date& to) const;
};

#endif


