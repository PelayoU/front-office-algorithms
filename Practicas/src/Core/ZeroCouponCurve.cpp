#include "ZeroCouponCurve.h"
#include "actual_360.h"
#include "thirty_360.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>



/**
* @brief Constructor para inicializar la curva de tipos cero cupón. 
* @param today Fecha de valoración t=0.
* @param nodes Vector de nodos (fecha, tipo). 
* IMPORTANTE: Los tipos deben estar en formato decimal (0.05 para 5%) 
* y con CAPITALIZACIÓN CONTINUA para ser coherentes con get_discount_factor().
* @param day_count Convención de días. "30/360" o "Actual/360".
*/
ZeroCouponCurve::ZeroCouponCurve(const boost::gregorian::date& today, std::vector<Node> nodes,const std::string& day_count)
    : today_(today), nodes_(std::move(nodes)), day_count_(day_count) {

    std::sort(nodes_.begin(), nodes_.end(), [](const Node& a, const Node& b) { return a.first < b.first; }); //Ordenamos los nodos por fecha usando sort y un lambda que compara los primeros elementos de los pares (first) (la fecha) de los nodos.

    if (day_count_ != "30/360" && day_count_ != "Actual/360") //Si la convención de días no es válida, lanzamos una excepción.
        throw std::invalid_argument("ZeroCouponCurve: day_count debe ser \"30/360\" o \"Actual/360\"");
}




double ZeroCouponCurve::get_dcf_impl(const boost::gregorian::date& from, const boost::gregorian::date& to) const {
    if (day_count_ == "Actual/360") {
        Actual_360 conv;
        return conv(from, to);
    }
    Thirty_360 conv;
    return conv(from, to);
}




//Simplemente llamamos a get_dcf_impl() con nuestro atributo today_ (fecha de valoracion) y la fecha to.
double ZeroCouponCurve::get_dcf(const boost::gregorian::date& to) const {
    return get_dcf_impl(today_, to);
}




/**
 * @brief Tipo cero cupón en to. No hay interpolacion, tiene que existir el tipo cero cupon entre la fecha de valoracion (today_) y la fecha to. Dejo esto por si en el futuro implementamos interpolacion lineal.
 * @throws std::invalid_argument si to no coincide con ningún nodo.
 */
double ZeroCouponCurve::get_zero_rate(const boost::gregorian::date& to) const {
    for (const auto& node : nodes_) {
        if (node.first == to)
            return node.second;
    }
    throw std::invalid_argument("ZeroCouponCurve: fecha no encontrada en nodos");
}




/**
 * @brief Calcula el factor de descuento DF(0,T) = exp(-r*T) con capitalización continua.
 * @note Se usa get_zero_rate(to) para el tipo cero cupón en to y get_dcf(to) para el tiempo T.
 * @param to Fecha de vencimiento.
 * @return Factor de descuento DF(0,T) en formato decimal.
 * @note El tiempo T se calcula mediante get_dcf(to) respetando la convención (30/360 o Act/360) 
 * seleccionada en el constructor.
 */
double ZeroCouponCurve::get_discount_factor(const boost::gregorian::date& to) const {
    const double T = get_dcf(to);
    const double r = get_zero_rate(to);
    return std::exp(-r * T);
}

