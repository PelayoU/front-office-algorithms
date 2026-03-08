# Memoria de la práctica – Bonos, swaps y calibración de curvas

## 1.3.1 Descripción de la práctica

### Objetivo

Esta práctica implementa un motor de valoración de instrumentos financieros de renta fija en C++14. Partiendo de una arquitectura modular en dos capas (Core y Rates), se cubren tres bloques funcionales del temario:

1. **Valoración de bonos y swaps de tipos de interés** (Tema 2): descuento de flujos de caja sobre una curva de tipos cero cupón capitalizada de forma continua.
2. **Patrón Factory con registro de builders** (Tema 2, diseño): construcción de instrumentos a partir de una descripción de alto nivel sin que el cliente conozca los constructores concretos.
3. **Bootstrapping iterativo de la curva de descuento** (Tema 3): calibración de los factores de descuento nodo a nodo a partir de depósitos y swaps cotizados en el mercado (par swaps).

### Alcance

| Módulo | Clases / ficheros principales |
|--------|-------------------------------|
| **Core** | `Instrument`, `ZeroCouponCurve`, `DayCountCalculator` (`Actual_360`, `Thirty_360`), `InstrumentDescription` (con `LegDescription` y schedule explícito), `Factory`, `FactoryRegistrator` |
| **Core – Bootstrap** | `IBootstrapInstrument`, `Bootstrapping`, `Deposit`, `ParSwapBootstrap` |
| **Rates** | `Leg`, `FixedLeg`, `FloatingLeg`, `Bond`, `Swap`, `SwapBuilder`, `BondBuilder`, `ScheduleUtils` |

La curva se define por nodos discretos (fecha, tipo continuo) sin interpolación. Los flujos se descuentan con `DF(0,T) = exp(−Rc·T)`. Cada pata lleva un **schedule explícito** (vector de fechas de pago) en `LegDescription::schedule`; esas fechas deben coincidir con los nodos de la `ZeroCouponCurve` para poder descontar. Quien construye la descripción puede generar el schedule con `Rates::make_schedule(inicio, fin, meses)` o definir las fechas a mano en un `std::vector<boost::gregorian::date>`.






### Tecnología

- **Lenguaje**: C++14 (compilador Apple Clang / GCC).
- **Build system**: CMake con macros propias (`create_library`, `boost_test_project`); compilación fuera de árbol (`build/`).
- **Framework de tests**: Boost.Test — dos ejecutables independientes: `test_core` (38 casos) y `test_rates` (24 casos).
- **Librería de fechas**: Boost.Date_Time (`boost::gregorian::date`).

---

## 1.3.2 Diagrama de clases y decisiones de diseño

**Diagrama UML**: Ver [PRACTICA.md](PRACTICA.md). Dos diagramas: [assets/diagrama_valoracion_v2.png](assets/diagrama_valoracion_v2.png) (instrumentos, legs, Factory, curva, DayCountCalculator) y [assets/diagrama_bootstrapping_v2.png](assets/diagrama_bootstrapping_v2.png) (bootstrapping).



El diagrama incluye: Instrument, Leg, FixedLeg, FloatingLeg, Bond, Swap, ZeroCouponCurve, InstrumentDescription, Factory, FactoryRegistrator, SwapBuilder, BondBuilder, Bootstrapping, IBootstrapInstrument, Deposit, ParSwapBootstrap (DayCountCalculator, Actual_360, Thirty_360 en convenciones de día).

**Decisiones de diseño**: Patrón Factory + registrador; convenciones de días (DayCountCalculator); curva por nodos sin interpolación; bootstrap iterativo con annuity (IBootstrapInstrument); schedule explícito en LegDescription.

---

## 1.3.3 Evidencias de tests y resultados esperados

### Cómo ejecutar los tests

```bash
cd build
cmake ..
make -j4
./test_core    # 38 tests
./test_rates   # 24 tests
# o: ctest --output-on-failure
```

### Rutas de los tests (para revisión)

Los ejecutables se generan en `build/`. El código fuente de cada test está en:

