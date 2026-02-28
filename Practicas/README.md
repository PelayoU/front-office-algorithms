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


