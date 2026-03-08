# README

### 1. Plantilla de Arquitectura Escalable (Directorio Raíz)

Plaintext

```
sqf-master/
├── CMakeLists.txt                 # Orquestador principal: add_subdirectory(Core) y demás módulos.
├── README.md                      # Documentación del repositorio.
├── build/                         # Directorio para "out-of-source build". Excluido en .gitignore.
├── cmake-lib/                     # Infraestructura de macros de CMake.
│   ├── CMakeAuxFunctions.cmake    # Contiene la macro create_library.
│   └── CMakeBoostTest.cmake       # Contiene la macro boost_test_project.
└── src/
    ├── CMakeLists.txt             # Índice de submódulos de código fuente.
    │
    ├── Core/                      # FUNDAMENTOS CUANTITATIVOS (Base de todo)
    │   ├── CMakeLists.txt         # -> create_library(NAME core)
    │   ├── Instrument.h
    │   ├── DayCountCalculator.h / DayCountCalculator.cpp
    │   ├── ZeroCouponCurve.h / ZeroCouponCurve.cpp
    │   ├── Index.h / Index.cpp
    │   ├── Factory.h / Factory.cpp
    │   ├── FactoryRegistrator.h
    │   ├── InstrumentDescription.h
    │   └── tests/
    │       ├── CMakeLists.txt     # -> boost_test_project(NAME test_core DEPS core)
    │       ├── test_main.cpp      # Define BOOST_TEST_MODULE
    │       ├── test_daycount.cpp
    │       └── test_curves.cpp
    │
    ├── Rates/                     # RENTA FIJA Y SWAPS (Tema 2)
    │   ├── CMakeLists.txt         # -> create_library(NAME rates DEPS core)
    │   ├── Bond.h / Bond.cpp
    │   ├── BondBuilder.h / BondBuilder.cpp
    │   ├── SwapIRS.h / SwapIRS.cpp
    │   ├── SwapBuilder.h / SwapBuilder.cpp
    │   ├── FixedLeg.h / FixedLeg.cpp
    │   ├── FloatingLeg.h / FloatingLeg.cpp
    │   └── tests/
    │       ├── CMakeLists.txt     # -> boost_test_project(NAME test_rates DEPS core rates)
    │       ├── test_main.cpp
    │       ├── test_bond.cpp
    │       └── test_swap_irs.cpp
    │
    ├── Options/                   # DERIVADOS Y VOLATILIDAD (Futuro módulo)
    │   ├── CMakeLists.txt         # -> create_library(NAME options DEPS core)
    │   ├── BlackScholes.h / BlackScholes.cpp
    │   └── tests/
    │
    └── Crypto/                    # DEFI Y MERCADOS DIGITALES (Futuro módulo)
        ├── CMakeLists.txt         # -> create_library(NAME crypto DEPS core rates)
        ├── PerpetualSwap.h / PerpetualSwap.cpp
        └── tests/
```

### 2. Archivo `README.md` Profesional

Este archivo está redactado con rigor técnico, detallando los comandos precisos para explotar la concurrencia de la arquitectura ARM64 de tu equipo local.


# Quantitative Finance & Fintech Pricing Engine

Motor de valoración de instrumentos financieros implementado en C++ (estándar `gnu++14` / `C++14`). El sistema utiliza una arquitectura modular fundamentada en el patrón Factory con inyección de dependencias (Inversión de Control) y una jerarquía de clases estricta para el descuento de flujos de caja.

## Arquitectura del Sistema

El proyecto está diseñado bajo un paradigma de alta cohesión y bajo acoplamiento, segmentado en los siguientes dominios de negocio:

* **Core**: Proporciona las interfaces virtuales (`Instrument`), las calculadoras de fracciones de año (`Actual_360`, `30_360`), las curvas de tipos de interés y la infraestructura de inyección (`Factory` y `FactoryRegistrator`).
* **Rates**: Motor de valoración para instrumentos de Renta Fija tradicional (Bonos, IRS Swaps) y sus constructores asociados (`BondBuilder`, `SwapBuilder`).
* **Options** / **Crypto**: Módulos extensibles para derivados no lineales y activos digitales (implementación futura).

## Infraestructura de Construcción

El sistema utiliza **CMake** para la generación del entorno de compilación (Out-of-source builds) y automatiza el empaquetado de librerías dinámicas (`.dylib`) mediante la macro `create_library()`.

### Dependencias
* Compilador compatible con `C++14` (Apple Clang / GCC).
* `CMake` (versión mínima 3.0).
* `Boost` (componentes principales y `unit_test_framework`).

## Flujo de Trabajo y Compilación

Toda la interacción de compilación debe realizarse estrictamente desde el directorio de construcción para no contaminar el árbol de código fuente.