| Ejecutable | Ruta del ejecutable | Ficheros de test (rutas desde la raíz del proyecto) |
|------------|---------------------|-----------------------------------------------------|
| **test_core** | `build/test_core` | `src/Core/tests/test_bootstrapping.cpp` (Tabla 6, bootstrap) |
| | | `src/Core/tests/test_instrumentdescription.cpp` (validación LegDescription) |
| | | `src/Core/tests/test_zerocouponcurve.cpp` (curva cero cupón) |
| | | `src/Core/tests/test_daycount.cpp` (Actual_360, Thirty_360) |
| **test_rates** | `build/test_rates` | `src/Rates/tests/test_swap_curve.cpp` (Tablas 1–4, swap) |
| | | `src/Rates/tests/test_bond_curve.cpp` (Tabla 5, bono) |
| | | `src/Rates/tests/test_factory_builders_schedule.cpp` (Factory, builders, ScheduleUtils) |

Cada fichero define una o varias suites (`BOOST_AUTO_TEST_SUITE`); los nombres de suite coinciden con los del resumen más abajo.

### Resultado obtenido

```
Running 38 test cases...
*** No errors detected

Running 24 test cases...
*** No errors detected
```

---

### Ejemplo A – Valoración de un IRS (Tablas 1–4)

**Contexto:** Swap (IRS) nocional 100.000.000 €, Actual/360, valoración 01/04/2016.

#### Tabla 1: Estructura temporal y curva de descuento (Zero Coupon)
Contiene la base de tiempo y los factores de descuento generados a partir de los tipos continuamente compuestos ($R_c$).

| Periodo | Inicio | Fin | Días | $\tau$ (Act/360) | $T_i$ Acumulado | ZC ($R_c$) | DF ($P(0,T_i)$) |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 1 | 01/04/2016 | 03/10/2016 | 185 | 0.513889 | 0.513889 | 0.0474 | 0.97593 |
| 2 | 03/10/2016 | 03/04/2017 | 182 | 0.505556 | 1.019444 | 0.0500 | 0.95030 |
| 3 | 03/04/2017 | 02/10/2017 | 182 | 0.505556 | 1.525000 | 0.0510 | 0.92517 |
| 4 | 02/10/2017 | 02/04/2018 | 182 | 0.505556 | 2.030556 | 0.0520 | 0.89977 |


Tests sobre Tabla 1 (suite `Swap_README_Excel_tests`):
- `year_fractions_dcf_match_readme_table1` — `get_dcf()` coincide con los T acumulados (tol. 1e-5).
- `discount_factors_match_readme_table1` — `get_discount_factor()` coincide con DF (tol. 1e-4).



---
#### Tabla 2: Pata flotante Euribor 6M

Cálculo de flujos variables proyectados. 

| Periodo | Inicio | Fin | Fwd Continuo ($RF_{cc}$) | Euribor 6M ($R_m$) | Cupón Flotante (CF) | PV Pata Flotante |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 1 | 01/04/2016 | 03/10/2016 | 0.047400 | 0.048000 *(Fix)* | 2465742.10 | 2406406.33 |
| 2 | 03/10/2016 | 03/04/2017 | 0.052646 | 0.053345 | 2697120.12 | 2563086.92 |
| 3 | 03/04/2017 | 02/10/2017 | 0.053014 | 0.053723 | 2716520.30 | 2513250.06 |
| 4 | 02/10/2017 | 02/04/2018 | 0.055014 | 0.055778 | 2820430.64 | 2537807.94 |
| **TOTAL** | | | | | **10699387.00** | **10020551.24** |



Tests sobre Tabla 2:
- `floating_rates_euribor6m_match_readme_table2` — tipos forward (tol. 1e-3).
- `floating_coupon_amounts_match_readme_table2` — cupones en € (tol. 1.0 €).
- `floating_leg_pv_total_match_readme_table2` — PV total = **10 020 551.24 €** (tol. 1.0 €).




#### Tabla 3: Swap a la par (K = 0.052613)

El *Swap Rate* ($K$) se calcula como: $PV_{Float} / (N \cdot \sum(\tau_i \cdot DF_i))$.

| Periodo | Inicio | Fin | Annuity ($\tau_i \cdot DF_i$) | Swap Rate Par ($K$) | Cupón Fijo Par | PV Pata Fija |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 1 | 01/04/2016 | 03/10/2016 | 0.501523 | 0.052613 | 2703723.86 | 2638661.28 |
| 2 | 03/10/2016 | 03/04/2017 | 0.480432 | 0.052613 | 2659879.69 | 2527697.15 |
| 3 | 03/04/2017 | 02/10/2017 | 0.467726 | 0.052613 | 2659879.69 | 2460847.72 |
| 4 | 02/10/2017 | 02/04/2018 | 0.454896 | 0.052613 | 2659879.69 | 2393345.09 |
| **TOTAL** | | | **1.904577** | | | **10020551.24** |


