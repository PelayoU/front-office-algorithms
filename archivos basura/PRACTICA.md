# Práctica – Documentación de entrega

## Diagrama de clases y decisiones de diseño

Se incluyen dos diagramas para mayor claridad: valoración (instrumentos, legs, Factory, curva) y bootstrapping.

### 1. Valoración: Instrument, Leg, DayCountCalculator, ZeroCouponCurve, Factory

Instrument (abstracta) → Bond y Swap. Leg (abstracta) → FixedLeg y FloatingLeg. DayCountCalculator → Actual_360 y Thirty_360. ZeroCouponCurve con get_dcf / get_zero_rate / get_discount_factor. Factory singleton con FactoryRegistrator\<T\>, SwapBuilder y BondBuilder. InstrumentDescription con LegDescription (payer y receiver).

![Diagrama de valoración](assets/diagrama_valoracion_v2.png)

### 2. Bootstrapping: Bootstrapping, IBootstrapInstrument, Deposit, ParSwapBootstrap

Bootstrapping agrega IBootstrapInstrument (interfaz) y construye iterativamente la curva de factores de descuento. Deposit (DF = 1/(1+r·τ)) y ParSwapBootstrap (DF_n = (1−K·annuity)/(1+K·τ_n)) implementan la interfaz.

![Diagrama de bootstrapping](assets/diagrama_bootstrapping_v2.png)

### Decisiones de diseño

- **Patrón Factory + registrador**: `Factory` singleton con `register_constructor` y plantilla `FactoryRegistrator<T>` para registrar builders (`SwapBuilder`, `BondBuilder`) en tiempo de compilación/arranque.
- **Convenciones de días**: Uso de `DayCountCalculator` con implementaciones como `Actual_360` y `Thirty_360` para DCF; la curva expone `get_dcf(Date)`.
- **Curva por nodos sin interpolación**: `ZeroCouponCurve` definida por nodos (fechas + tipos/DF); no se aplica interpolación entre nodos.
- **Bootstrap iterativo con annuity**: `Bootstrapping` utiliza instrumentos que implementan `IBootstrapInstrument` (`Deposit`, `ParSwapBootstrap`); en par swaps se usa la annuity para resolver el tipo cero en cada nodo de forma iterativa.

---

## Checklist de entrega (referencia)

- [x] **1.1.1** Código de librerías (Core, Rates).
- [x] **1.1.2** Tests unitarios (test_core, test_rates).
- [x] **1.2.1** Hoja de cálculo – Valoración del swap → `ExcelEjercicios.xlsx`.
- [x] **1.2.2** Hoja de cálculo – Calibración curva de factores de descuento → `ExcelEjercicios.xlsx`.
- [ ] **1.3.1** Descripción de la práctica (completar en MEMORIA.md).
- [x] **1.3.2** Diagrama de clases y decisiones de diseño (este documento + MEMORIA.md).
- [x] **1.3.3** Evidencias de tests (MEMORIA.md).
- [ ] **1.3.4** Valoración del grupo y mejoras (completar en MEMORIA.md).
- [x] **2.1** Valoración de un swap (README Tablas 1–4).
- [x] **2.2** Calibración de curva (README Tabla 6).