Toda la interacción de compilación debe realizarse estrictamente desde el directorio de construcción para no contaminar el árbol de código fuente.

**1 - Configuración Inicial (Generación de Makefiles):**

Ejecutar unicamente tras clonar el repositorio o al añadir/eliminar ficheros físicos (`.cpp` / `.hpp`) para que `file(GLOB)` reevalúe el directorio. 


```bash
mkdir -p build && cd build cmake ..
```


**2 - Compilación (Incremental):** Para compilar el proyecto aprovechando la concurrencia a nivel de hardware (recomendado para arquitecturas ARM64 multiproceso):

```bash
cd build
make -j$(sysctl -n hw.ncpu)

```

Para compilar exclusivamente un módulo en desarrollo:

```bash
make test_rates -j$(sysctl -n hw.ncpu)
```


## Ejecución de Unit Tests

Las pruebas de regresión se desarrollan bajo **Boost.Test**. Cada módulo de negocio genera su propio ejecutable de test asilado mediante la macro `boost_test_project()`.

- **Ejecutar todos los tests de un módulo específico:**

```bash
./build/src/Rates/tests/test_rates
```

- **Ejecutar una suite de pruebas concreta (filtrado):**


```bash
./build/src/Rates/tests/test_rates --run_test=Suite_Bonos/*
```


- **Ejecutar todo el framework de tests del proyecto:**

```bash
cd build
ctest --output-on-failure
```






## Datos de Validación para Tests Unitarios (Interest Rate Swap)

### EJEMPLO SWAP
**Contexto del Ejercicio:** Valoración de un Swap (IRS) con nocional de 100.000.000€. 
**Convención de Días:** Actual/360.
**Fecha de Valoración:** 01/04/2016.

> **Nota para el generador de código (Cursor/AI):** Utiliza los valores de estas tablas como `expected_values` en los tests de Boost (`BOOST_TEST` o `BOOST_CHECK_CLOSE`). Emplea una tolerancia (`boost::test_tools::tolerance`) adecuada para operaciones de coma flotante (ej. `1e-4` para tasas, `1.0` para valores absolutos en euros).

### Tabla 1: Estructura Temporal y Curva de Descuento (Zero Coupon)
Contiene la base de tiempo y los factores de descuento generados a partir de los tipos continuamente compuestos ($R_c$).

| Periodo | Inicio | Fin | Días | $\tau$ (Act/360) | $T_i$ Acumulado | ZC ($R_c$) | DF ($P(0,T_i)$) |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 1 | 01/04/2016 | 03/10/2016 | 185 | 0.513889 | 0.513889 | 0.0474 | 0.97593 |
| 2 | 03/10/2016 | 03/04/2017 | 182 | 0.505556 | 1.019444 | 0.0500 | 0.95030 |
| 3 | 03/04/2017 | 02/10/2017 | 182 | 0.505556 | 1.525000 | 0.0510 | 0.92517 |
| 4 | 02/10/2017 | 02/04/2018 | 182 | 0.505556 | 2.030556 | 0.0520 | 0.89977 |

### Tabla 2: Valoración Pata Flotante (Euribor 6M)
Cálculo de flujos variables proyectados. El Periodo 1 utiliza el *fixing* histórico.

| Periodo | Inicio | Fin | Fwd Continuo ($RF_{cc}$) | Euribor 6M ($R_m$) | Cupón Flotante (CF) | PV Pata Flotante |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 1 | 01/04/2016 | 03/10/2016 | 0.047400 | 0.048000 *(Fix)* | 2465742.10 | 2406406.33 |
| 2 | 03/10/2016 | 03/04/2017 | 0.052646 | 0.053345 | 2697120.12 | 2563086.92 |
| 3 | 03/04/2017 | 02/10/2017 | 0.053014 | 0.053723 | 2716520.30 | 2513250.06 |
| 4 | 02/10/2017 | 02/04/2018 | 0.055014 | 0.055778 | 2820430.64 | 2537807.94 |
| **TOTAL** | | | | | **10699387.00** | **10020551.24** |

### Tabla 3: Valoración Pata Fija a la PAR (Swap Valorado a Cero)
El *Swap Rate* ($K$) se calcula como: $PV_{Float} / (N \cdot \sum(\tau_i \cdot DF_i))$.