Test: `swap_at_par_near_zero_readme_table3` — con K par el NPV ≈ 0 (|NPV| < 1 €).



#### Tabla 4: Swap al 5% fuera de mercado

Posición Entidad A: Recibe Flotante, Paga Fijo (5%). 
$NPV_{Swap} = PV_{Flotante} - PV_{Fija}$.

| Periodo | Inicio | Fin | Tipo Fijo Ejemplo | Cupón Fijo (5%) | PV Pata Fija (5%) | MtM Swap (Valor) |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 1 | 01/04/2016 | 03/10/2016 | 0.0500 | 2569444.44 | 2507613.17 | - |
| 2 | 03/10/2016 | 03/04/2017 | 0.0500 | 2527777.78 | 2402160.03 | - |
| 3 | 03/04/2017 | 02/10/2017 | 0.0500 | 2527777.78 | 2338630.66 | - |
| 4 | 02/10/2017 | 02/04/2018 | 0.0500 | 2527777.78 | 2274480.51 | - |
| **TOTAL** | | | | | **9522884.37** | **+497666.88** |



Test: `swap_mtm_receive_float_pay_fixed_5_match_readme_table4` — NPV = **+497 666.88 €** (tol. 1.0 €).

---

### Ejemplo B – Valoración de un Bono (Tabla 5)

**Contexto**: Bono 2 años, nominal 100, cupón 6% anual semestral, 30/360, valoración 01/04/2014.

#### Tabla 5: Descuento de flujos del bono

| Periodo | dcf (30/360) | Rc | DF | Flujo | PV flujo |
|:---:|:---:|:---:|:---:|:---:|:---:|
| 1 | 0.5 | 0.050 | 0.9753099 | 3.0 | 2.9259297 |
| 2 | 1.0 | 0.058 | 0.9436499 | 3.0 | 2.8309498 |
| 3 | 1.5 | 0.064 | 0.9084640 | 3.0 | 2.7253920 |
| 4 | 2.0 | 0.068 | 0.8728426 | 103.0 | 89.9027912 |
| **Total** | | | | | **98.3850628** |


Tests sobre Tabla 5 (suite `Bond_curve_PDF_tests`):
- `bond_pv_matches_pdf_example` — `FixedLeg::price()` = **98.3850627729396** (tol. 1e-15).
- `bond_instrument_pv_matches_pdf` — `Bond::price()` = **98.3850627729396** (tol. 1e-15).
- `zero_curve_dcf_and_zc_continuous` — `get_dcf()` y `get_discount_factor()` exactos (tol. 1e-15).
- `factory_builds_bond_and_pv_matches_pdf` — Factory → Bond, precio = **98.3850627729396** (tol. 1e-10).

---

### Ejemplo C – Bootstrapping de curva de descuento (Tabla 6)

**Contexto del Ejercicio:** Calibración iterativa a partir de un depósito a 6 meses y tres IRS (Ejercicio 3.1).
**Convención de Días:** Actual/360 para todos los instrumentos.
**Fecha de Valoración:** 01/04/2016.


| Periodo | Instrumento | Tasa Mercado | Inicio | Fin | Días | $\tau$ (Act/360) | $T_i$ Acumulado | DF ($P(0,T_i)$) | Tipo ZC Simple | Tipo ZC Continuo ($R_c$) |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 0 | Hoy | - | - | 01/04/2016 | - | - | 0.0000000 | 1.0000000 | - | - |
| 1 | Depo 6m | 0.050 | 01/04/2016 | 03/10/2016 | 185 | 0.5138889 | 0.5138889 | 0.9749492 | 0.0500000 | 0.0493684 |
| 2 | Swap 12m | 0.055 | 03/10/2016 | 03/04/2017 | 182 | 0.5055556 | 1.0194444 | 0.9461363 | 0.0558443 | 0.0543126 |
| 3 | Swap 18m | 0.060 | 03/04/2017 | 02/10/2017 | 182 | 0.5055556 | 1.5250000 | 0.9135292 | 0.0620693 | 0.0593049 |
| 4 | Swap 2y | 0.064 | 02/10/2017 | 02/04/2018 | 182 | 0.5055556 | 2.0305556 | 0.8793138 | 0.0675925 | 0.0633390 |

