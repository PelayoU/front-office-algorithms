#ifndef INSTRUMENT_H
#define INSTRUMENT_H

/**
 * @brief Clase base abstracta para todos los instrumentos financieros valorables por descuento de flujos.
 * Define la interfaz común: cualquier instrumento (Bond, Swap, etc.) debe poder calcular su precio.
 * Las clases derivadas (Bond, Swap) implementan price() con la lógica específica de valoración.
 * El objetivo sera obligar a las clases derivadas (Bond, Swap) a implementar el metodo price() (que calcula el precio/valor actual del instrumento por descuento de flujos). 
 * Como la clase Instrument es abstracta, no se puede instanciar directamente, solo se puede usar como base para las clases derivadas.
 */
class Instrument {
public:
    virtual ~Instrument() = default;

    /**
     * @brief Calcula el precio/valor actual del instrumento por descuento de flujos.
     * @return Valor presente del instrumento.
     */
    virtual double price() const = 0;
};

#endif