| Periodo | Inicio | Fin | Annuity ($\tau_i \cdot DF_i$) | Swap Rate Par ($K$) | Cupón Fijo Par | PV Pata Fija |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 1 | 01/04/2016 | 03/10/2016 | 0.501523 | 0.052613 | 2703723.86 | 2638661.28 |
| 2 | 03/10/2016 | 03/04/2017 | 0.480432 | 0.052613 | 2659879.69 | 2527697.15 |
| 3 | 03/04/2017 | 02/10/2017 | 0.467726 | 0.052613 | 2659879.69 | 2460847.72 |
| 4 | 02/10/2017 | 02/04/2018 | 0.454896 | 0.052613 | 2659879.69 | 2393345.09 |
| **TOTAL** | | | **1.904577** | | | **10020551.24** |

### Tabla 4: Valoración Swap Fuera de Mercado (Ejemplo Profesor al 5%)
Posición Entidad A: Recibe Flotante, Paga Fijo (5%). 
$NPV_{Swap} = PV_{Flotante} - PV_{Fija}$.

| Periodo | Inicio | Fin | Tipo Fijo Ejemplo | Cupón Fijo (5%) | PV Pata Fija (5%) | MtM Swap (Valor) |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 1 | 01/04/2016 | 03/10/2016 | 0.0500 | 2569444.44 | 2507613.17 | - |
| 2 | 03/10/2016 | 03/04/2017 | 0.0500 | 2527777.78 | 2402160.03 | - |
| 3 | 03/04/2017 | 02/10/2017 | 0.0500 | 2527777.78 | 2338630.66 | - |
| 4 | 02/10/2017 | 02/04/2018 | 0.0500 | 2527777.78 | 2274480.51 | - |
| **TOTAL** | | | | | **9522884.37** | **+497666.88** |





### EJEMPLO BONO

### Tabla 5: Valoración de Bono Cupón Fijo (Descuento de Flujos)

**Contexto del Ejercicio:** Valoración de un bono fijo con nocional de 100 y cupón del 6% anual (pagadero semestralmente).
**Convención de Días:** 30/360.

> **Nota para el generador de código (Cursor/AI):** Utiliza esta tabla como `expected_values` para los tests de Boost de la clase `FixedLeg` o `Bond`. Aplica una tolerancia de `1e-6` para los factores de descuento y `1e-4` para los valores presentes (PV).

| Periodo | dcf (30/360) | ZC ($R_c$) | DF ($P(0,T)$) | $\tau$ (30/360) | Flujo (CF) | PV Flujo |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 1 | 0.5 | 0.050 | 0.9753099 | 0.5 | 3.0 | 2.9259297 |
| 2 | 1.0 | 0.058 | 0.9436499 | 0.5 | 3.0 | 2.8309498 |
| 3 | 1.5 | 0.064 | 0.9084640 | 0.5 | 3.0 | 2.7253920 |
| 4 | 2.0 | 0.068 | 0.8728426 | 0.5 | 103.0 | 89.9027912 |
| **TOTAL** | | | | | | **98.3850628** |





### EJEMPLO BOOTSTRAPING

### Tabla 6: Calibración de Curva Zero Coupon (Bootstrapping)

**Contexto del Ejercicio:** Calibración iterativa a partir de un depósito a 6 meses y tres IRS (Ejercicio 3.1).
**Convención de Días:** Actual/360 para todos los instrumentos.
**Fecha de Valoración:** 01/04/2016.

> **Nota para el generador de código (Cursor/AI):** Utiliza esta tabla para generar los tests unitarios de la clase `Bootstrapping`. Verifica que al pasar el vector de instrumentos (Depósito y Swaps), el método iterativo de calibración produce exactamente la misma curva de factores de descuento (`DF`) y tipos cero continuamente compuestos (`Tipo ZC Continuo`). Emplea una tolerancia de `1e-6` para las aserciones de `BOOST_TEST`.

| Periodo | Instrumento | Tasa Mercado | Inicio | Fin | Días | $\tau$ (Act/360) | $T_i$ Acumulado | DF ($P(0,T_i)$) | Tipo ZC Simple | Tipo ZC Continuo ($R_c$) |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 0 | Hoy | - | - | 01/04/2016 | - | - | 0.0000000 | 1.0000000 | - | - |
| 1 | Depo 6m | 0.050 | 01/04/2016 | 03/10/2016 | 185 | 0.5138889 | 0.5138889 | 0.9749492 | 0.0500000 | 0.0493684 |
| 2 | Swap 12m | 0.055 | 03/10/2016 | 03/04/2017 | 182 | 0.5055556 | 1.0194444 | 0.9461363 | 0.0558443 | 0.0543126 |
| 3 | Swap 18m | 0.060 | 03/04/2017 | 02/10/2017 | 182 | 0.5055556 | 1.5250000 | 0.9135292 | 0.0620693 | 0.0593049 |
| 4 | Swap 2y | 0.064 | 02/10/2017 | 02/04/2018 | 182 | 0.5055556 | 2.0305556 | 0.8793138 | 0.0675925 | 0.0633390 |