Fórmula Depósito: DF = 1 / (1 + r·τ).  
Fórmula Par Swap: DF_n = (1 − K·annuity_previa) / (1 + K·τ_n).

Tests sobre Tabla 6 (suite `Bootstrapping_README_Table6_tests`):
- `bootstrap_curve_discount_factors_match_table6` — DF en los 4 nodos coinciden con la tabla (tol. 1e-6).
- `bootstrap_implied_zero_rates_continuous_match_table6` — Rc = −ln(DF)/T en los 4 nodos (tol. 1e-6).
- `deposit_6m_discount_factor_match_table6` — Deposit aislado: DF = **0.9749492** (tol. 1e-6).
- `par_swap_12m_discount_factor_match_table6` — ParSwapBootstrap con annuity correcta: DF = **0.9461363** (tol. 1e-6).

---

### Resumen de suites

| Ejecutable | Suite | Tests | Resultado |
|-----------|-------|-------|-----------|
| test_rates | `Swap_README_Excel_tests` | 8 | ✓ |
| test_rates | `Bond_curve_PDF_tests` | 4 | ✓ |
| test_rates | `ScheduleUtils_tests` | 3 | ✓ |
| test_rates | `SwapBuilder_tests` | 2 | ✓ |
| test_rates | `BondBuilder_tests` | 2 | ✓ |
| test_rates | `Factory_tests` + `FactoryRegistrator_tests` | 5 | ✓ |
| test_core | `Bootstrapping_README_Table6_tests` | 4 | ✓ |
| test_core | `InstrumentDescription_validate_tests`, `DayCount_tests`, `ZeroCouponCurve_tests`, … | 34 | ✓ |

---

## 1.3.4 Valoración del grupo, mejoras posibles y trabajo pendiente

**Valoración:** Se han cubierto los tres bloques del enunciado (valoración de bonos y swaps por descuento de flujos, Factory con builders, bootstrapping iterativo de la curva) con tests ligados a las tablas del README. El diseño con schedule explícito en la descripción permite flexibilidad y alineación con la curva de descuento.

**Mejoras posibles y trabajo pendiente:**

| Mejora / pendiente | Descripción |
|-------------------|-------------|
| **Interpolación de curva** | Hoy `ZeroCouponCurve` solo devuelve tipo/DF en nodos existentes; no hay interpolación entre nodos. Una extensión natural sería interpolación lineal (o log-lineal en DF) para fechas intermedias y así poder descontar flujos en fechas que no coincidan exactamente con los nodos. |
| **Más instrumentos en bootstrap** | Añadir otros `IBootstrapInstrument` (por ejemplo FRA, futuros) para calibrar nodos intermedios o curvas en otras monedas. |
| **GoalSeeker / tipo par** | Utilidad para resolver el tipo fijo que hace el swap a la par (dado el schedule y la curva), en lugar de tener que probar manualmente o en Excel. |
| **Calendario de festivos** | Los schedules se generan por fechas exactas (o con `make_schedule`); no se aplican convenciones de negocio (Modified Following, etc.) para mover fechas a días hábiles. |

No queda trabajo obligatorio pendiente para la entrega; los ítems anteriores son extensiones opcionales.

---

## Checklist de entrega (referencia)

- [x] **1.1.1** Código de librerías (Core: curva, Factory, bootstrap, day count, instrument description; Rates: legs, bond, swap, builders, ScheduleUtils).
- [x] **1.1.2** Tests unitarios (test_core: 38; test_rates: 24).
- [x] **1.2.1** Hoja de cálculo – Valoración del swap → `ExcelEjercicios.xlsx`.
- [x] **1.2.2** Hoja de cálculo – Calibración curva de factores de descuento → `ExcelEjercicios.xlsx`.
- [x] **1.3.1** Descripción de la práctica (completada arriba).
- [x] **1.3.2** Diagrama de clases y decisiones de diseño → PRACTICA.md, diagrama_valoracion_v2.png y diagrama_bootstrapping_v2.png.
- [x] **1.3.3** Evidencias de tests (comandos, rutas de ficheros y resultados descritos arriba).
- [x] **1.3.4** Valoración del grupo y mejoras (completada arriba).
- [x] **2.1** Valoración de un swap (datos Tema 2 / README Tablas 1–4).
- [x] **2.2** Calibración de curva (datos Tema 3 / README Tabla 6).
